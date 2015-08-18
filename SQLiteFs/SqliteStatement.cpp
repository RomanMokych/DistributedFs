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
    
int SqliteStatement::getIntColumn(int column)
{
    return sqlite3_column_int(m_stmt, column);
}
    
boost::optional<std::string> SqliteStatement::getTextColumn(int column)
{
    boost::optional<std::string> result;

    const char* textPtr = reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, column));
    if (textPtr)
    {
        result = textPtr;
    }
    
    return result;
}
    
boost::optional<std::vector<char>> SqliteStatement::getBlobColumn(int column)
{
    boost::optional<std::vector<char>> result;
    
    const char* blobPtr = reinterpret_cast<const char*>(sqlite3_column_blob(m_stmt, column));
    if (blobPtr)
    {
        int blobSize = sqlite3_column_bytes(m_stmt, column);
        result = std::vector<char>(blobPtr, blobPtr + blobSize);
    }
    
    return result;
}
    
int SqliteStatement::step()
{
    return sqlite3_step(m_stmt);
}
    
}

