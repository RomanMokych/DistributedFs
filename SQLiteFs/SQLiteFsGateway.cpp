//
//  SQLiteFsGateway.cpp
//  DistributedFs
//
//  Created by Роман on 8/8/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include "SQLiteFsGateway.h"

#include "Utils.h"

namespace
{
    void ExecuteSqliteQuery(sqlite3* sqlite, const char* query)
    {
        char* errorMsg = nullptr;
        int error = sqlite3_exec(sqlite, query, nullptr, nullptr, &errorMsg);
        if (error != SQLITE_OK)
        {
            if (errorMsg)
                sqlite3_free(errorMsg);
            
            THROW("Can't create Links table");
        }
    }
    
    void PrepareSqliteStmt(sqlite3* sqlite, sqlite3_stmt** stmt, const char* query)
    {
        int error = sqlite3_prepare(sqlite, query, -1, stmt, nullptr);
        if (error != SQLITE_OK)
        {
            THROW("Can't create select request");
        }
    }
    
    class SqliteStmtReseter
    {
    public:
        SqliteStmtReseter(sqlite3_stmt* stmt)
        : m_stmt(stmt)
        {}
        
        ~SqliteStmtReseter()
        {
            sqlite3_reset(m_stmt);
        }
        
    private:
        sqlite3_stmt* m_stmt;
        
    };
}

