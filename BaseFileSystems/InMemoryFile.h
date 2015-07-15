//
//  InMemoryFile.h
//  DistributedFs
//
//  Created by Роман on 7/15/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#pragma once

#include "IFileSystem.h"

#include "InMemoryFsTreeNode.h"

#include <memory>

namespace dfs
{
    class InMemoryFile : public IFile
    {
    public:
        InMemoryFile(details::InMemoryFsTreeNode* node);
        
        virtual size_t read(char* buffer, const size_t bufferSize);
        virtual size_t write(const char* buffer, const size_t bufferSize);
        virtual void seek(uint64_t offset, const SeekPosition position);
        virtual void flush();
        
    private:
        details::InMemoryFsTreeNode* m_fileNode;
        size_t m_seekPosition;
    };
}
