//
//  IFileSystem.h
//  DistributedFs
//
//  Created by Роман on 5/31/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <cstdint>
#include <ctime>

#include <boost/filesystem/path.hpp>

namespace dfs
{
    enum class FsError
    {
        kSuccess,
        kFileNotFound,
        kPermissionDenied,
        kNotImplemented,
        kFileExists,
        kAttributeNotFound,
        kFileHasWrongType,
        kReadFolderError,
        kUnknownError
    };
 
    enum class FileType
    {
        kFile,
        kFolder,
        kSymLink
    };
    
    enum class Permissions
    {
        kRead = 1,
        kWrite = 2,
        kExecute = 4,
        kAll = kRead | kWrite | kExecute
    };
    
    class FileOpenMode
    {
    public:
        enum
        {
            kRead = 1,
            kWrite = 2,
            kTruncate = 4,
            kCreate = 8,
            kAppend = 16
        };
    };
    
    typedef boost::filesystem::path Path;

    class IFolder;
    class IFile;
    
    struct FileInfo;
    
    class IFileSystem
    {
    public:
        virtual ~IFileSystem(){}
        
        //folders
        virtual FsError createFolder(const Path& folderPath, const Permissions permissions) = 0;
        virtual FsError openFolder(const Path& folderPath, std::unique_ptr<IFolder>& outFolder) = 0;
        
        //files
        virtual FsError openFile(const Path& filePath, const int fileOpenMode, std::unique_ptr<IFile>& outFile) = 0;
        virtual FsError truncateFile(const Path& filePath, const uint64_t newSize) = 0;
        
        //links
        virtual FsError createSymLink(const Path& linkPath, const Path& targetPath) = 0;
        virtual FsError readSymLink(const Path& linkPath, Path* symLinkValue) = 0;
        
        virtual FsError createHardLink(const Path& linkPath, const Path& targetPath) = 0;
        
        //general
        virtual FsError rename(const Path& oldPath, const Path& newPath) = 0;
        virtual FsError remove(const Path& path) = 0;
        
        virtual FsError getFileInfo(const Path& path, FileInfo* info) = 0;
        
        virtual FsError setPermissions(const Path& path, const Permissions permissions) = 0;
        virtual FsError getPermissions(const Path& path, Permissions* permissions) = 0;
        
        virtual FsError setCreationTime(const Path& path, const std::time_t time) = 0;
        virtual FsError getCreationTime(const Path& path, std::time_t* time) = 0;
        
        virtual FsError setModificationTime(const Path& path, const std::time_t time) = 0;
        virtual FsError getModificationTime(const Path& path, std::time_t* time) = 0;
        
        virtual FsError setExtendedAttribute(const Path& path, const char* attributeKey, const char* attributeValue, const size_t attributeValueSize) = 0;
        virtual FsError getExtendedAttribute(const Path& path, const char* attributeKey, std::vector<char>* attributeValue) = 0;
        virtual FsError deleteExtendedAttribute(const Path& path, const char* attributeKey) = 0;
        virtual FsError getAllExtendedAttributes(const Path& path, std::vector<std::string>* attributesNames) = 0;
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
        
        virtual void seek(uint64_t offset, const SeekPosition position) = 0;
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
        Path name;
        FileType type;
        Permissions permissions;
        
        time_t creationTime;
        time_t modificationTime;
    };
    
    typedef std::unique_ptr<IFileSystem> IFileSystemUPtr;
    typedef std::unique_ptr<IFolder> IFolderUPtr;
    typedef std::unique_ptr<IFile> IFileUPtr;
    
} //dfs