namespace dfs
{

SQLiteFsGateway::SQLiteFsGateway(const Path& dbPath)
    : m_sqlite(nullptr)
{
    int error = sqlite3_open(dbPath.c_str(), &m_sqlite);
    if (error != SQLITE_OK)
    {
        THROW("Can't opend db");
    }
    
    ExecuteSqliteQuery(m_sqlite, "CREATE TABLE Links (id INTEGER PRIMARY KEY ASC, parentId INT NOT NULL, itemId INT NOT NULL, name TEXT NOT NULL, UNIQUE (parentId, name));");
    ExecuteSqliteQuery(m_sqlite, "CREATE TABLE Items (id INTEGER PRIMARY KEY ASC, type INT NOT NULL, concreteItemId INT NOT NULL, permissions INT NOT NULL);");
    ExecuteSqliteQuery(m_sqlite, "CREATE TABLE Folders (id INTEGER PRIMARY KEY ASC, dummy INT);");
    
    PrepareSqliteStmt(m_sqlite, &m_selectLinkQueryWithParentIdAndName, "SELECT * FROM Links WHERE parentId = ? AND name = ?;");
    PrepareSqliteStmt(m_sqlite, &m_selectItemQueryWithId, "SELECT * FROM Items WHERE id = ?;");
    PrepareSqliteStmt(m_sqlite, &m_selectFolderQueryWithId, "SELECT * FROM Folders WHERE id = ?;");
    
    PrepareSqliteStmt(m_sqlite, &m_insertFolderQuery, "INSERT INTO Folders (dummy) VALUES (0);");
    PrepareSqliteStmt(m_sqlite, &m_insertItemQuery, "INSERT INTO Items (type, concreteItemId, permissions) VALUES (?, ?, ?);");
    PrepareSqliteStmt(m_sqlite, &m_insertLinkQuery, "INSERT INTO Links (parentId, itemId, name) VALUES (?, ?, ?);");
    
    ExecuteSqliteQuery(m_sqlite, "INSERT INTO Folders (dummy) VALUES (0);"); //super root
    m_superRootFolderId = static_cast<int>(sqlite3_last_insert_rowid(m_sqlite));

    createFolder(m_superRootFolderId, "/", Permissions::kAll);
}
    
SQLiteFsGateway::~SQLiteFsGateway()
{
    sqlite3_close(m_sqlite);
}

SQLiteEntities::Folder SQLiteFsGateway::getFolderByPath(const Path& folderPath)
{
    SQLiteEntities::Item item = getItemByPath(folderPath);
    if (item.type != SQLiteEntities::ItemType::Folder)
    {
        throw SQLiteFsException(FsError::kFileNotFound, "File is not folder");
    }
    
    return getFolderById(item.concreteItemId);
}
    
SQLiteEntities::Item SQLiteFsGateway::getItemByPath(const Path& itemPath)
{
    SQLiteEntities::Link link;
    int parentId = m_superRootFolderId;
    
    auto pathIt = itemPath.begin();
    while (pathIt != itemPath.end())
    {
        link = getLink(parentId, pathIt->leaf());
        parentId = link.parentFolderId;
        pathIt++;
    }
    
    return getItemById(link.itemId);
}
    
SQLiteEntities::Link SQLiteFsGateway::getLink(int parentId, const Path& name)
{
    SqliteStmtReseter reseter(m_selectLinkQueryWithParentIdAndName);
    
    int error = sqlite3_bind_int(m_selectLinkQueryWithParentIdAndName, 1, parentId);
    if (error != SQLITE_OK)
    {
        THROW("can't bind param");
    }

    error = sqlite3_bind_text(m_selectLinkQueryWithParentIdAndName, 2, name.c_str(), -1, SQLITE_STATIC);
    if (error != SQLITE_OK)
    {
        THROW("can't bind param");
    }
    
    error = sqlite3_step(m_selectLinkQueryWithParentIdAndName);
    if (error == SQLITE_ROW)
    {
        SQLiteEntities::Link link;
        link.id = sqlite3_column_int(m_selectLinkQueryWithParentIdAndName, 0);
        link.parentFolderId = sqlite3_column_int(m_selectLinkQueryWithParentIdAndName, 1);
        link.itemId = sqlite3_column_int(m_selectLinkQueryWithParentIdAndName, 2);
        link.name = reinterpret_cast<const char*>(sqlite3_column_text(m_selectLinkQueryWithParentIdAndName, 3));
        
        return link;
    }
    
    throw SQLiteFsException(FsError::kFileNotFound, "Can't find file with such name");
}
    
SQLiteEntities::Folder SQLiteFsGateway::getFolderById(int folderId)
{
    SqliteStmtReseter reseter(m_selectFolderQueryWithId);
    
    sqlite3_bind_int(m_selectFolderQueryWithId, 1, folderId);
    
    int error = sqlite3_step(m_selectFolderQueryWithId);
    if (error == SQLITE_ROW)
    {
        SQLiteEntities::Folder folder;
        folder.id = sqlite3_column_int(m_selectFolderQueryWithId, 0);
        
        return folder;
    }
    
    throw SQLiteFsException(FsError::kFileNotFound, "Folder was not found");
}
    
SQLiteEntities::Item SQLiteFsGateway::getItemById(int itemId)
{
    SqliteStmtReseter reseter(m_selectItemQueryWithId);
    
    sqlite3_bind_int(m_selectItemQueryWithId, 1, itemId);
    
    int error = sqlite3_step(m_selectItemQueryWithId);
    if (error == SQLITE_ROW)
    {
        SQLiteEntities::Item item;
        item.id = sqlite3_column_int(m_selectItemQueryWithId, 0);
        item.type = static_cast<SQLiteEntities::ItemType>(sqlite3_column_int(m_selectItemQueryWithId, 1));
        item.concreteItemId = sqlite3_column_int(m_selectItemQueryWithId, 2);
        item.permissions = static_cast<Permissions>(sqlite3_column_int(m_selectItemQueryWithId, 3));
        
        return item;
    }
    
    throw SQLiteFsException(FsError::kFileNotFound, "item was not found");
}

void SQLiteFsGateway::createFolder(int parentFolderId, const Path& newFolderName, Permissions permissions)
{
    SqliteStmtReseter folderReseter(m_insertFolderQuery);
    
    int error = sqlite3_step(m_insertFolderQuery);
    if (error != SQLITE_DONE)
    {
        THROW("can't insert");
    }
    
    int newFolderId = static_cast<int>(sqlite3_last_insert_rowid(m_sqlite));
    
    SqliteStmtReseter itemReseter(m_insertItemQuery);
    
    sqlite3_bind_int(m_insertItemQuery, 1, static_cast<int>(SQLiteEntities::ItemType::Folder));
    sqlite3_bind_int(m_insertItemQuery, 2, newFolderId);
    sqlite3_bind_int(m_insertItemQuery, 3, static_cast<int>(permissions));
    
    error = sqlite3_step(m_insertItemQuery);
    if (error != SQLITE_DONE)
    {
        THROW("can't insert");
    }
    
    int newItemId = static_cast<int>(sqlite3_last_insert_rowid(m_sqlite));
    
    SqliteStmtReseter linkReseter(m_insertLinkQuery);
    
    sqlite3_bind_int(m_insertLinkQuery, 1, parentFolderId);
    sqlite3_bind_int(m_insertLinkQuery, 2, newItemId);
    sqlite3_bind_text(m_insertLinkQuery, 3, newFolderName.c_str(), -1, SQLITE_STATIC);
    
    error = sqlite3_step(m_insertLinkQuery);
    if (error != SQLITE_DONE)
    {
        throw SQLiteFsException(FsError::kFileExists, "Such file exists");
    }
}

}
