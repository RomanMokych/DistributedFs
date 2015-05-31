//
//  InMemoryFolder.h
//  DistributedFs
//
//  Created by Роман on 5/31/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#pragma once

#include "IFileSystem.h"

#include "InMemoryFsTreeNode.h"

namespace dfs
{
    class InMemoryFolder : public IFolder
    {
    public:
        InMemoryFolder(const details::InMemoryFsTreeNode& folderNode);
        
        virtual size_t readNextFileInfos(std::vector<FileInfo>* fileInfos);
        
    private:
        const details::InMemoryFsTreeNode* m_folderNode;
    };
}
