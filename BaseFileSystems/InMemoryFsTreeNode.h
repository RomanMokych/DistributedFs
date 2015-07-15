//
//  InMemoryFsTreeNode.h
//  DistributedFs
//
//  Created by Роман on 5/31/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#pragma once

#include "IFileSystem.h"

#include <memory>
#include <string>
#include <vector>
#include <map>

namespace dfs
{
    namespace details
    {
        struct InMemoryFsTreeNode;
        
        struct InMemoryFsLink
        {
            dfs::Path name;
            std::shared_ptr<InMemoryFsTreeNode> node;
        };
        
        struct InMemoryFsTreeNode
        {
            InMemoryFsTreeNode(dfs::FileType aType, dfs::Permissions aPermissions)
            : type(aType)
            , permissions(aPermissions)
            {
                std::time_t theCreationTime = std::time(nullptr);
                
                modificationTime = theCreationTime;
                creationTime = theCreationTime;
            }
            
            dfs::FileType type;
            dfs::Permissions permissions;
            
            std::time_t modificationTime;
            std::time_t creationTime;
            
            std::vector<char> fileContent;
            std::vector<InMemoryFsLink> childLinks;
            std::string symLinkValue;
            
            std::map< std::string, std::vector<char> > exAttributes;
        };
        
        FsError getNode(const Path& filePath, InMemoryFsTreeNode* root, InMemoryFsTreeNode** resultNode);
        FsError addChildNode(InMemoryFsTreeNode* parentNode, const Path& name, FileType type, Permissions permissions);
        FsError removeChildNode(InMemoryFsTreeNode* parentNode, const Path& name);
        
        FsError createFile(const Path& filePath, InMemoryFsTreeNode* root, InMemoryFsTreeNode** resultNode);
    }
}
