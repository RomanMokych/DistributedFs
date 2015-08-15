//
//  Sqlite.h
//  DistributedFs
//
//  Created by Роман on 8/15/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#pragma once

#include <sqlite3.h>
#include <string>
#include <memory>

#include "SqliteStatement.h"

namespace dfs
{
    
class Sqlite
{
public:
    Sqlite(const std::string& filePath);
    ~Sqlite();
    
    sqlite3* get();
    
    void executeQuery(const std::string& query);
    std::unique_ptr<SqliteStatement> createStatement(const std::string& query);
    
    int getLastInsertedRowId();
    
private:
    sqlite3* m_sqlite;
};
    
}
