//
//  InMemoryFs.cpp
//  DistributedFs
//
//  Created by Роман on 5/31/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include "InMemoryFs.h"

#include "InMemoryFile.h"
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
    if (error != FsError::success)
    {
        return error;
    }
    
    return details::addChildNode(parentNode, folderPath.filename(), FileType::kFolder, permissions);
}

dfs::FsError dfs::InMemoryFs::openFolder(const Path& folderPath, std::unique_ptr<IFolder>& outFolder)
{
    details::InMemoryFsTreeNode* node = nullptr;
    FsError error = details::getNode(folderPath, m_superRoot.get(), &node);
    if (error != FsError::success)
    {
        return error;
    }
    
    outFolder.reset(new InMemoryFolder(*node));
    return FsError::success;
}

//files
dfs::FsError dfs::InMemoryFs::openFile(const Path& filePath, const int fileOpenMode, std::unique_ptr<IFile>& outFile)
{
    details::InMemoryFsTreeNode* node = nullptr;
    FsError error = details::getNode(filePath, m_superRoot.get(), &node);
    if (error != FsError::success && !(fileOpenMode & dfs::FileOpenMode::kCreate))
    {
        return error;
    }
    
    if (!node)
    {
        error = details::createFile(filePath, m_superRoot.get(), &node);
        if (error != FsError::success)
        {
            return error;
        }
    }
    
    if (node->type != dfs::FileType::kFile)
    {
        return FsError::fileHasWrongType;
    }
    
    outFile.reset(new InMemoryFile(node));
    
    return FsError::success;
}

dfs::FsError dfs::InMemoryFs::truncateFile(const Path& filePath, const uint64_t newSize)
{ return FsError::notImplemented; }

//links
dfs::FsError dfs::InMemoryFs::createSymLink(const Path& linkPath, const Path& targetPath)
{
    details::InMemoryFsTreeNode* parentNode = nullptr;
    FsError error = details::getNode(linkPath.parent_path(), m_superRoot.get(), &parentNode);
    if (error != FsError::success)
    {
        return error;
    }
    
    error = details::addChildNode(parentNode, linkPath.filename(), FileType::kSymLink, Permissions::kAll);
    if (error != FsError::success)
    {
        return error;
    }

    details::InMemoryFsTreeNode* targetNode = nullptr;
    error = details::getNode(linkPath, m_superRoot.get(), &targetNode);
    if (error != FsError::success)
    {
        return error;
    }
    
    targetNode->symLinkValue = targetPath;
    
    return FsError::success;
}

dfs::FsError dfs::InMemoryFs::readSymLink(const Path& linkPath, Path* symLinkValue)
{
    details::InMemoryFsTreeNode* node = nullptr;
    FsError error = details::getNode(linkPath, m_superRoot.get(), &node);
    if (error != FsError::success)
    {
        return error;
    }

    *symLinkValue = node->symLinkValue;
    
    return FsError::success;
}

dfs::FsError dfs::InMemoryFs::createHardLink(const Path& linkPath, const Path& targetPath)
{
    std::shared_ptr<details::InMemoryFsTreeNode> targetNode;
    FsError error = details::getNodeSPtr(targetPath, m_superRoot, targetNode);
    if (error != FsError::success)
    {
        return error;
    }
    
    std::shared_ptr<details::InMemoryFsTreeNode> parentNode;
    error = details::getNodeSPtr(linkPath.parent_path(), m_superRoot, parentNode);
    if (error != FsError::success)
    {
        return error;
    }
    
    details::InMemoryFsLink newLink{linkPath.leaf(), targetNode};
    return details::addChildLink(parentNode.get(), newLink);
}

//general
dfs::FsError dfs::InMemoryFs::rename(const Path& oldPath, const Path& newPath)
{ return FsError::notImplemented; }

dfs::FsError dfs::InMemoryFs::remove(const Path& path)
{
    if (path == "/")
    {
        return FsError::permissionDenied;
    }
    
    details::InMemoryFsTreeNode* parentNode;
    FsError error = details::getNode(path.parent_path(), m_superRoot.get(), &parentNode);
    if (error != FsError::success)
    {
        return error;
    }
    
    return details::removeChildNode(parentNode, path.filename());
}

