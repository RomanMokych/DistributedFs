//
//  SqliteFsFile.cpp
//  DistributedFs
//
//  Created by Роман on 8/16/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include "SqliteFsFile.h"

#include <algorithm>

namespace dfs
{
    
SqliteFsFile::SqliteFsFile(int fileId, ISqliteFsGateway* gateway)
    : m_fileId(fileId)
    , m_gateway(gateway)
    , m_offset(0)
{
    m_gateway->getFileData(fileId, &m_fileData);
}

SqliteFsFile::~SqliteFsFile()
{
    flushImpl();
}

size_t SqliteFsFile::read(char* buffer, const size_t bufferSize)
{
    const size_t bytesToRead = std::min(static_cast<size_t>(m_fileData.size() - m_offset), bufferSize);
    std::copy(m_fileData.begin() + m_offset, m_fileData.begin() + m_offset + bytesToRead, buffer);
    m_offset += bytesToRead;
    
    return bytesToRead;
}

size_t SqliteFsFile::write(const char* buffer, const size_t bufferSize)
{
    if (m_fileData.size() < m_offset + bufferSize)
    {
        m_fileData.resize(m_offset + bufferSize);
    }
    
    std::copy(buffer, buffer + bufferSize, m_fileData.begin() + m_offset);
    m_offset += bufferSize;
    
    return bufferSize;
}

void SqliteFsFile::seek(uint64_t offset, const SeekPosition position)
{
    switch (position)
    {
        case SeekPosition::kBegin:
            m_offset = std::min(offset, static_cast<uint64_t>(m_fileData.size()));
            break;
            
        case SeekPosition::kCurrent:
            m_offset = std::min(m_offset + offset, static_cast<uint64_t>(m_fileData.size()));
            break;
            
        case SeekPosition::kEnd:
            m_offset = std::max(int64_t(0), static_cast<int64_t>(m_fileData.size()) - static_cast<int64_t>(offset));
            break;
    }
}

void SqliteFsFile::flush()
{
    flushImpl();
}

void SqliteFsFile::flushImpl()
{
    m_gateway->updateFileData(m_fileId, m_fileData);
}

}