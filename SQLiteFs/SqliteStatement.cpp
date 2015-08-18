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

void SqliteStatement::bindInt(int index, int value)
{
    int error = sqlite3_bind_int(m_stmt, index, value);
    if (error != SQLITE_OK)
    {
        THROW("can't bind param");
    }
}
    
void SqliteStatement::bindText(int index, const char* text, int textLen /*= -1*/)
{
    int error = sqlite3_bind_text(m_stmt, index, text, textLen, SQLITE_STATIC);
    if (error != SQLITE_OK)
    {
        THROW("can't bind param");
    }
}
    
void SqliteStatement::bindBlob(int index, const char* blob, size_t blobLen)
{
    int error = sqlite3_bind_blob(m_stmt, index, blob, static_cast<int>(blobLen), SQLITE_STATIC);
    if (error != SQLITE_OK)
    {
        THROW("can't bind param");
    }
}
    
}

