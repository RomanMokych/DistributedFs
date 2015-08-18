/*
 *  SqliteFs.h
 *  SqliteFs
 *
 *  Created by Роман on 7/19/15.
 *  Copyright (c) 2015 Роман. All rights reserved.
 *
 */

#pragma once

#include "IFileSystem.h"

#include "SqliteFsGateway.h"

namespace dfs
{

class SqliteFs : public IFileSystem
{
public:
    SqliteFs(const Path& fsDbPath);
    SqliteFs(std::unique_ptr<ISqliteFsGateway>&& gateway);
    
    virtual ~SqliteFs() {}
    
    virtual FsError createFolder(const Path& folderPath, const Permissions permissions);
    virtual FsError openFolder(const Path& folderPath, std::unique_ptr<IFolder>& outFolder);
    
    //files
    virtual FsError openFile(const Path& filePath, const int fileOpenMode, std::unique_ptr<IFile>& outFile);
    virtual FsError truncateFile(const Path& filePath, const uint64_t newSize);
    
    //links
    virtual FsError createSymLink(const Path& linkPath, const Path& targetPath);
    virtual FsError readSymLink(const Path& linkPath, Path* symLinkValue);
    
    virtual FsError createHardLink(const Path& linkPath, const Path& targetPath);
    
    //general
    virtual FsError rename(const Path& oldPath, const Path& newPath);
    virtual FsError remove(const Path& path);
    
    virtual FsError getFileInfo(const Path& path, FileInfo* info);
    
    virtual FsError setPermissions(const Path& path, const Permissions permissions);
    virtual FsError getPermissions(const Path& path, Permissions* permissions);
    
    virtual FsError setCreationTime(const Path& path, const std::time_t time);
    virtual FsError getCreationTime(const Path& path, std::time_t* time);
    
    virtual FsError setModificationTime(const Path& path, const std::time_t time);
    virtual FsError getModificationTime(const Path& path, std::time_t* time);
    
    virtual FsError setExtendedAttribute(const Path& path, const char* attributeKey, const char* attributeValue, const size_t attributeValueSize);
    virtual FsError getExtendedAttribute(const Path& path, const char* attributeKey, std::vector<char>* attributeValue);
    virtual FsError deleteExtendedAttribute(const Path& path, const char* attributeKey);
    virtual FsError getAllExtendedAttributes(const Path& path, std::vector<std::string>* attributesNames);
    
private:
    std::unique_ptr<ISqliteFsGateway> m_gateway;
};

}