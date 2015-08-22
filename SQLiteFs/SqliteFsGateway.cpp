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
        SqliteStmtReseter(dfs::SqliteStatement& stmt)
        : m_stmt(stmt)
        {}
        
        ~SqliteStmtReseter()
        {
            m_stmt.reset();
        }
        
    private:
        dfs::SqliteStatement& m_stmt;
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
    m_sqlite.executeQuery("CREATE TABLE ExtendedAttributes ("
                                        "id INTEGER PRIMARY KEY ASC,"
                                        "itemId INTEGER NOT NULL,"
                                        "name TEXT NOT NULL,"
                                        "value BLOB NOT NULL,"
                                        "UNIQUE (itemId, name)"
                                        ");");
    
    m_sqlite.executeQuery("CREATE TRIGGER FolderRemover BEFORE DELETE ON Items WHEN (OLD.type = 1) "
                          "BEGIN "
                              "DELETE FROM Folders WHERE Folders.id = OLD.concreteItemId; "
                          "END;");
    
    m_sqlite.executeQuery("CREATE TRIGGER FileRemover BEFORE DELETE ON Items WHEN (OLD.type = 0) "
                          "BEGIN "
                              "DELETE FROM Files WHERE Files.id = OLD.concreteItemId; "
                          "END;");
    
    m_sqlite.executeQuery("CREATE TRIGGER SymLinkRemover BEFORE DELETE ON Items WHEN (OLD.type = 2) "
                          "BEGIN "
                              "DELETE FROM SymLinks WHERE SymLinks.id = OLD.concreteItemId; "
                          "END;");

    m_sqlite.executeQuery("CREATE TRIGGER ExtendedAttributesRemover BEFORE DELETE ON Items "
                          "BEGIN "
                            "DELETE FROM ExtendedAttributes WHERE ExtendedAttributes.itemId = OLD.id; "
                          "END;");
    
    m_selectLinkQueryWithParentIdAndName = m_sqlite.createStatement("SELECT id, parentId, itemId, name FROM Links WHERE parentId = ? AND name = ?;");
    m_selectItemQueryWithId              = m_sqlite.createStatement("SELECT id, type, concreteItemId, permissions, creationTime, modificationTime FROM Items WHERE id = ?;");
    m_selectFolderQueryWithId            = m_sqlite.createStatement("SELECT id, dummy  FROM Folders WHERE id = ?;");
    m_selectLinksWithParentId            = m_sqlite.createStatement("SELECT Links.name, Items.type, Items.permissions FROM Links JOIN Items ON Links.itemId = Items.id WHERE parentId = ?;");
    
    m_getItemLinksCountWithItemId = m_sqlite.createStatement("SELECT COUNT(*) FROM Links WHERE itemId = ?;");
    
    m_updateItemWithIdQuery = m_sqlite.createStatement("UPDATE Items SET permissions = ?2, creationTime = ?3, modificationTime = ?4 WHERE id = ?1;");
    
    m_insertFolderQuery   = m_sqlite.createStatement("INSERT INTO Folders (dummy) VALUES (0);");
    m_insertFileQuery     = m_sqlite.createStatement("INSERT INTO Files (data) VALUES (NULL);");
    m_insertSymLinkQuery  = m_sqlite.createStatement("INSERT INTO SymLinks (path) VALUES (?);");
    m_insertItemQuery     = m_sqlite.createStatement("INSERT INTO Items (type, concreteItemId, permissions, creationTime, modificationTime) VALUES (?, ?, ?, ?, ?);");
    m_insertLinkQuery     = m_sqlite.createStatement("INSERT INTO Links (parentId, itemId, name) VALUES (?, ?, ?);");
    
    m_deleteLinkWithId = m_sqlite.createStatement("DELETE FROM Links WHERE id = ?;");
    m_deleteItemWithId = m_sqlite.createStatement("DELETE FROM Items WHERE id = ?;");
    
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
    SqliteStmtReseter reseter(*m_selectLinkQueryWithParentIdAndName);

    m_selectLinkQueryWithParentIdAndName->bindInt(1, parentId);
    m_selectLinkQueryWithParentIdAndName->bindText(2, name.c_str());
    
    int error = m_selectLinkQueryWithParentIdAndName->step();
    if (error == SQLITE_ROW)
    {
        SqliteEntities::Link link;
        link.id = m_selectLinkQueryWithParentIdAndName->getIntColumn(0);
        link.parentFolderId = m_selectLinkQueryWithParentIdAndName->getIntColumn(1);
        link.itemId = m_selectLinkQueryWithParentIdAndName->getIntColumn(2);
        link.name = m_selectLinkQueryWithParentIdAndName->getTextColumn(3).get();
        
        return link;
    }
    
    throw SqliteFsException(FsError::kFileNotFound, "Can't find file with such name");
}
    
