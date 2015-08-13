//
//  SqliteStatement.cpp
//  DistributedFs
//
//  Created by Роман on 8/13/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include "SqliteStatement.h"

#include "Utils.h"

namespace dfs
{

SqliteStatement::SqliteStatement(const std::string& query, sqlite3* sqlite)
: m_stmt(nullptr)
{
    int error = sqlite3_prepare(sqlite, query.c_str(), -1, &m_stmt, nullptr);
    if (error != SQLITE_OK)
    {
        THROW("Can't create select request");
    }
}

SqliteStatement::~SqliteStatement()
{
    sqlite3_finalize(m_stmt);
}

sqlite3_stmt* SqliteStatement::get()
{
    return m_stmt;
}

void SqliteStatement::reset()
{
    sqlite3_reset(m_stmt);
}

}

