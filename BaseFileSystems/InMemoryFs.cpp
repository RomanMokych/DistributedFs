//
//  InMemoryFs.cpp
//  DistributedFs
//
//  Created by Роман on 5/31/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include "InMemoryFs.h"

#include "InMemoryFolder.h"
#include "InMemoryFsTreeNode.h"

dfs::InMemoryFs::InMemoryFs()
    : m_superRoot(new details::InMemoryFsTreeNode(FileType::kFolder, Permissions::kAll))
{
    details::InMemoryFsLink rootLink{"/", std::make_shared<details::InMemoryFsTreeNode>(FileType::kFolder, Permissions::kAll)};
    m_superRoot->childLinks.push_back(rootLink);
}

//folders
dfs::FsError dfs::InMemoryFs::createFolder(const Path& folderPath, const Permissions permissions)
{
    details::InMemoryFsTreeNode* parentNode;
    FsError error = details::getNode(folderPath.parent_path(), m_superRoot.get(), &parentNode);
    if (error != FsError::kSuccess)
    {
        return error;
    }
    
    return details::addChildNode(parentNode, folderPath.filename(), FileType::kFolder, permissions);
}

dfs::FsError dfs::InMemoryFs::openFolder(const Path& folderPath, std::unique_ptr<IFolder>& outFolder)
{
    details::InMemoryFsTreeNode* node = nullptr;
    FsError error = details::getNode(folderPath, m_superRoot.get(), &node);
    if (error != FsError::kSuccess)
    {
        return error;
    }
    
    outFolder.reset(new InMemoryFolder(*node));
    return FsError::kSuccess;
}

//files
dfs::FsError dfs::InMemoryFs::openFile(const Path& filePath, const FileOpenMode access, std::unique_ptr<IFile>& outFile)
{ return FsError::kNotImplemented; }

dfs::FsError dfs::InMemoryFs::truncateFile(const Path& filePath, const uint64_t newSize)
{ return FsError::kNotImplemented; }

//links
dfs::FsError dfs::InMemoryFs::createSymLink(const Path& linkPath, const Path& targetPath)
{ return FsError::kNotImplemented; }

dfs::FsError dfs::InMemoryFs::readSymLink(const Path& linkPath, Path* symLinkValue)
{ return FsError::kNotImplemented; }

dfs::FsError dfs::InMemoryFs::createHardLink(const Path& linkPath, const Path& targetPath)
{ return FsError::kNotImplemented; }

//general
dfs::FsError dfs::InMemoryFs::rename(const Path& oldPath, const Path& newPath)
{ return FsError::kNotImplemented; }

dfs::FsError dfs::InMemoryFs::remove(const Path& path)
{ return FsError::kNotImplemented; }

dfs::FsError dfs::InMemoryFs::getFileInfo(const Path& path, FileInfo* info)
{ return FsError::kNotImplemented; }

dfs::FsError dfs::InMemoryFs::setPermissions(const Path& path, const Permissions permissions)
{ return FsError::kNotImplemented; }

dfs::FsError dfs::InMemoryFs::getPermissions(const Path& path, Permissions* permissions)
{ return FsError::kNotImplemented; }

dfs::FsError dfs::InMemoryFs::setCreationTime(const Path& path, const std::time_t time)
{ return FsError::kNotImplemented; }

dfs::FsError dfs::InMemoryFs::getCreationTime(const Path& path, std::time_t* time)
{ return FsError::kNotImplemented; }

dfs::FsError dfs::InMemoryFs::setModificationTime(const Path& path, const std::time_t time)
{ return FsError::kNotImplemented; }

dfs::FsError dfs::InMemoryFs::getModificationTime(const Path& path, std::time_t* time)
{ return FsError::kNotImplemented; }

dfs::FsError dfs::InMemoryFs::setExtendedAttribute(const Path& path, const char* attributeKey, const char* attributeValue, const size_t attributeValueSize)
{ return FsError::kNotImplemented; }

dfs::FsError dfs::InMemoryFs::getExtendedAttribute(const Path& path, const char* attributeKey, std::vector<char>* attributeValue)
{ return FsError::kNotImplemented; }

dfs::FsError dfs::InMemoryFs::getAllExtendedAttributes(const Path& path, std::vector<std::string>* attributesNames)
{ return FsError::kNotImplemented; }
