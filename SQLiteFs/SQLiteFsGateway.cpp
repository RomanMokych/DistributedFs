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
    ExecuteSqliteQuery(m_sqlite, "CREATE TABLE ExtendedAttributes (id INTEGER PRIMARY KEY ASC, itemId INT NOT NULL, name TEXT NOT NULL, value BLOB NOT NULL, UNIQUE (itemId, name));");
    
    m_selectLinkQueryWithParentIdAndName.reset(new SqliteStatement("SELECT id, parentId, itemId, name FROM Links WHERE parentId = ? AND name = ?;", m_sqlite));
    m_selectItemQueryWithId.reset(new SqliteStatement("SELECT id, type, concreteItemId, permissions FROM Items WHERE id = ?;", m_sqlite));
    m_selectFolderQueryWithId.reset(new SqliteStatement("SELECT id, dummy  FROM Folders WHERE id = ?;", m_sqlite));
    m_selectLinksWithParentId.reset(new SqliteStatement("SELECT Links.name, Items.type, Items.permissions FROM Links JOIN Items ON Links.itemId = Items.id WHERE parentId = ?;", m_sqlite));
    
    m_insertFolderQuery.reset(new SqliteStatement("INSERT INTO Folders (dummy) VALUES (0);", m_sqlite));
    m_insertItemQuery.reset(new SqliteStatement("INSERT INTO Items (type, concreteItemId, permissions) VALUES (?, ?, ?);", m_sqlite));
    m_insertLinkQuery.reset(new SqliteStatement("INSERT INTO Links (parentId, itemId, name) VALUES (?, ?, ?);", m_sqlite));
    
    m_insertExtendedAttributeQuery.reset(new SqliteStatement("INSERT INTO ExtendedAttributes (itemId, name, value) VALUES (?, ?, ?);", m_sqlite));
    m_deleteExtendedAttributeByItemIdAndNameQuery.reset(new SqliteStatement("DELETE FROM ExtendedAttributes WHERE itemId = ? AND name = ?;", m_sqlite));
    m_selectExtendedAttributesByItemIdQuery.reset(new SqliteStatement("SELECT id, itemId, name, value FROM ExtendedAttributes WHERE itemId = ?;", m_sqlite));
    m_selectExtendedAttributeByItemIdAndNameQuery.reset(new SqliteStatement("SELECT id, itemId, name, value FROM ExtendedAttributes WHERE itemId = ? AND name = ?;", m_sqlite));
    
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
       
        SQLiteEntities::Item item = getItemById(link.itemId);
        if (item.type != SQLiteEntities::ItemType::Folder)
        {
            throw SQLiteFsException(FsError::kFileHasWrongType, "File has wrong type");
        }
        
        parentId = item.concreteItemId;
        pathIt++;
    }
    
    return getItemById(link.itemId);
}
    
SQLiteEntities::Link SQLiteFsGateway::getLink(int parentId, const Path& name)
{
    SqliteStmtReseter reseter(m_selectLinkQueryWithParentIdAndName->get());
    
    int error = sqlite3_bind_int(m_selectLinkQueryWithParentIdAndName->get(), 1, parentId);
    if (error != SQLITE_OK)
    {
        THROW("can't bind param");
    }

    error = sqlite3_bind_text(m_selectLinkQueryWithParentIdAndName->get(), 2, name.c_str(), -1, SQLITE_STATIC);
    if (error != SQLITE_OK)
    {
        THROW("can't bind param");
    }
    
    error = sqlite3_step(m_selectLinkQueryWithParentIdAndName->get());
    if (error == SQLITE_ROW)
    {
        SQLiteEntities::Link link;
        link.id = sqlite3_column_int(m_selectLinkQueryWithParentIdAndName->get(), 0);
        link.parentFolderId = sqlite3_column_int(m_selectLinkQueryWithParentIdAndName->get(), 1);
        link.itemId = sqlite3_column_int(m_selectLinkQueryWithParentIdAndName->get(), 2);
        link.name = reinterpret_cast<const char*>(sqlite3_column_text(m_selectLinkQueryWithParentIdAndName->get(), 3));
        
        return link;
    }
    
    throw SQLiteFsException(FsError::kFileNotFound, "Can't find file with such name");
}
    