dfs::FsError dfs::InMemoryFs::getFileInfo(const Path& path, FileInfo* info)
{ return FsError::notImplemented; }

dfs::FsError dfs::InMemoryFs::setPermissions(const Path& path, const Permissions permissions)
{
    details::InMemoryFsTreeNode* node = nullptr;
    FsError error = details::getNode(path, m_superRoot.get(), &node);
    if (error != FsError::success)
    {
        return error;
    }
    
    node->permissions = permissions;
    return FsError::success;
}

dfs::FsError dfs::InMemoryFs::getPermissions(const Path& path, Permissions* permissions)
{
    details::InMemoryFsTreeNode* node = nullptr;
    FsError error = details::getNode(path, m_superRoot.get(), &node);
    if (error != FsError::success)
    {
        return error;
    }
    
    *permissions = node->permissions;
    return FsError::success;
}

dfs::FsError dfs::InMemoryFs::setCreationTime(const Path& path, const std::time_t time)
{
    details::InMemoryFsTreeNode* node = nullptr;
    FsError error = details::getNode(path, m_superRoot.get(), &node);
    if (error != FsError::success)
    {
        return error;
    }
    
    node->creationTime = time;
    return FsError::success;
}

dfs::FsError dfs::InMemoryFs::getCreationTime(const Path& path, std::time_t* time)
{
    details::InMemoryFsTreeNode* node = nullptr;
    FsError error = details::getNode(path, m_superRoot.get(), &node);
    if (error != FsError::success)
    {
        return error;
    }
    
    *time = node->creationTime;
    return FsError::success;
}

dfs::FsError dfs::InMemoryFs::setModificationTime(const Path& path, const std::time_t time)
{
    details::InMemoryFsTreeNode* node = nullptr;
    FsError error = details::getNode(path, m_superRoot.get(), &node);
    if (error != FsError::success)
    {
        return error;
    }
    
    node->modificationTime = time;
    return FsError::success;
}

dfs::FsError dfs::InMemoryFs::getModificationTime(const Path& path, std::time_t* time)
{
    details::InMemoryFsTreeNode* node = nullptr;
    FsError error = details::getNode(path, m_superRoot.get(), &node);
    if (error != FsError::success)
    {
        return error;
    }
    
    *time = node->modificationTime;
    return FsError::success;
}

dfs::FsError dfs::InMemoryFs::setExtendedAttribute(const Path& path, const char* attributeKey, const char* attributeValue, const size_t attributeValueSize)
{
    details::InMemoryFsTreeNode* node = nullptr;
    FsError error = details::getNode(path, m_superRoot.get(), &node);
    if (error != FsError::success)
    {
        return error;
    }
    
    node->exAttributes[attributeKey] = std::vector<char>(attributeValue, attributeValue + attributeValueSize);
    
    return FsError::success;
}

dfs::FsError dfs::InMemoryFs::getExtendedAttribute(const Path& path, const char* attributeKey, std::vector<char>* attributeValue)
{
    details::InMemoryFsTreeNode* node = nullptr;
    FsError error = details::getNode(path, m_superRoot.get(), &node);
    if (error != FsError::success)
    {
        return error;
    }
    
    auto attributeIt = node->exAttributes.find(attributeKey);
    if (attributeIt == node->exAttributes.end())
    {
        return FsError::attributeNotFound;
    }
    
    *attributeValue = attributeIt->second;
    
    return FsError::success;
}

dfs::FsError dfs::InMemoryFs::deleteExtendedAttribute(const Path& path, const char* attributeKey)
{
    details::InMemoryFsTreeNode* node = nullptr;
    FsError error = details::getNode(path, m_superRoot.get(), &node);
    if (error != FsError::success)
    {
        return error;
    }
    
    node->exAttributes.erase(attributeKey);
    
    return FsError::success;
}

dfs::FsError dfs::InMemoryFs::getAllExtendedAttributes(const Path& path, std::vector<std::string>* attributesNames)
{
    details::InMemoryFsTreeNode* node = nullptr;
    FsError error = details::getNode(path, m_superRoot.get(), &node);
    if (error != FsError::success)
    {
        return error;
    }
    
    attributesNames->clear();
    
    for (auto& pair : node->exAttributes)
    {
        attributesNames->push_back(pair.first);
    }
    
    return FsError::success;
}
