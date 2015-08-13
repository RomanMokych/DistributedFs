//
//  SqliteStatement.h
//  DistributedFs
//
//  Created by Роман on 8/13/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#pragma once

#include <string>

#include <sqlite3.h>

namespace dfs
{
    class SqliteStatement
    {
    public:
        SqliteStatement(const std::string& query, sqlite3* sqlite);
        ~SqliteStatement();
        
        sqlite3_stmt* get();
        
        void reset();
        
    private:
        sqlite3_stmt* m_stmt;
    };
}
