//
//  SqliteFsGateway.cpp
//  DistributedFs
//
//  Created by Роман on 8/8/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include "SqliteFsGateway.h"

#include "SqliteFsException.h"
#include "Utils.h"

namespace
{
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

SqliteFsGateway::SqliteFsGateway(const Path& dbPath)
    : m_sqlite(dbPath.string())
{
    m_sqlite.executeQuery("CREATE TABLE Links (id INTEGER PRIMARY KEY ASC, parentId INT NOT NULL, itemId INT NOT NULL, name TEXT NOT NULL, UNIQUE (parentId, name));");
    m_sqlite.executeQuery("CREATE TABLE Items (id INTEGER PRIMARY KEY ASC, type INT NOT NULL, concreteItemId INT NOT NULL, permissions INT NOT NULL, creationTime INT NOT NULL, modificationTime INT NOT NULL);");
    m_sqlite.executeQuery("CREATE TABLE Folders (id INTEGER PRIMARY KEY ASC, dummy INT);");
    m_sqlite.executeQuery("CREATE TABLE Files (id INTEGER PRIMARY KEY ASC, data BLOB);");
    m_sqlite.executeQuery("CREATE TABLE SymLinks (id INTEGER PRIMARY KEY ASC, path TEXT NOT NULL);");
    m_sqlite.executeQuery("CREATE TABLE ExtendedAttributes (id INTEGER PRIMARY KEY ASC, itemId INT NOT NULL, name TEXT NOT NULL, value BLOB NOT NULL, UNIQUE (itemId, name));");
    
    m_selectLinkQueryWithParentIdAndName = m_sqlite.createStatement("SELECT id, parentId, itemId, name FROM Links WHERE parentId = ? AND name = ?;");
    m_selectItemQueryWithId              = m_sqlite.createStatement("SELECT id, type, concreteItemId, permissions, creationTime, modificationTime FROM Items WHERE id = ?;");
    m_selectFolderQueryWithId            = m_sqlite.createStatement("SELECT id, dummy  FROM Folders WHERE id = ?;");
    m_selectLinksWithParentId            = m_sqlite.createStatement("SELECT Links.name, Items.type, Items.permissions FROM Links JOIN Items ON Links.itemId = Items.id WHERE parentId = ?;");
    
    m_updateItemWithIdQuery = m_sqlite.createStatement("UPDATE Items SET permissions = ?2, creationTime = ?3, modificationTime = ?4 WHERE id = ?1;");
    
    m_insertFolderQuery   = m_sqlite.createStatement("INSERT INTO Folders (dummy) VALUES (0);");
    m_insertFileQuery     = m_sqlite.createStatement("INSERT INTO Files (data) VALUES (NULL);");
    m_insertSymLinkQuery  = m_sqlite.createStatement("INSERT INTO SymLinks (path) VALUES (?);");
    m_insertItemQuery     = m_sqlite.createStatement("INSERT INTO Items (type, concreteItemId, permissions, creationTime, modificationTime) VALUES (?, ?, ?, ?, ?);");
    m_insertLinkQuery     = m_sqlite.createStatement("INSERT INTO Links (parentId, itemId, name) VALUES (?, ?, ?);");
    
    m_deleteLinkWithId = m_sqlite.createStatement("DELETE FROM Links WHERE id = ?;");
    
    m_selectFileDataWithIdQuery = m_sqlite.createStatement("SELECT data FROM Files WHERE id = ?;");
    m_updateFileDataWithIdQuery = m_sqlite.createStatement("UPDATE Files SET data = ?2 WHERE id = ?1;");
    
    m_selectSymLinkWithIdQuery = m_sqlite.createStatement("SELECT path FROM SymLinks WHERE id = ?;");
    
    m_insertExtendedAttributeQuery                = m_sqlite.createStatement("INSERT INTO ExtendedAttributes (itemId, name, value) VALUES (?, ?, ?);");
    m_deleteExtendedAttributeByItemIdAndNameQuery = m_sqlite.createStatement("DELETE FROM ExtendedAttributes WHERE itemId = ? AND name = ?;");
    m_selectExtendedAttributesByItemIdQuery       = m_sqlite.createStatement("SELECT id, itemId, name, value FROM ExtendedAttributes WHERE itemId = ?;");
    m_selectExtendedAttributeByItemIdAndNameQuery = m_sqlite.createStatement("SELECT id, itemId, name, value FROM ExtendedAttributes WHERE itemId = ? AND name = ?;");
    
    m_sqlite.executeQuery("INSERT INTO Folders (dummy) VALUES (0);"); //super root
    m_superRootFolderId = m_sqlite.getLastInsertedRowId();

    createFolder(m_superRootFolderId, "/", Permissions::kAll);
}

SqliteEntities::Folder SqliteFsGateway::getFolderByPath(const Path& folderPath)
{
    SqliteEntities::Item item = getItemByPath(folderPath);
    if (item.type != dfs::FileType::kFolder)
    {
        throw SqliteFsException(FsError::kFileNotFound, "File is not folder");
    }
    
    return getFolderById(item.concreteItemId);
}
    
SqliteEntities::Item SqliteFsGateway::getItemByPath(const Path& itemPath, bool followLastSymlink)
{
    SqliteEntities::Link link;
    int parentId = m_superRootFolderId;
    
    auto pathIt = itemPath.begin();
    auto pathEndIt = itemPath.end();
    pathEndIt--;
    while (pathIt != pathEndIt)
    {
        link = getLink(parentId, pathIt->leaf());
        
        SqliteEntities::Item item = getItemById(link.itemId);
        while (item.type == dfs::FileType::kSymLink)
        {
            SqliteEntities::SymLink symLink = getSymLinkById(item.concreteItemId);
            item = getItemByPath(symLink.path);
        }
        
        if (item.type != dfs::FileType::kFolder)
        {
            throw SqliteFsException(FsError::kFileHasWrongType, "File has wrong type");
        }
        
        parentId = item.concreteItemId;
        pathIt++;
    }

    link = getLink(parentId, pathIt->leaf());
    SqliteEntities::Item item = getItemById(link.itemId);
    
    if (followLastSymlink)
    {
        while (item.type == dfs::FileType::kSymLink)
        {
            SqliteEntities::SymLink symLink = getSymLinkById(item.concreteItemId);
            item = getItemByPath(symLink.path);
        }
    }
    
    return item;
}
    
SqliteEntities::Link SqliteFsGateway::getLink(int parentId, const Path& name)
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
        SqliteEntities::Link link;
        link.id = sqlite3_column_int(m_selectLinkQueryWithParentIdAndName->get(), 0);
        link.parentFolderId = sqlite3_column_int(m_selectLinkQueryWithParentIdAndName->get(), 1);
        link.itemId = sqlite3_column_int(m_selectLinkQueryWithParentIdAndName->get(), 2);
        link.name = reinterpret_cast<const char*>(sqlite3_column_text(m_selectLinkQueryWithParentIdAndName->get(), 3));
        