SqliteEntities::Folder SqliteFsGateway::getFolderById(int folderId)
{
    SqliteStmtReseter reseter(*m_selectFolderQueryWithId);
    
    m_selectFolderQueryWithId->bindInt(1, folderId);
    
    int error = m_selectFolderQueryWithId->step();
    if (error == SQLITE_ROW)
    {
        SqliteEntities::Folder folder;
        folder.id = m_selectFolderQueryWithId->getIntColumn(0);
        
        return folder;
    }
    
    throw SqliteFsException(FsError::kFileNotFound, "Folder was not found");
}
    
SqliteEntities::Item SqliteFsGateway::getItemById(int itemId)
{
    SqliteStmtReseter reseter(*m_selectItemQueryWithId);
    
    m_selectItemQueryWithId->bindInt(1, itemId);
    
    int error = m_selectItemQueryWithId->step();
    if (error == SQLITE_ROW)
    {
        SqliteEntities::Item item;
        item.id = m_selectItemQueryWithId->getIntColumn(0);
        item.type = static_cast<dfs::FileType>(m_selectItemQueryWithId->getIntColumn(1));
        item.concreteItemId = m_selectItemQueryWithId->getIntColumn(2);
        item.permissions = static_cast<Permissions>(m_selectItemQueryWithId->getIntColumn(3));
        item.creationTime = static_cast<std::time_t>(m_selectItemQueryWithId->getIntColumn(4));
        item.modificationTime = static_cast<std::time_t>(m_selectItemQueryWithId->getIntColumn(5));
        
        return item;
    }
    
    throw SqliteFsException(FsError::kFileNotFound, "item was not found");
}
    
int  SqliteFsGateway::getItemLinksCount(int itemId)
{
    SqliteStmtReseter reseter(*m_getItemLinksCountWithItemId);
    
    m_getItemLinksCountWithItemId->bindInt(1, itemId);
    
    int error = m_getItemLinksCountWithItemId->step();
    if (error != SQLITE_ROW)
    {
        throw SqliteFsException(FsError::kUnknownError, "Can't update item");
    }
    
    return m_getItemLinksCountWithItemId->getIntColumn(0);
}

void SqliteFsGateway::updateItem(const SqliteEntities::Item& item)
{
    SqliteStmtReseter reseter(*m_updateItemWithIdQuery);
    
    m_updateItemWithIdQuery->bindInt(1, item.id);
    m_updateItemWithIdQuery->bindInt(2, static_cast<int>(item.permissions));
    m_updateItemWithIdQuery->bindInt(3, static_cast<int>(item.creationTime));
    m_updateItemWithIdQuery->bindInt(4, static_cast<int>(item.modificationTime));
    
    int error = m_updateItemWithIdQuery->step();
    if (error != SQLITE_DONE)
    {
        throw SqliteFsException(FsError::kUnknownError, "Can't update item");
    }
}
    
SqliteEntities::SymLink SqliteFsGateway::getSymLinkById(int symLinkId)
{
    SqliteStmtReseter reseter(*m_selectSymLinkWithIdQuery);
    
    m_selectSymLinkWithIdQuery->bindInt(1, symLinkId);

    int error = m_selectSymLinkWithIdQuery->step();
    if (error != SQLITE_ROW)
    {
        throw SqliteFsException(FsError::kUnknownError, "Can't get symlink");
    }
        
    SqliteEntities::SymLink symLink;
    symLink.id = symLinkId;
    symLink.path = m_selectSymLinkWithIdQuery->getTextColumn(0).get();
    
    return symLink;
}
    
