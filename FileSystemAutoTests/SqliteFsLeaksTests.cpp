//
//  SqliteFsLeaksTests.cpp
//  DistributedFs
//
//  Created by Роман on 8/22/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include "SqliteFs.h"
#include "SqliteFsGateway.h"
#include "SqliteFsException.h"

#include <gtest/gtest.h>

#include <memory>
#include <vector>
#include <string>

TEST(SqliteFsLeaksTests, ItemCascadeRemoveTest)
{
    std::unique_ptr<dfs::SqliteFsGateway> fsGateway(new dfs::SqliteFsGateway(":memory:"));
    dfs::SqliteFsGateway* rawFsGateway = fsGateway.get();
    dfs::SqliteFs fs(std::move(fsGateway));

    dfs::FsError error = fs.createFolder("/test", dfs::Permissions::kAll);
    ASSERT_EQ(dfs::FsError::success, error);
    
    dfs::SqliteEntities::Item testItem = rawFsGateway->getItemByPath("/test");
    
    error = fs.remove("/test");
    ASSERT_EQ(dfs::FsError::success, error);
    
    EXPECT_THROW(rawFsGateway->getItemById(testItem.id), dfs::SqliteFsException);
}

TEST(SqliteFsLeaksTests, FolderCascadeRemoveTest)
{
    std::unique_ptr<dfs::SqliteFsGateway> fsGateway(new dfs::SqliteFsGateway(":memory:"));
    dfs::SqliteFsGateway* rawFsGateway = fsGateway.get();
    dfs::SqliteFs fs(std::move(fsGateway));
    
    dfs::FsError error = fs.createFolder("/test", dfs::Permissions::kAll);
    ASSERT_EQ(dfs::FsError::success, error);
    
    dfs::SqliteEntities::Item testItem = rawFsGateway->getItemByPath("/test");
    
    error = fs.remove("/test");
    ASSERT_EQ(dfs::FsError::success, error);
    
    EXPECT_THROW(rawFsGateway->getFolderById(testItem.concreteItemId), dfs::SqliteFsException);
}

TEST(SqliteFsLeaksTests, FileCascadeRemoveTest)
{
    std::unique_ptr<dfs::SqliteFsGateway> fsGateway(new dfs::SqliteFsGateway(":memory:"));
    dfs::SqliteFsGateway* rawFsGateway = fsGateway.get();
    dfs::SqliteFs fs(std::move(fsGateway));
    
    dfs::IFileUPtr file;
    dfs::FsError error = fs.openFile("/test", dfs::FileOpenMode::kCreate | dfs::FileOpenMode::kRead, file);
    ASSERT_EQ(dfs::FsError::success, error);
    
    file.reset();
    
    dfs::SqliteEntities::Item testItem = rawFsGateway->getItemByPath("/test");
    
    error = fs.remove("/test");
    ASSERT_EQ(dfs::FsError::success, error);
    
    std::vector<char> fileData;
    EXPECT_THROW(rawFsGateway->getFileData(testItem.concreteItemId, &fileData), dfs::SqliteFsException);
}

TEST(SqliteFsLeaksTests, SymLinkCascadeRemoveTest)
{
    std::unique_ptr<dfs::SqliteFsGateway> fsGateway(new dfs::SqliteFsGateway(":memory:"));
    dfs::SqliteFsGateway* rawFsGateway = fsGateway.get();
    dfs::SqliteFs fs(std::move(fsGateway));

    dfs::FsError error = fs.createSymLink("/test", "/test_target");
    ASSERT_EQ(dfs::FsError::success, error);
    
    dfs::SqliteEntities::Item testItem = rawFsGateway->getItemByPath("/test", false);
    
    error = fs.remove("/test");
    ASSERT_EQ(dfs::FsError::success, error);
    
    EXPECT_THROW(rawFsGateway->getSymLinkById(testItem.concreteItemId), dfs::SqliteFsException);
}

TEST(SqliteFsLeaksTests, ExtendedAttributesCascadeRemoveTest)
{
    std::unique_ptr<dfs::SqliteFsGateway> fsGateway(new dfs::SqliteFsGateway(":memory:"));
    dfs::SqliteFsGateway* rawFsGateway = fsGateway.get();
    dfs::SqliteFs fs(std::move(fsGateway));
    
    dfs::FsError error = fs.createFolder("/test", dfs::Permissions::kAll);
    ASSERT_EQ(dfs::FsError::success, error);
    
    std::string attrValue = "value";
    error = fs.setExtendedAttribute("/test", "attr1", attrValue.c_str(), attrValue.size());
    ASSERT_EQ(dfs::FsError::success, error);

    error = fs.setExtendedAttribute("/test", "attr2", attrValue.c_str(), attrValue.size());
    ASSERT_EQ(dfs::FsError::success, error);

    error = fs.setExtendedAttribute("/test", "attr3", attrValue.c_str(), attrValue.size());
    ASSERT_EQ(dfs::FsError::success, error);

    dfs::SqliteEntities::Item testItem = rawFsGateway->getItemByPath("/test");
    
    error = fs.remove("/test");
    ASSERT_EQ(dfs::FsError::success, error);
    
    std::vector<std::string> attrNames;
    rawFsGateway->getExtendedAttributesNames(testItem.id, &attrNames);
    EXPECT_TRUE(attrNames.empty());
}
