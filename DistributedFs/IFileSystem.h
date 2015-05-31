//
//  IFileSystem.h
//  DistributedFs
//
//  Created by Роман on 5/31/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#pragma once

#include <memory>
#include <stdint>
#include <ctime>

namespace dfs
{
    enum FsError
    {
        kFileNotFound,
        kPermissionDenied,
        kNotImplemented
    };
    
    enum FileType
    {
        kFile,
        kFolder,
        kSymLink
    };
    
    enum Permissions
    {
        kRead = 1,
        kWrite = 2,
        kExecute = 4
    };
    
    enum FileOpenMode
    {
        kRead = 1,
        kWrite = 2,
        kTruncate = 4
    };

    class IFolder;
    class IFile;
    
    struct FileInfo;
    
    class IFileSystem
    {
    public:
        virtual ~IFileSystem(){}
        
        //folders
        virtual FsError createFolder(const char* folderPath, const Permissions permissions) = 0;
        virtual FsError openFolder(const char* folderPath, std::unique_ptr<IFolder>& outFolder) = 0;
        
        //files
        virtual FsError openFile(const char* filePath, const FileOpenMode access, std::unique_ptr<IFile>& outFile) = 0;
        virtual FsError truncateFile(const char* filePath, const uint64_t newSize) = 0;
        
        //links
        virtual FsError createSymLink(const char* linkPath, const char* targetPath) = 0;
        virtual FsError readSymLink(const char* linkPath, std::string* symLinkValue) = 0;
        
        virtual FsError createHardLink(const char* linkPath, const char* targetPath) = 0;
        
        //general
        virtual FsError rename(const char* oldPath, const char* newPath) = 0;
        virtual FsError remove(const char* path) = 0;
        
        virtual FsError getFileInfo(const char* path, FileInfo* info) = 0;
        
        virtual FsError setPermissions(const char* path, const Permissions permissions) = 0;
        virtual FsError getPermissions(const char* path, Permissions* permissions) = 0;
        
        virtual FsError setCreationTime(const char* path, const std::time_t time) = 0;
        virtual FsError getCreationTime(const char* path, std::time_t* time) = 0;
        
        virtual FsError setModificationTime(const char* path, const std::time_t time) = 0;
        virtual FsError getModificationTime(const char* path, std::time_t* time) = 0;
        
        virtual FsError setExtendedAttribute(const char* path, const char* attributeKey, const char* attributeValue, const size_t attributeValueSize) = 0;
        virtual FsError getExtendedAttribute(const char* path, const char* attributeKey, std::vector<char>* attributeValue) = 0;
        virtual FsError getAllExtendedAttributes(const char* path, std::vector<std::string>* attributesNames) = 0;
    };
    
    enum SeekPosition
    {
        kBegin,
        kEnd,
        kCurrent
    };
    
    class IFile
    {
    public:
        virtual ~IFile() {}
        
        virtual size_t read(char* buffer, const size_t bufferSize) = 0;
        virtual size_t write(const char* buffer, const size_t bufferSize) = 0;
        
        virtual seek(uint64_t offset, const SeekPosition position) = 0;
        virtual void flush() = 0;
    };
    
    class IFolder
    {
    public:
        virtual ~IFolder() {}
        
        virtual size_t readNextFileInfos(std::vector<FileInfo>* fileInfos) = 0;
    };
    
    struct FileInfo
    {
        std::string name;
        FileType type;
        Permissions permissions;
        
        time_t creationTime;
        time_t modificationTime;
    };

} //dfs