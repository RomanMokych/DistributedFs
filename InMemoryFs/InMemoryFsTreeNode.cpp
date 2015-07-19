//
//  InMemoryFsTreeNode.cpp
//  DistributedFs
//
//  Created by Роман on 5/31/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include "InMemoryFsTreeNode.h"

namespace dfs
{
namespace details
{

FsError getNode(const Path& filePath, InMemoryFsTreeNode* root, InMemoryFsTreeNode** resultNode)
{
    InMemoryFsTreeNode* fsIt = root;
    auto pathIt = filePath.begin();
    
    while (pathIt != filePath.end())
    {
        bool foundComponent = false;
        for (int i = 0; i < fsIt->childLinks.size(); ++i)
        {
            if (fsIt->childLinks[i].name == *pathIt)
            {
                foundComponent = true;
                fsIt = fsIt->childLinks[i].node.get();
                break;
            }
        }
        
        if (!foundComponent)
        {
            return FsError::kFileNotFound;
        }
        
        ++pathIt;
    }
    
    *resultNode = fsIt;
    return FsError::kSuccess;
}
    
FsError getNodeSPtr(const Path& filePath, std::shared_ptr<InMemoryFsTreeNode> root, std::shared_ptr<InMemoryFsTreeNode>& resultNode)
{
    std::shared_ptr<InMemoryFsTreeNode> fsIt = root;
    auto pathIt = filePath.begin();
    
    while (pathIt != filePath.end())
    {
        bool foundComponent = false;
        for (int i = 0; i < fsIt->childLinks.size(); ++i)
        {
            if (fsIt->childLinks[i].name == *pathIt)
            {
                foundComponent = true;
                fsIt = fsIt->childLinks[i].node;
                break;
            }
        }
        
        if (!foundComponent)
        {
            return FsError::kFileNotFound;
        }
        
        ++pathIt;
    }
    
    resultNode = fsIt;
    return FsError::kSuccess;
}

FsError addChildNode(InMemoryFsTreeNode* parentNode, const Path& name, FileType type, Permissions permissions)
{
    InMemoryFsLink newLink{name, std::make_shared<InMemoryFsTreeNode>(type, permissions)};

    FsError error = addChildLink(parentNode, newLink);
    if (error != FsError::kSuccess)
    {
        return error;
    }
    
    parentNode->modificationTime = std::time(nullptr);
    
    return FsError::kSuccess;
}
    
FsError addChildLink(InMemoryFsTreeNode* parentNode, const InMemoryFsLink& childLink)
{
    for (int i = 0; i < parentNode->childLinks.size(); ++i)
    {
        if (parentNode->childLinks[i].name == childLink.name)
        {
            return FsError::kFileExists;
        }
    }
    
    parentNode->childLinks.push_back(childLink);
    
    return FsError::kSuccess;
}
    
FsError removeChildNode(InMemoryFsTreeNode* parentNode, const Path& name)
{
    for (int i = 0; i < parentNode->childLinks.size(); ++i)
    {
        if (parentNode->childLinks[i].name == name)
        {
            parentNode->childLinks.erase(parentNode->childLinks.begin() + i);
            return FsError::kSuccess;
        }
    }
    
    return FsError::kFileNotFound;
}
    
FsError createFile(const Path& filePath, InMemoryFsTreeNode* root, InMemoryFsTreeNode** resultNode)
{
    InMemoryFsTreeNode* parentNode = nullptr;
    FsError error = getNode(filePath.parent_path(), root, &parentNode);
    if (error != FsError::kSuccess)
    {
        return error;
    }
    
    error = addChildNode(parentNode, filePath.leaf(), dfs::FileType::kFile, dfs::Permissions::kAll);
    if (error != FsError::kSuccess)
    {
        return error;
    }
    
    return getNode(filePath, root, resultNode);
}

}
}

