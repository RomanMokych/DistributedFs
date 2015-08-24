//
//  FolderTest.cpp
//  DistributedFs
//
//  Created by Роман on 7/19/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include "FsTestFixtures.h"

#include <gtest/gtest.h>
#include <memory>

TYPED_TEST(FolderTest, CreateFolderFail)
{
    dfs::FsError error = this->getFs().createFolder("/", dfs::Permissions::kRead);
    EXPECT_EQ(error, dfs::FsError::fileExists);
    
    error = this->getFs().createFolder("/test/test", dfs::Permissions::kRead);
    EXPECT_EQ(error, dfs::FsError::fileNotFound);
    
    error = this->getFs().createFolder("/test", dfs::Permissions::kRead);
    ASSERT_EQ(error, dfs::FsError::success);
    
    error = this->getFs().createFolder("/test", dfs::Permissions::kRead);
    EXPECT_EQ(error, dfs::FsError::fileExists);
}

TYPED_TEST(FolderTest, OpenFolderFail)
{
    dfs::IFileSystem& fs = this->getFs();
    dfs::IFolderUPtr folder;
    
    dfs::FsError error = fs.openFolder("/test", folder);
    EXPECT_EQ(dfs::FsError::fileNotFound, error);
    
    error = fs.openFolder("/test/test", folder);
    EXPECT_EQ(dfs::FsError::fileNotFound, error);
}

TYPED_TEST(FolderTest, EmptyRootReadTest)
{
    std::unique_ptr<dfs::IFolder> folder;
    dfs::FsError error = this->getFs().openFolder("/", folder);
    ASSERT_EQ(error, dfs::FsError::success);
    
    std::vector<dfs::FileInfo> fileInfos;
    folder->readNextFileInfos(&fileInfos);
    EXPECT_TRUE(fileInfos.empty());
}

TYPED_TEST(FolderTest, CreateFolderTest)
{
    dfs::FsError error = this->getFs().createFolder("/test", dfs::Permissions::kRead);
    ASSERT_EQ(error, dfs::FsError::success);
    
    dfs::IFolderUPtr folder;
    error = this->getFs().openFolder("/", folder);
    ASSERT_EQ(error, dfs::FsError::success);
    
    std::vector<dfs::FileInfo> fileInfos;
    folder->readNextFileInfos(&fileInfos);
    ASSERT_EQ(1, fileInfos.size());
    
    EXPECT_EQ(fileInfos[0].name, "test");
    EXPECT_EQ(fileInfos[0].permissions, dfs::Permissions::kRead);
}

TYPED_TEST(FolderTest, CreateNestedFolderTest)
{
    dfs::FsError error = this->getFs().createFolder("/test1", dfs::Permissions::kRead);
    ASSERT_EQ(error, dfs::FsError::success);
    
    error = this->getFs().createFolder("/test1/test2", dfs::Permissions::kRead);
    ASSERT_EQ(error, dfs::FsError::success);
    
    error = this->getFs().createFolder("/test1/test2/test3", dfs::Permissions::kRead);
    ASSERT_EQ(error, dfs::FsError::success);
    
    error = this->getFs().createFolder("/test1/test2/test4", dfs::Permissions::kRead);
    ASSERT_EQ(error, dfs::FsError::success);
    
    dfs::IFolderUPtr folder;
    error = this->getFs().openFolder("/test1/test2", folder);
    ASSERT_EQ(error, dfs::FsError::success);
    
    std::vector<dfs::FileInfo> fileInfos;
    folder->readNextFileInfos(&fileInfos);
    ASSERT_EQ(2, fileInfos.size());
    
    EXPECT_EQ(fileInfos[0].name, "test3");
    EXPECT_EQ(fileInfos[0].permissions, dfs::Permissions::kRead);
    EXPECT_EQ(fileInfos[1].name, "test4");
    EXPECT_EQ(fileInfos[1].permissions, dfs::Permissions::kRead);
}

TYPED_TEST(FolderTest, RemoveFolderFailureTest)
{
    dfs::FsError error = this->getFs().remove("/");
    EXPECT_EQ(error, dfs::FsError::permissionDenied);
    
    error = this->getFs().remove("/test");
    EXPECT_EQ(error, dfs::FsError::fileNotFound);
}

TYPED_TEST(FolderTest, RemoveOneFolderTest)
{
    dfs::FsError error = this->getFs().createFolder("/test1", dfs::Permissions::kRead);
    ASSERT_EQ(error, dfs::FsError::success);
    
    error = this->getFs().remove("/test1");
    ASSERT_EQ(error, dfs::FsError::success);
    
    std::unique_ptr<dfs::IFolder> folder;
    error = this->getFs().openFolder("/", folder);
    ASSERT_EQ(error, dfs::FsError::success);
    
    std::vector<dfs::FileInfo> fileInfos;
    folder->readNextFileInfos(&fileInfos);
    EXPECT_TRUE(fileInfos.empty());
}

TYPED_TEST(FolderTest, RemoveOneOfManyFoldersTest)
{
    this->getFs().createFolder("/test1", dfs::Permissions::kRead);
    this->getFs().createFolder("/test2", dfs::Permissions::kRead);
    this->getFs().createFolder("/test3", dfs::Permissions::kRead);
    this->getFs().createFolder("/test4", dfs::Permissions::kRead);
    
    dfs::FsError error = this->getFs().remove("/test2");
    ASSERT_EQ(error, dfs::FsError::success);
    
    std::unique_ptr<dfs::IFolder> folder;
    error = this->getFs().openFolder("/", folder);
    ASSERT_EQ(error, dfs::FsError::success);
    
    std::vector<dfs::FileInfo> fileInfos;
    folder->readNextFileInfos(&fileInfos);
    ASSERT_EQ(3, fileInfos.size());
    
    EXPECT_EQ(fileInfos[0].name, "test1");
    EXPECT_EQ(fileInfos[1].name, "test3");
    EXPECT_EQ(fileInfos[2].name, "test4");
}

TYPED_TEST(FolderTest, RemoveManyFoldersTest)
{
    this->getFs().createFolder("/test1", dfs::Permissions::kRead);
    this->getFs().createFolder("/test2", dfs::Permissions::kRead);
    this->getFs().createFolder("/test3", dfs::Permissions::kRead);
    this->getFs().createFolder("/test4", dfs::Permissions::kRead);
    
    dfs::FsError error = this->getFs().remove("/test1");
    ASSERT_EQ(error, dfs::FsError::success);
    
    error = this->getFs().remove("/test2");
    ASSERT_EQ(error, dfs::FsError::success);
    
    error = this->getFs().remove("/test3");
    ASSERT_EQ(error, dfs::FsError::success);
    
    error = this->getFs().remove("/test4");
    ASSERT_EQ(error, dfs::FsError::success);
    
    std::unique_ptr<dfs::IFolder> folder;
    error = this->getFs().openFolder("/", folder);
    ASSERT_EQ(error, dfs::FsError::success);
    
    std::vector<dfs::FileInfo> fileInfos;
    folder->readNextFileInfos(&fileInfos);
    EXPECT_TRUE(fileInfos.empty());
}