        return link;
    }
    
    throw SqliteFsException(FsError::kFileNotFound, "Can't find file with such name");
}
    
SqliteEntities::Folder SqliteFsGateway::getFolderById(int folderId)
{
    SqliteStmtReseter reseter(m_selectFolderQueryWithId->get());
    
    sqlite3_bind_int(m_selectFolderQueryWithId->get(), 1, folderId);
    
    int error = sqlite3_step(m_selectFolderQueryWithId->get());
    if (error == SQLITE_ROW)
    {
        SqliteEntities::Folder folder;
        folder.id = sqlite3_column_int(m_selectFolderQueryWithId->get(), 0);
        
        return folder;
    }
    
    throw SqliteFsException(FsError::kFileNotFound, "Folder was not found");
}
    
SqliteEntities::Item SqliteFsGateway::getItemById(int itemId)
{
    SqliteStmtReseter reseter(m_selectItemQueryWithId->get());
    
    sqlite3_bind_int(m_selectItemQueryWithId->get(), 1, itemId);
    
    int error = sqlite3_step(m_selectItemQueryWithId->get());
    if (error == SQLITE_ROW)
    {
        SqliteEntities::Item item;
        item.id = sqlite3_column_int(m_selectItemQueryWithId->get(), 0);
        item.type = static_cast<dfs::FileType>(sqlite3_column_int(m_selectItemQueryWithId->get(), 1));
        item.concreteItemId = sqlite3_column_int(m_selectItemQueryWithId->get(), 2);
        item.permissions = static_cast<Permissions>(sqlite3_column_int(m_selectItemQueryWithId->get(), 3));
        item.creationTime = static_cast<std::time_t>(sqlite3_column_int(m_selectItemQueryWithId->get(), 4));
        item.modificationTime = static_cast<std::time_t>(sqlite3_column_int(m_selectItemQueryWithId->get(), 5));
        
        return item;
    }
    
    throw SqliteFsException(FsError::kFileNotFound, "item was not found");
}

void SqliteFsGateway::updateItem(const SqliteEntities::Item& item)
{
    SqliteStmtReseter reseter(m_updateItemWithIdQuery->get());
    
    sqlite3_bind_int(m_updateItemWithIdQuery->get(), 1, item.id);
    sqlite3_bind_int(m_updateItemWithIdQuery->get(), 2, static_cast<int>(item.permissions));
    sqlite3_bind_int(m_updateItemWithIdQuery->get(), 3, static_cast<int>(item.creationTime));
    sqlite3_bind_int(m_updateItemWithIdQuery->get(), 4, static_cast<int>(item.modificationTime));
    
    int error = sqlite3_step(m_updateItemWithIdQuery->get());
    if (error != SQLITE_DONE)
    {
        throw SqliteFsException(FsError::kUnknownError, "Can't update item");
    }
}
    
