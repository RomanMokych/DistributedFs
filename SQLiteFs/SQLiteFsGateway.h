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
    
private:
    sqlite3* m_sqlite;
    sqlite3_stmt* m_selectLinkQueryWithParentIdAndName;
    sqlite3_stmt* m_selectItemQueryWithId;
    sqlite3_stmt* m_selectFolderQueryWithId;
    sqlite3_stmt* m_selectLinksWithParentId;
    
    sqlite3_stmt* m_insertItemQuery;
    sqlite3_stmt* m_insertFolderQuery;
    sqlite3_stmt* m_insertLinkQuery;
    
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
