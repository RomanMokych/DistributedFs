//
//  SqliteFsFile.cpp
//  DistributedFs
//
//  Created by Роман on 8/16/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include "SqliteFsFile.h"

namespace dfs
{
    
SqliteFsFile::SqliteFsFile(int fileId, SqliteFsGateway* gateway)
    : m_fileId(fileId)
    , m_gateway(gateway)
{}

SqliteFsFile::~SqliteFsFile()
{
    flushImpl();
}

size_t SqliteFsFile::read(char* buffer, const size_t bufferSize)
{
    return bufferSize;
}

size_t SqliteFsFile::write(const char* buffer, const size_t bufferSize)
{
    return bufferSize;
}

void SqliteFsFile::seek(uint64_t offset, const SeekPosition position)
{}

void SqliteFsFile::flush()
{
    flushImpl();
}

void SqliteFsFile::flushImpl()
{

}

}