SqliteEntities::SymLink SqliteFsGateway::getSymLinkById(int symLinkId)
{
    SqliteStmtReseter reseter(m_selectSymLinkWithIdQuery->get());
    
    sqlite3_bind_int(m_selectSymLinkWithIdQuery->get(), 1, symLinkId);

    int error = sqlite3_step(m_selectSymLinkWithIdQuery->get());
    if (error != SQLITE_ROW)
    {
        throw SqliteFsException(FsError::kUnknownError, "Can't get symlink");
    }
        
    SqliteEntities::SymLink symLink;
    symLink.id = symLinkId;
    symLink.path = reinterpret_cast<const char*>(sqlite3_column_text(m_selectSymLinkWithIdQuery->get(), 0));
    
    return symLink;
}
    
void SqliteFsGateway::createFolder(int parentFolderId, const Path& newFolderName, Permissions permissions)
{
    SqliteStmtReseter folderReseter(m_insertFolderQuery->get());
    
    int error = sqlite3_step(m_insertFolderQuery->get());
    if (error != SQLITE_DONE)
    {
        THROW("can't insert");
    }
    
    int newFolderId = m_sqlite.getLastInsertedRowId();
    createItemImpl(FileType::kFolder, newFolderId, permissions);
    
    int newItemId = m_sqlite.getLastInsertedRowId();
    createHardLinkImpl(parentFolderId, newItemId, newFolderName);
}
    
void SqliteFsGateway::createFile(int parentFolderId, const Path& newFolderName, Permissions permissions)
{
    SqliteStmtReseter fileReseter(m_insertFileQuery->get());
    
    int error = sqlite3_step(m_insertFileQuery->get());
    if (error != SQLITE_DONE)
    {
        THROW("can't insert");
    }
    
    int newFileId = m_sqlite.getLastInsertedRowId();
    createItemImpl(FileType::kFile, newFileId, permissions);
    
    int newItemId = m_sqlite.getLastInsertedRowId();
    createHardLinkImpl(parentFolderId, newItemId, newFolderName);
}
    
void SqliteFsGateway::createSymLink(int parentFolderId, const Path& newLinkName, Permissions permissions, const Path& symLinkValue)
{
    SqliteStmtReseter reseter(m_insertSymLinkQuery->get());
    
    sqlite3_bind_text(m_insertSymLinkQuery->get(), 1, symLinkValue.c_str(), -1, SQLITE_STATIC);
    
    int error = sqlite3_step(m_insertSymLinkQuery->get());
    if (error != SQLITE_DONE)
    {
        THROW("can't insert");
    }
    
    int newSymLinkName = m_sqlite.getLastInsertedRowId();
    createItemImpl(FileType::kSymLink, newSymLinkName, permissions);
    
    int newItemId = m_sqlite.getLastInsertedRowId();
    createHardLinkImpl(parentFolderId, newItemId, newLinkName);
}
    
void SqliteFsGateway::createHardLink(int parentId, int itemId, const Path& linkName)
{
    createHardLinkImpl(parentId, itemId, linkName);
}
    
void SqliteFsGateway::removeLink(int linkId)
{
    SqliteStmtReseter linkReseter(m_deleteLinkWithId->get());
    
    sqlite3_bind_int(m_deleteLinkWithId->get(), 1, linkId);
    
    int error = sqlite3_step(m_deleteLinkWithId->get());
    if (error != SQLITE_DONE)
    {
        throw SqliteFsException(FsError::kFileNotFound, "No such link");
    }
}
    
void SqliteFsGateway::readFolderWithId(int folderId, std::vector<FileInfo>* fileInfos)
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
        throw SqliteFsException(FsError::kReadFolderError, "Such file exists");
    }
    
    fileInfos->swap(actualFileInfos);
}
    
void SqliteFsGateway::getFileData(int fileId, std::vector<char>* fileData)
{
    SqliteStmtReseter reseter(m_selectFileDataWithIdQuery->get());
    
    sqlite3_bind_int(m_selectFileDataWithIdQuery->get(), 1, fileId);
    
    int error = sqlite3_step(m_selectFileDataWithIdQuery->get());
    if (error != SQLITE_ROW)
    {
        throw SqliteFsException(FsError::kUnknownError, "Can't get file data");
    }
    
    const char* fileDataPtr = reinterpret_cast<const char*>(sqlite3_column_blob(m_selectFileDataWithIdQuery->get(), 0));
    if (!fileDataPtr)
    {
        fileData->clear();
        return;
    }
    
    const int fileDataSize = sqlite3_column_bytes(m_selectFileDataWithIdQuery->get(), 0);
    fileData->assign(fileDataPtr, fileDataPtr + fileDataSize);
}