void SqliteFsGateway::createFolder(int parentFolderId, const Path& newFolderName, Permissions permissions)
{
    SqliteStmtReseter folderReseter(*m_insertFolderQuery);
    
    int error = m_insertFolderQuery->step();
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
    SqliteStmtReseter fileReseter(*m_insertFileQuery);
    
    int error = m_insertFileQuery->step();
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
    SqliteStmtReseter reseter(*m_insertSymLinkQuery);
    
    m_insertSymLinkQuery->bindText(1, symLinkValue.c_str());
    
    int error = m_insertSymLinkQuery->step();
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
    SqliteStmtReseter linkReseter(*m_deleteLinkWithId);
    
    m_deleteLinkWithId->bindInt(1, linkId);
    
    int error = m_deleteLinkWithId->step();
    if (error != SQLITE_DONE)
    {
        throw SqliteFsException(FsError::kFileNotFound, "No such link");
    }
}
    
void SqliteFsGateway::removeItem(int itemId)
{
    SqliteStmtReseter reseter(*m_deleteItemWithId);
    
    m_deleteItemWithId->bindInt(1, itemId);
    
    int error = m_deleteItemWithId->step();
    if (error != SQLITE_DONE)
    {
        throw SqliteFsException(FsError::kFileNotFound, "No such item");
    }
}
    
void SqliteFsGateway::readFolderWithId(int folderId, std::vector<FileInfo>* fileInfos)
{
    std::vector<FileInfo> actualFileInfos;
    
    SqliteStmtReseter reseter(*m_selectLinksWithParentId);
    
    m_selectLinksWithParentId->bindInt(1, folderId);
    int error = m_selectLinksWithParentId->step();
    while (error == SQLITE_ROW)
    {
        FileInfo info;
        info.name = m_selectLinksWithParentId->getTextColumn(0).get();
        info.type = static_cast<FileType>(m_selectLinksWithParentId->getIntColumn(1));
        info.permissions = static_cast<Permissions>(m_selectLinksWithParentId->getIntColumn(2));
        
        actualFileInfos.push_back(info);
        
        error = m_selectLinksWithParentId->step();
    }

    if (error != SQLITE_DONE)
    {
        throw SqliteFsException(FsError::kReadFolderError, "Such file exists");
    }
    
    fileInfos->swap(actualFileInfos);
}
    
void SqliteFsGateway::getFileData(int fileId, std::vector<char>* fileData)
{
    SqliteStmtReseter reseter(*m_selectFileDataWithIdQuery);
    
    m_selectFileDataWithIdQuery->bindInt(1, fileId);
    
    int error = m_selectFileDataWithIdQuery->step();
    if (error != SQLITE_ROW)
    {
        throw SqliteFsException(FsError::kUnknownError, "Can't get file data");
    }
    
    auto blobRes = m_selectFileDataWithIdQuery->getBlobColumn(0);
    if (blobRes)
    {
        fileData->swap(blobRes.get());
        return;
    }

    fileData->clear();
}

void SqliteFsGateway::updateFileData(int fileId, const std::vector<char>& fileData)
{
    SqliteStmtReseter reseter(*m_updateFileDataWithIdQuery);
    
    m_updateFileDataWithIdQuery->bindInt(1, fileId);
    m_updateFileDataWithIdQuery->bindBlob(2, fileData.data(), fileData.size());
    
    int error = m_updateFileDataWithIdQuery->step();
    if (error != SQLITE_DONE)
    {
       throw SqliteFsException(FsError::kUnknownError, "Can't update file data");
    }
}
    
