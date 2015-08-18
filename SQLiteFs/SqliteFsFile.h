//
//  SqliteFsFile.h
//  DistributedFs
//
//  Created by Роман on 8/16/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#pragma once

#include "IFileSystem.h"

#include "SqliteFsGateway.h"

#include <vector>

namespace dfs
{

class SqliteFsFile : public IFile
{
public:
    SqliteFsFile(int fileId, ISqliteFsGateway* gateway);
    ~SqliteFsFile();
    
    virtual size_t read(char* buffer, const size_t bufferSize);
    virtual size_t write(const char* buffer, const size_t bufferSize);
    
    virtual void seek(uint64_t offset, const SeekPosition position);
    virtual void flush();
    
private:
    void flushImpl();
    
private:
    const int m_fileId;
    ISqliteFsGateway* m_gateway;
    
    std::vector<char> m_fileData;
    uint64_t m_offset;
};
    
}
