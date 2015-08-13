//
//  SQLiteFsGateway.h
//  DistributedFs
//
//  Created by Роман on 8/8/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#pragma once

#include "IFileSystem.h"

#include "SQLiteFsEnitities.h"
#include "SqliteStatement.h"

#include <sqlite3.h>

#include <stdexcept>
#include <memory>

namespace dfs
{
    
class SQLiteFsGateway
{
public:
    SQLiteFsGateway(const Path& dbPath);
    ~SQLiteFsGateway();
    
    SQLiteEntities::Item getItemByPath(const Path& itemPath);
    SQLiteEntities::Folder getFolderByPath(const Path& folderPath);
    
    SQLiteEntities::Folder getFolderById(int folderId);
    
    SQLiteEntities::Link getLink(int parentId, const Path& name);
    SQLiteEntities::Item getItemById(int itemId);
    
    void createFolder(int parentFolderId, const Path& newFolderName, Permissions permissions);
    
    void readFolderWithId(int folderId, std::vector<FileInfo>* fileInfo);
    
    void addExtendedAttribute(int itemId, const char* attributeKey, const char* attributeValue, const int attributeValueSize);
    void getExtendedAttribute(int itemId, const char* attributeKey, std::vector<char>* attributeValue);
    void deleteExtendedAttribute(int itemId, const char* attributeKey);
    
private:
    sqlite3* m_sqlite;
    std::unique_ptr<SqliteStatement> m_selectLinkQueryWithParentIdAndName;
    std::unique_ptr<SqliteStatement> m_selectItemQueryWithId;
    std::unique_ptr<SqliteStatement> m_selectFolderQueryWithId;
    std::unique_ptr<SqliteStatement> m_selectLinksWithParentId;
    
    std::unique_ptr<SqliteStatement> m_insertItemQuery;
    std::unique_ptr<SqliteStatement> m_insertFolderQuery;
    std::unique_ptr<SqliteStatement> m_insertLinkQuery;
    
    std::unique_ptr<SqliteStatement> m_insertExtendedAttributeQuery;
    std::unique_ptr<SqliteStatement> m_deleteExtendedAttributeByItemIdAndNameQuery;
    std::unique_ptr<SqliteStatement> m_selectExtendedAttributesByItemIdQuery;
    std::unique_ptr<SqliteStatement> m_selectExtendedAttributeByItemIdAndNameQuery;
    
    int m_superRootFolderId;
};
    
class SQLiteFsException : public std::runtime_error
{
public:
    SQLiteFsException(FsError error, const std::string& errorStr)
    : std::runtime_error(errorStr)
    , m_error(error)
    {}
    
    FsError getError() const
    {
        return m_error;
    }
    
    std::string getErrorStr() const
    {
        return what();
    }
    
private:
    FsError m_error;
};
    
}
