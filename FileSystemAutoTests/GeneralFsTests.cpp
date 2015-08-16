//
//  GeneralFsTests.cpp
//  DistributedFs
//
//  Created by Роман on 5/31/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include "InMemoryFs.h"
#include "FsTestFixtures.h"

#include <gtest/gtest.h>

#include <vector>
#include <memory>

TYPED_TEST(GeneralFsTest, GetModificationTimeFailure)
{
    std::time_t time;
    
    dfs::FsError error = this->getFs().getModificationTime("/test", &time);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TYPED_TEST(GeneralFsTest, GetModificationTimeAfterCreation)
{
    std::time_t timeBeforeCreation = std::time(nullptr);
    std::time_t modificationTime;
    dfs::FsError error = this->getFs().getModificationTime("/", &modificationTime);
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    EXPECT_LE(timeBeforeCreation, modificationTime);
    EXPECT_GE(modificationTime, std::time(nullptr));
    
    timeBeforeCreation = std::time(nullptr);
    
    this->getFs().createFolder("/test", dfs::Permissions::kAll);
    error = this->getFs().getModificationTime("/test", &modificationTime);
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    EXPECT_LE(timeBeforeCreation, modificationTime);
    EXPECT_GE(modificationTime, std::time(nullptr));
}

TYPED_TEST(GeneralFsTest, GetFolderModificationTimeAfterNodeAdding)
{
    std::time_t timeBeforeModification = std::time(nullptr);
    
    this->getFs().createFolder("/test", dfs::Permissions::kAll);
    
    std::time_t modificationTime;
    dfs::FsError error = this->getFs().getModificationTime("/", &modificationTime);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    EXPECT_LE(timeBeforeModification, modificationTime);
    EXPECT_GE(modificationTime, std::time(nullptr));
}

TYPED_TEST(GeneralFsTest, SetModificationTimeFailure)
{
    dfs::FsError error = this->getFs().setModificationTime("/test", 1);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TYPED_TEST(GeneralFsTest, SetModificationTime)
{
    dfs::FsError error = this->getFs().setModificationTime("/", 1);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    std::time_t modificationTime;
    error = this->getFs().getModificationTime("/", &modificationTime);
    
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    EXPECT_EQ(1, modificationTime);
    
    this->getFs().createFolder("/test", dfs::Permissions::kAll);
    error = this->getFs().setModificationTime("/test", 1);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    error = this->getFs().getModificationTime("/test", &modificationTime);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    EXPECT_EQ(1, modificationTime);
}

TYPED_TEST(GeneralFsTest, GetCreationTimeFailure)
{
    std::time_t time;
    dfs::FsError error = this->getFs().getCreationTime("/test", &time);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TYPED_TEST(GeneralFsTest, GetCreationTimeAfterCreation)
{
    std::time_t timeBeforeCreation = std::time(nullptr);
    std::time_t modificationTime;
    dfs::FsError error = this->getFs().getCreationTime("/", &modificationTime);
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    EXPECT_LE(timeBeforeCreation, modificationTime);
    EXPECT_GE(modificationTime, std::time(nullptr));
    
    timeBeforeCreation = std::time(nullptr);
    
    this->getFs().createFolder("/test", dfs::Permissions::kAll);
    error = this->getFs().getCreationTime("/test", &modificationTime);
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    EXPECT_LE(timeBeforeCreation, modificationTime);
    EXPECT_GE(modificationTime, std::time(nullptr));
}

TYPED_TEST(GeneralFsTest, SetCreationTimeFailure)
{
    dfs::FsError error = this->getFs().setCreationTime("/test", 1);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TYPED_TEST(GeneralFsTest, SetCreationTime)
{
    dfs::FsError error = this->getFs().setCreationTime("/", 1);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    std::time_t modificationTime;
    error = this->getFs().getCreationTime("/", &modificationTime);
    
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    EXPECT_EQ(1, modificationTime);
    
    this->getFs().createFolder("/test", dfs::Permissions::kAll);
    error = this->getFs().setCreationTime("/test", 1);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    error = this->getFs().getCreationTime("/test", &modificationTime);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    EXPECT_EQ(1, modificationTime);
}

TYPED_TEST(GeneralFsTest, SetPermissionsFail)
{
    dfs::IFileSystem& fs = this->getFs();
    
    dfs::FsError error = fs.setPermissions("/test", dfs::Permissions::kRead);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TYPED_TEST(GeneralFsTest, GetPermissionsFail)
{
    dfs::IFileSystem& fs = this->getFs();
    
    dfs::Permissions permissions = dfs::Permissions::kAll;
    dfs::FsError error = fs.getPermissions("/test", &permissions);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TYPED_TEST(GeneralFsTest, GetPermissionsOfRoot)
{
    dfs::IFileSystem& fs = this->getFs();
    
    dfs::Permissions permissions = dfs::Permissions::kAll;
    dfs::FsError error = fs.getPermissions("/", &permissions);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    EXPECT_EQ(dfs::Permissions::kAll, permissions);
}

TYPED_TEST(GeneralFsTest, GetPermissionsOfJustCreatedFolder)
{
    dfs::IFileSystem& fs = this->getFs();
    
    dfs::FsError error = fs.createFolder("/test", dfs::Permissions::kRead);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    dfs::Permissions permissions = dfs::Permissions::kAll;
    error = fs.getPermissions("/test", &permissions);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    EXPECT_EQ(dfs::Permissions::kRead, permissions);
    
    error = fs.createFolder("/test1", dfs::Permissions::kWrite);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    permissions = dfs::Permissions::kAll;
    error = fs.getPermissions("/test1", &permissions);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    EXPECT_EQ(dfs::Permissions::kWrite, permissions);
}

TYPED_TEST(GeneralFsTest, GetPermissionsOfJustCreatedFile)
{
    dfs::IFileSystem& fs = this->getFs();
    
    dfs::IFileUPtr file;
    dfs::FsError error = fs.openFile("/test", dfs::FileOpenMode::kRead | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    dfs::Permissions permissions = dfs::Permissions::kAll;
    error = fs.getPermissions("/test", &permissions);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    EXPECT_EQ(dfs::Permissions::kAll, permissions);
}

TYPED_TEST(GeneralFsTest, SetGetPermissions)
{
    dfs::IFileSystem& fs = this->getFs();
    
    dfs::IFileUPtr file;
    dfs::FsError error = fs.openFile("/test", dfs::FileOpenMode::kRead | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    error = fs.setPermissions("/test", dfs::Permissions::kRead);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    dfs::Permissions permissions = dfs::Permissions::kAll;
    error = fs.getPermissions("/test", &permissions);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    EXPECT_EQ(dfs::Permissions::kRead, permissions);
    
    error = fs.setPermissions("/test", dfs::Permissions::kWrite);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    permissions = dfs::Permissions::kAll;
    error = fs.getPermissions("/test", &permissions);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    EXPECT_EQ(dfs::Permissions::kWrite, permissions);
}