SQLiteEntities::Folder SQLiteFsGateway::getFolderById(int folderId)
{
    SqliteStmtReseter reseter(m_selectFolderQueryWithId->get());
    
    sqlite3_bind_int(m_selectFolderQueryWithId->get(), 1, folderId);
    
    int error = sqlite3_step(m_selectFolderQueryWithId->get());
    if (error == SQLITE_ROW)
    {
        SQLiteEntities::Folder folder;
        folder.id = sqlite3_column_int(m_selectFolderQueryWithId->get(), 0);
        
        return folder;
    }
    
    throw SQLiteFsException(FsError::kFileNotFound, "Folder was not found");
}
    
SQLiteEntities::Item SQLiteFsGateway::getItemById(int itemId)
{
    SqliteStmtReseter reseter(m_selectItemQueryWithId->get());
    
    sqlite3_bind_int(m_selectItemQueryWithId->get(), 1, itemId);
    
    int error = sqlite3_step(m_selectItemQueryWithId->get());
    if (error == SQLITE_ROW)
    {
        SQLiteEntities::Item item;
        item.id = sqlite3_column_int(m_selectItemQueryWithId->get(), 0);
        item.type = static_cast<SQLiteEntities::ItemType>(sqlite3_column_int(m_selectItemQueryWithId->get(), 1));
        item.concreteItemId = sqlite3_column_int(m_selectItemQueryWithId->get(), 2);
        item.permissions = static_cast<Permissions>(sqlite3_column_int(m_selectItemQueryWithId->get(), 3));
        
        return item;
    }
    
    throw SQLiteFsException(FsError::kFileNotFound, "item was not found");
}

void SQLiteFsGateway::createFolder(int parentFolderId, const Path& newFolderName, Permissions permissions)
{
    SqliteStmtReseter folderReseter(m_insertFolderQuery->get());
    
    int error = sqlite3_step(m_insertFolderQuery->get());
    if (error != SQLITE_DONE)
    {
        THROW("can't insert");
    }
    
    int newFolderId = static_cast<int>(sqlite3_last_insert_rowid(m_sqlite));
    
    SqliteStmtReseter itemReseter(m_insertItemQuery->get());
    
    sqlite3_bind_int(m_insertItemQuery->get(), 1, static_cast<int>(SQLiteEntities::ItemType::Folder));
    sqlite3_bind_int(m_insertItemQuery->get(), 2, newFolderId);
    sqlite3_bind_int(m_insertItemQuery->get(), 3, static_cast<int>(permissions));
    
    error = sqlite3_step(m_insertItemQuery->get());
    if (error != SQLITE_DONE)
    {
        THROW("can't insert");
    }
    
    int newItemId = static_cast<int>(sqlite3_last_insert_rowid(m_sqlite));
    
    SqliteStmtReseter linkReseter(m_insertLinkQuery->get());
    
    sqlite3_bind_int(m_insertLinkQuery->get(), 1, parentFolderId);
    sqlite3_bind_int(m_insertLinkQuery->get(), 2, newItemId);
    sqlite3_bind_text(m_insertLinkQuery->get(), 3, newFolderName.c_str(), -1, SQLITE_STATIC);
    
    error = sqlite3_step(m_insertLinkQuery->get());
    if (error != SQLITE_DONE)
    {
        throw SQLiteFsException(FsError::kFileExists, "Such file exists");
    }
}
    
void SQLiteFsGateway::readFolderWithId(int folderId, std::vector<FileInfo>* fileInfos)
{
    std::vector<FileInfo> actualFileInfos;
    
    SqliteStmtReseter reseter(m_selectLinksWithParentId->get());
    
    sqlite3_bind_int(m_selectLinksWithParentId->get(), 1, folderId);
    int error = sqlite3_step(m_selectLinksWithParentId->get());
    while (error == SQLITE_ROW)
    {
        FileInfo info;
        info.name = reinterpret_cast<const char*>(sqlite3_column_text(m_selectLinksWithParentId->get(), 0));
        info.type = static_cast<FileType>(sqlite3_column_int(m_selectLinksWithParentId->get(), 1));
        info.permissions = static_cast<Permissions>(sqlite3_column_int(m_selectLinksWithParentId->get(), 2));
        
        actualFileInfos.push_back(info);
        
        error = sqlite3_step(m_selectLinksWithParentId->get());
    }

    if (error != SQLITE_DONE)
    {
        throw SQLiteFsException(FsError::kReadFolderError, "Such file exists");
    }
    
    fileInfos->swap(actualFileInfos);
}