void SqliteFsGateway::updateFileData(int fileId, const std::vector<char>& fileData)
{
    SqliteStmtReseter reseter(m_updateFileDataWithIdQuery->get());
    
    sqlite3_bind_int(m_updateFileDataWithIdQuery->get(), 1, fileId);
    sqlite3_bind_blob(m_updateFileDataWithIdQuery->get(), 2, fileData.data(), static_cast<int>(fileData.size()), SQLITE_STATIC);
    
    int error = sqlite3_step(m_updateFileDataWithIdQuery->get());
    if (error != SQLITE_DONE)
    {
       throw SqliteFsException(FsError::kUnknownError, "Can't update file data");
    }
}
    
void SqliteFsGateway::addExtendedAttribute(int itemId, const char* attributeKey, const char* attributeValue, const int attributeValueSize)
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
    
void SqliteFsGateway::deleteExtendedAttribute(int itemId, const char* attributeKey)
{
    SqliteStmtReseter reseter(m_deleteExtendedAttributeByItemIdAndNameQuery->get());
    
    sqlite3_bind_int(m_deleteExtendedAttributeByItemIdAndNameQuery->get(), 1, itemId);
    sqlite3_bind_text(m_deleteExtendedAttributeByItemIdAndNameQuery->get(), 2, attributeKey, -1, SQLITE_STATIC);
    
    int error = sqlite3_step(m_selectExtendedAttributeByItemIdAndNameQuery->get());
    if (error != SQLITE_DONE)
    {
        throw SqliteFsException(FsError::kAttributeNotFound, "Attribute not found");
    }
}
    
void SqliteFsGateway::getExtendedAttribute(int itemId, const char* attributeKey, std::vector<char>* attributeValue)
{
    SqliteStmtReseter reseter(m_selectExtendedAttributeByItemIdAndNameQuery->get());
    
    sqlite3_bind_int(m_selectExtendedAttributeByItemIdAndNameQuery->get(), 1, itemId);
    sqlite3_bind_text(m_selectExtendedAttributeByItemIdAndNameQuery->get(), 2, attributeKey, -1, SQLITE_STATIC);
    
    int error = sqlite3_step(m_selectExtendedAttributeByItemIdAndNameQuery->get());
    if (error == SQLITE_ROW)
    {
        const char* attributeValuePtr = reinterpret_cast<const char*>(sqlite3_column_blob(m_selectExtendedAttributeByItemIdAndNameQuery->get(), 3));
        int attributeValueSize = sqlite3_column_bytes(m_selectExtendedAttributeByItemIdAndNameQuery->get(), 3);
        attributeValue->assign(attributeValuePtr, attributeValuePtr + attributeValueSize);
    }
    else
    {
        throw SqliteFsException(FsError::kAttributeNotFound, "Attribute not found");
    }
}
    
void SqliteFsGateway::getExtendedAttributesNames(int itemId, std::vector<std::string>* attributesNames)
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

void SqliteFsGateway::createItemImpl(FileType fileType, int concreteItemId, Permissions permissions)
{
    SqliteStmtReseter itemReseter(m_insertItemQuery->get());
    
    std::time_t currentTime = std::time(nullptr);
    sqlite3_bind_int(m_insertItemQuery->get(), 1, static_cast<int>(fileType));
    sqlite3_bind_int(m_insertItemQuery->get(), 2, concreteItemId);
    sqlite3_bind_int(m_insertItemQuery->get(), 3, static_cast<int>(permissions));
    sqlite3_bind_int(m_insertItemQuery->get(), 4, static_cast<int>(currentTime));
    sqlite3_bind_int(m_insertItemQuery->get(), 5, static_cast<int>(currentTime));
    
    int error = sqlite3_step(m_insertItemQuery->get());
    if (error != SQLITE_DONE)
    {
        THROW("can't insert");
    }
}
    
void SqliteFsGateway::createHardLinkImpl(int parentId, int itemId, const Path& linkName)
{
    SqliteStmtReseter link(m_insertLinkQuery->get());
    
    sqlite3_bind_int(m_insertLinkQuery->get(), 1, parentId);
    sqlite3_bind_int(m_insertLinkQuery->get(), 2, itemId);
    sqlite3_bind_text(m_insertLinkQuery->get(), 3, linkName.c_str(), -1, SQLITE_STATIC);
    
    int error = sqlite3_step(m_insertLinkQuery->get());
    if (error != SQLITE_DONE)
    {
        throw SqliteFsException(FsError::kFileExists, "Such file exists");
    }
}
    
}
