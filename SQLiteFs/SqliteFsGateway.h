//
//  SqliteFsGateway.h
//  DistributedFs
//
//  Created by Роман on 8/8/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#pragma once

#include "IFileSystem.h"

#include "Sqlite.h"
#include "SqliteStatement.h"
#include "SqliteEntities.h"

#include <sqlite3.h>

#include <memory>

namespace dfs
{
    
class SqliteFsGateway
{
public:
    SqliteFsGateway(const Path& dbPath);
    
    SqliteEntities::Item getItemByPath(const Path& itemPath);
    SqliteEntities::Folder getFolderByPath(const Path& folderPath);
    
    SqliteEntities::Folder getFolderById(int folderId);
    
    SqliteEntities::Link getLink(int parentId, const Path& name);
    SqliteEntities::Item getItemById(int itemId);
    
    void createFolder(int parentFolderId, const Path& newFolderName, Permissions permissions);
    void createFile(int parentFolderId, const Path& newFolderName, Permissions permissions);
    void createHardLink(int parentId, int itemId, const Path& linkName);
   
    void removeLink(int linkId);
    
    void readFolderWithId(int folderId, std::vector<FileInfo>* fileInfo);
    
    void getFileData(int fileId, std::vector<char>* fileData);
    void updateFileData(int fileId, const std::vector<char>& fileData);
    
    void addExtendedAttribute(int itemId, const char* attributeKey, const char* attributeValue, const int attributeValueSize);
    void getExtendedAttribute(int itemId, const char* attributeKey, std::vector<char>* attributeValue);
    void getExtendedAttributesNames(int itemId, std::vector<std::string>* attributesNames);
    void deleteExtendedAttribute(int itemId, const char* attributeKey);
    
private:
    void createItemImpl(FileType fileType, int concreteItemId, Permissions permissions);
    void createHardLinkImpl(int parentId, int itemId, const Path& linkName);
    
private:
    Sqlite m_sqlite;
    std::unique_ptr<SqliteStatement> m_selectLinkQueryWithParentIdAndName;
    std::unique_ptr<SqliteStatement> m_selectItemQueryWithId;
    std::unique_ptr<SqliteStatement> m_selectFolderQueryWithId;
    std::unique_ptr<SqliteStatement> m_selectLinksWithParentId;
    
    std::unique_ptr<SqliteStatement> m_insertItemQuery;
    std::unique_ptr<SqliteStatement> m_insertFolderQuery;
    std::unique_ptr<SqliteStatement> m_insertFileQuery;
    std::unique_ptr<SqliteStatement> m_insertLinkQuery;
    
    std::unique_ptr<SqliteStatement> m_deleteLinkWithId;

    std::unique_ptr<SqliteStatement> m_selectFileDataWithIdQuery;
    std::unique_ptr<SqliteStatement> m_updateFileDataWithIdQuery;
    
    std::unique_ptr<SqliteStatement> m_insertExtendedAttributeQuery;
    std::unique_ptr<SqliteStatement> m_deleteExtendedAttributeByItemIdAndNameQuery;
    std::unique_ptr<SqliteStatement> m_selectExtendedAttributesByItemIdQuery;
    std::unique_ptr<SqliteStatement> m_selectExtendedAttributeByItemIdAndNameQuery;
    
    int m_superRootFolderId;
};
    
}