void SQLiteFsGateway::addExtendedAttribute(int itemId, const char* attributeKey, const char* attributeValue, const int attributeValueSize)
{
    SqliteStmtReseter reseter(m_insertExtendedAttributeQuery->get());
    
    sqlite3_bind_int(m_insertExtendedAttributeQuery->get(), 1, itemId);
    sqlite3_bind_text(m_insertExtendedAttributeQuery->get(), 2, attributeKey, -1, SQLITE_STATIC);
    sqlite3_bind_blob(m_insertExtendedAttributeQuery->get(), 3, attributeValue, attributeValueSize, SQLITE_STATIC);
    
    int error = sqlite3_step(m_insertExtendedAttributeQuery->get());
    if (error != SQLITE_DONE)
    {
        THROW("can't insert");
    }
}
    
void SQLiteFsGateway::deleteExtendedAttribute(int itemId, const char* attributeKey)
{
    SqliteStmtReseter reseter(m_deleteExtendedAttributeByItemIdAndNameQuery->get());
    
    sqlite3_bind_int(m_deleteExtendedAttributeByItemIdAndNameQuery->get(), 1, itemId);
    sqlite3_bind_text(m_deleteExtendedAttributeByItemIdAndNameQuery->get(), 2, attributeKey, -1, SQLITE_STATIC);
    
    int error = sqlite3_step(m_selectExtendedAttributeByItemIdAndNameQuery->get());
    if (error != SQLITE_DONE)
    {
        throw SQLiteFsException(FsError::kAttributeNotFound, "Attribute not found");
    }
}
    
void SQLiteFsGateway::getExtendedAttribute(int itemId, const char* attributeKey, std::vector<char>* attributeValue)
{
    SqliteStmtReseter reseter(m_selectExtendedAttributeByItemIdAndNameQuery->get());
    
    sqlite3_bind_int(m_selectExtendedAttributeByItemIdAndNameQuery->get(), 1, itemId);
    sqlite3_bind_text(m_selectExtendedAttributeByItemIdAndNameQuery->get(), 2, attributeKey, -1, SQLITE_STATIC);
    
    int error =  sqlite3_step(m_selectExtendedAttributeByItemIdAndNameQuery->get());
    if (error == SQLITE_ROW)
    {
        const char* attributeValuePtr = reinterpret_cast<const char*>(sqlite3_column_blob(m_selectExtendedAttributeByItemIdAndNameQuery->get(), 3));
        int attributeValueSize = sqlite3_column_bytes(m_selectExtendedAttributeByItemIdAndNameQuery->get(), 3);
        attributeValue->assign(attributeValuePtr, attributeValuePtr + attributeValueSize);
    }
    else
    {
        throw SQLiteFsException(FsError::kAttributeNotFound, "Attribute not found");
    }
}
    
void SQLiteFsGateway::getExtendedAttributesNames(int itemId, std::vector<std::string>* attributesNames)
{
    SqliteStmtReseter reseter(m_selectExtendedAttributesByItemIdQuery->get());
    
    sqlite3_bind_int(m_selectExtendedAttributesByItemIdQuery->get(), 1, itemId);
    
    int error = sqlite3_step(m_selectExtendedAttributesByItemIdQuery->get());
    std::vector<std::string> attributesNamesRes;
    while (error == SQLITE_ROW)
    {
        std::string attributeName = reinterpret_cast<const char*>(sqlite3_column_text(m_selectExtendedAttributesByItemIdQuery->get(), 2));
        attributesNamesRes.push_back(attributeName);
        
        error = sqlite3_step(m_selectExtendedAttributesByItemIdQuery->get());
    }
    
    if (error != SQLITE_DONE)
    {
        THROW("Sqlite error occure");
    }
    
    attributesNames->swap(attributesNamesRes);
}
    
}
