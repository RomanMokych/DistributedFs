//
//  InMemoryFile.cpp
//  DistributedFs
//
//  Created by Роман on 7/15/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include "InMemoryFile.h"

dfs::InMemoryFile::InMemoryFile(details::InMemoryFsTreeNode* node)
: m_fileNode(node)
, m_seekPosition(0)
{}

size_t dfs::InMemoryFile::read(char* buffer, const size_t bufferSize)
{
    size_t bytesToRead = std::min(bufferSize, m_fileNode->fileContent.size() - m_seekPosition);
    auto srcPosition = m_fileNode->fileContent.data() + m_seekPosition;

    std::copy_n(srcPosition, bytesToRead, buffer);
    m_seekPosition += bytesToRead;
    
    return bytesToRead;
}

size_t dfs::InMemoryFile::write(const char* buffer, const size_t bufferSize)
{
    if (m_fileNode->fileContent.size() < (bufferSize + m_seekPosition))
    {
        m_fileNode->fileContent.resize(bufferSize + m_seekPosition);
    }
    
    auto dstPosition = m_fileNode->fileContent.data() + m_seekPosition;
    std::copy_n(buffer, bufferSize, dstPosition);
    m_seekPosition += bufferSize;
    
    return bufferSize;
}

void dfs::InMemoryFile::seek(uint64_t offset, const dfs::SeekPosition position)
{
    switch (position)
    {
        case dfs::SeekPosition::kBegin:
            m_seekPosition = std::min(static_cast<size_t>(offset), m_fileNode->fileContent.size());
            break;

        case dfs::SeekPosition::kEnd:
            m_seekPosition = 0;
            if (m_fileNode->fileContent.size() > offset)
            {
                m_seekPosition = m_fileNode->fileContent.size() - offset;
            }
            break;

        case dfs::SeekPosition::kCurrent:
            m_seekPosition += offset;
            m_seekPosition = std::min(m_fileNode->fileContent.size(), m_seekPosition);
            break;
    }
}

void dfs::InMemoryFile::flush()
{}