void SqliteFsGateway::addExtendedAttribute(int itemId, const char* attributeKey, const char* attributeValue, const int attributeValueSize)
{
    SqliteStmtReseter reseter(*m_insertExtendedAttributeQuery);
    
    m_insertExtendedAttributeQuery->bindInt(1, itemId);
    m_insertExtendedAttributeQuery->bindText(2, attributeKey);
    m_insertExtendedAttributeQuery->bindBlob(3, attributeValue, attributeValueSize);
    
    int error = m_insertExtendedAttributeQuery->step();
    if (error != SQLITE_DONE)
    {
        THROW("can't insert");
    }
}
    
void SqliteFsGateway::deleteExtendedAttribute(int itemId, const char* attributeKey)
{
    SqliteStmtReseter reseter(*m_deleteExtendedAttributeByItemIdAndNameQuery);
    
    m_deleteExtendedAttributeByItemIdAndNameQuery->bindInt(1, itemId);
    m_deleteExtendedAttributeByItemIdAndNameQuery->bindText(2, attributeKey);
    
    int error = m_deleteExtendedAttributeByItemIdAndNameQuery->step();
    if (error != SQLITE_DONE)
    {
        throw SqliteFsException(FsError::kAttributeNotFound, "Attribute not found");
    }
}
    
void SqliteFsGateway::getExtendedAttribute(int itemId, const char* attributeKey, std::vector<char>* attributeValue)
{
    SqliteStmtReseter reseter(*m_selectExtendedAttributeByItemIdAndNameQuery);
    
    m_selectExtendedAttributeByItemIdAndNameQuery->bindInt(1, itemId);
    m_selectExtendedAttributeByItemIdAndNameQuery->bindText(2, attributeKey);
    
    int error = m_selectExtendedAttributeByItemIdAndNameQuery->step();
    if (error == SQLITE_ROW)
    {
        auto actualAttrValue = m_selectExtendedAttributeByItemIdAndNameQuery->getBlobColumn(3);
        attributeValue->swap(actualAttrValue.get());
    }
    else
    {
        throw SqliteFsException(FsError::kAttributeNotFound, "Attribute not found");
    }
}
    
void SqliteFsGateway::getExtendedAttributesNames(int itemId, std::vector<std::string>* attributesNames)
{
    SqliteStmtReseter reseter(*m_selectExtendedAttributesByItemIdQuery);
    
    m_selectExtendedAttributesByItemIdQuery->bindInt(1, itemId);
    
    std::vector<std::string> attributesNamesRes;
    int error = m_selectExtendedAttributesByItemIdQuery->step();
    while (error == SQLITE_ROW)
    {
        attributesNamesRes.push_back(m_selectExtendedAttributesByItemIdQuery->getTextColumn(2).get());
        error = m_selectExtendedAttributesByItemIdQuery->step();
    }
    
    if (error != SQLITE_DONE)
    {
        THROW("Sqlite error occure");
    }
    
    attributesNames->swap(attributesNamesRes);
}

void SqliteFsGateway::createItemImpl(FileType fileType, int concreteItemId, Permissions permissions)
{
    SqliteStmtReseter itemReseter(*m_insertItemQuery);
    
    std::time_t currentTime = std::time(nullptr);
    m_insertItemQuery->bindInt(1, static_cast<int>(fileType));
    m_insertItemQuery->bindInt(2, concreteItemId);
    m_insertItemQuery->bindInt(3, static_cast<int>(permissions));
    m_insertItemQuery->bindInt(4, static_cast<int>(currentTime));
    m_insertItemQuery->bindInt(5, static_cast<int>(currentTime));
    
    int error = m_insertItemQuery->step();
    if (error != SQLITE_DONE)
    {
        THROW("can't insert");
    }
}
    
void SqliteFsGateway::createHardLinkImpl(int parentId, int itemId, const Path& linkName)
{
    SqliteStmtReseter link(*m_insertLinkQuery);
    
    m_insertLinkQuery->bindInt(1, parentId);
    m_insertLinkQuery->bindInt(2, itemId);
    m_insertLinkQuery->bindText(3, linkName.c_str());
    
    int error = m_insertLinkQuery->step();
    if (error != SQLITE_DONE)
    {
        throw SqliteFsException(FsError::kFileExists, "Such file exists");
    }
}
    
}
