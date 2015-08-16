//
//  SqliteFsEnitities.h
//  DistributedFs
//
//  Created by Роман on 8/8/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#pragma once

#include "IFileSystem.h"

namespace dfs
{
namespace SqliteEntities
{
    struct Link
    {
        int id;
        int parentFolderId;
        int itemId;
        Path name;
    };
    
    struct Item
    {
        int id;
        FileType type;
        int concreteItemId;
        
        Permissions permissions;
        std::time_t creationTime;
        std::time_t modificationTime;
    };
    
    struct Folder
    {
        int id;
    };
    
    struct SymLink
    {
        int id;
        Path path;
    };
}
}
