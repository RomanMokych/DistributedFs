//
//  SQLiteFsEnitities.h
//  DistributedFs
//
//  Created by Роман on 8/8/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#pragma once

#include "IFileSystem.h"

namespace dfs
{
namespace SQLiteEntities
{
    struct Link
    {
        int id;
        int parentFolderId;
        int itemId;
        Path name;
    };
    
    enum class ItemType
    {
        Folder,
        File,
        Symlink
    };
    
    struct Item
    {
        int id;
        ItemType type;
        int concreteItemId;
        
        Permissions permissions;
    };
    
    struct Folder
    {
        int id;
    };
}
}
