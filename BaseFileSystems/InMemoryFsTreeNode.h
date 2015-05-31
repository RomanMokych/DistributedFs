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
            {}
            
            dfs::FileType type;
            dfs::Permissions permissions;
            
            std::vector<char> fileContent;
            std::vector<InMemoryFsLink> childLinks;
            std::string symLinkValue;
        };
        
        FsError getNode(const Path& filePath, InMemoryFsTreeNode* root, InMemoryFsTreeNode** resultNode);
        FsError addChildNode(InMemoryFsTreeNode* parentNode, const Path& name, FileType type, Permissions permissions);
    }
}
