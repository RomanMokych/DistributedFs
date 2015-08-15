//
//  Sqlite.cpp
//  DistributedFs
//
//  Created by Роман on 8/15/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include "Sqlite.h"

#include "Utils.h"

namespace dfs
{

Sqlite::Sqlite(const std::string& dbPath)
    : m_sqlite(nullptr)
{
    int error = sqlite3_open(dbPath.c_str(), &m_sqlite);
    if (error != SQLITE_OK)
    {
        THROW("Can't opend db");
    }
}

Sqlite::~Sqlite()
{
    sqlite3_close(m_sqlite);
}
    
void Sqlite::executeQuery(const std::string& query)
{
    char* errorMsg = nullptr;
    int error = sqlite3_exec(m_sqlite, query.c_str(), nullptr, nullptr, &errorMsg);
    if (error != SQLITE_OK)
    {
        if (errorMsg)
            sqlite3_free(errorMsg);
        
        THROW("Can't create Links table");
    }
}

std::unique_ptr<SqliteStatement> Sqlite::createStatement(const std::string& query)
{
    return std::unique_ptr<SqliteStatement>(new SqliteStatement(query, m_sqlite));
}
    
int Sqlite::getLastInsertedRowId()
{
    return static_cast<int>(sqlite3_last_insert_rowid(m_sqlite));
}
    
}
