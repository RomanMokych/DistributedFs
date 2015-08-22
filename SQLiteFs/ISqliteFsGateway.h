//
//  ISqliteFsGateway.h
//  DistributedFs
//
//  Created by Роман on 8/18/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#pragma once

#include "IFileSystem.h"
#include "SqliteEntities.h"

#include <vector>
#include <string>

namespace dfs
{

class ISqliteFsGateway
{
public:
    virtual ~ISqliteFsGateway() {};
    
    virtual SqliteEntities::Item getItemByPath(const Path& itemPath, bool followLastSymLink = true) = 0;
    virtual SqliteEntities::Folder getFolderByPath(const Path& folderPath) = 0;
    
    virtual SqliteEntities::Folder getFolderById(int folderId) = 0;
    
    virtual SqliteEntities::Link getLink(int parentId, const Path& name) = 0;
    virtual SqliteEntities::Item getItemById(int itemId) = 0;
    virtual int getItemLinksCount(int itemId) = 0;
    
    virtual SqliteEntities::SymLink getSymLinkById(int symLinkId) = 0;
    
    virtual void createFolder(int parentFolderId, const Path& newFolderName, Permissions permissions) = 0;
    virtual void createFile(int parentFolderId, const Path& newFolderName, Permissions permissions) = 0;
    virtual void createSymLink(int parentFolderId, const Path& newLinkName, Permissions permissions, const Path& symLinkValue) = 0;
    virtual void createHardLink(int parentId, int itemId, const Path& linkName) = 0;
    
    virtual void updateItem(const SqliteEntities::Item& item) = 0;
    
    virtual void removeLink(int linkId) = 0;
    virtual void removeItem(int itemId) = 0;
    
    virtual void readFolderWithId(int folderId, std::vector<FileInfo>* fileInfo) = 0;
    
    virtual void getFileData(int fileId, std::vector<char>* fileData) = 0;
    virtual void updateFileData(int fileId, const std::vector<char>& fileData) = 0;
    
    virtual void addExtendedAttribute(int itemId, const char* attributeKey, const char* attributeValue, const int attributeValueSize) = 0;
    virtual void getExtendedAttribute(int itemId, const char* attributeKey, std::vector<char>* attributeValue) = 0;
    virtual void getExtendedAttributesNames(int itemId, std::vector<std::string>* attributesNames) = 0;
    virtual void deleteExtendedAttribute(int itemId, const char* attributeKey) = 0;
};

}
