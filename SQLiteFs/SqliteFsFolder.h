//
//  SqliteFsFolder.h
//  DistributedFs
//
//  Created by Роман on 8/8/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#pragma once

#include "IFileSystem.h"

#include "SQLiteFsGateway.h"

namespace dfs
{

class SqliteFsFolder : public IFolder
{
public:
    SqliteFsFolder(int folderId, SQLiteFsGateway* gateway);
    
    virtual size_t readNextFileInfos(std::vector<FileInfo>* fileInfos);
    
private:
    int m_folderId;
    SQLiteFsGateway* m_gateway;
};
    
}

