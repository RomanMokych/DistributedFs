//
//  InMemoryFolder.cpp
//  DistributedFs
//
//  Created by Роман on 5/31/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include "InMemoryFolder.h"

dfs::InMemoryFolder::InMemoryFolder(const details::InMemoryFsTreeNode& folderNode)
: m_folderNode(&folderNode)
{}

size_t dfs::InMemoryFolder::readNextFileInfos(std::vector<dfs::FileInfo>* fileInfos)
{
    fileInfos->clear();
    
    for (int i = 0; i < m_folderNode->childLinks.size(); ++i)
    {
        dfs::FileInfo info;
        
        info.name        = m_folderNode->childLinks[i].name;
        info.type        = m_folderNode->childLinks[i].node->type;
        info.permissions = m_folderNode->childLinks[i].node->permissions;
        
        fileInfos->push_back(info);
    }
    
    return 0;
}