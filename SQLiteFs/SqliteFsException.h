//
//  SqliteFsException.h
//  DistributedFs
//
//  Created by Роман on 8/15/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#pragma once

#include "IFileSystem.h"

#include <stdexcept>

namespace dfs
{

class SqliteFsException : public std::runtime_error
{
public:
    SqliteFsException(FsError error, const std::string& errorStr)
    : std::runtime_error(errorStr)
    , m_error(error)
    {}
    
    FsError getError() const
    {
        return m_error;
    }
    
    std::string getErrorStr() const
    {
        return what();
    }
    
private:
    FsError m_error;
};

}