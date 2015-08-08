//
//  SqliteFsFolder.cpp
//  DistributedFs
//
//  Created by Роман on 8/8/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include "SqliteFsFolder.h"

namespace dfs
{

SqliteFsFolder::SqliteFsFolder(int folderId, SQLiteFsGateway* gateway)
    : m_folderId(folderId)
    , m_gateway(gateway)
{}

size_t SqliteFsFolder::readNextFileInfos(std::vector<FileInfo>* fileInfos)
{
    m_gateway->readFolderWithId(m_folderId, fileInfos);
    
    return 0;
}
    
}
