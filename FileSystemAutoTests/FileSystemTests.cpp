//
//  FileSystemTests.cpp
//  DistributedFs
//
//  Created by Роман on 5/31/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include <gtest/gtest.h>

#include <vector>
#include <memory>

#include "InMemoryFs.h"

std::unique_ptr<dfs::IFileSystem> createFileSystem()
{
    return std::unique_ptr<dfs::IFileSystem>(new dfs::InMemoryFs);
}

TEST(FileSystemTest, EmptyRootReadTest)
{
    std::unique_ptr<dfs::IFileSystem> fs(createFileSystem());
    
    std::unique_ptr<dfs::IFolder> folder;
    dfs::FsError error = fs->openFolder("/", folder);
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    std::vector<dfs::FileInfo> fileInfos;
    folder->readNextFileInfos(&fileInfos);
    EXPECT_TRUE(fileInfos.empty());
}

TEST(FileSystemTest, CreateFolderTest)
{
    dfs::IFileSystemUPtr fs(createFileSystem());
    
    dfs::FsError error = fs->createFolder("/test", dfs::Permissions::kRead);
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    dfs::IFolderUPtr folder;
    error = fs->openFolder("/", folder);
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    std::vector<dfs::FileInfo> fileInfos;
    folder->readNextFileInfos(&fileInfos);
    ASSERT_EQ(1, fileInfos.size());
    
    EXPECT_EQ(fileInfos[0].name, "test");
    EXPECT_EQ(fileInfos[0].permissions, dfs::Permissions::kRead);
}

TEST(FileSystemTest, CreateNestedFolderTest)
{
    dfs::IFileSystemUPtr fs(createFileSystem());
    
    dfs::FsError error = fs->createFolder("/test1", dfs::Permissions::kRead);
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    error = fs->createFolder("/test1/test2", dfs::Permissions::kRead);
    ASSERT_EQ(error, dfs::FsError::kSuccess);

    error = fs->createFolder("/test1/test2/test3", dfs::Permissions::kRead);
    ASSERT_EQ(error, dfs::FsError::kSuccess);

    error = fs->createFolder("/test1/test2/test4", dfs::Permissions::kRead);
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    dfs::IFolderUPtr folder;
    error = fs->openFolder("/test1/test2", folder);
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    std::vector<dfs::FileInfo> fileInfos;
    folder->readNextFileInfos(&fileInfos);
    ASSERT_EQ(2, fileInfos.size());
    
    EXPECT_EQ(fileInfos[0].name, "test3");
    EXPECT_EQ(fileInfos[0].permissions, dfs::Permissions::kRead);
    EXPECT_EQ(fileInfos[1].name, "test4");
    EXPECT_EQ(fileInfos[1].permissions, dfs::Permissions::kRead);

}

TEST(FileSystemTest, CreateBadFolder)
{
    auto fs = createFileSystem();
    
    dfs::FsError error = fs->createFolder("/", dfs::Permissions::kRead);
    EXPECT_EQ(error, dfs::FsError::kFileExists);
    
    error = fs->createFolder("/test/test", dfs::Permissions::kRead);
    EXPECT_EQ(error, dfs::FsError::kFileNotFound);
    
    error = fs->createFolder("/test", dfs::Permissions::kRead);
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    error = fs->createFolder("/test", dfs::Permissions::kRead);
    EXPECT_EQ(error, dfs::FsError::kFileExists);
}
