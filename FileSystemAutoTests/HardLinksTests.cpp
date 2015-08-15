//
//  HardLinksTest.cpp
//  DistributedFs
//
//  Created by Роман on 8/15/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include "FsTestFixtures.h"

#include <gtest/gtest.h>

TYPED_TEST(HardLinksTest, CreateHardLinkFail)
{
    dfs::FsError error = this->getFs().createHardLink("/test", "/test1");
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
    
    error = this->getFs().createHardLink("/test/test", "/");
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
    
    error = this->getFs().createHardLink("/", "/");
    EXPECT_EQ(dfs::FsError::kFileExists, error);
}

TYPED_TEST(HardLinksTest, CreateHardLinkForRoot)
{
    dfs::FsError error = this->getFs().createHardLink("/test", "/");
    EXPECT_EQ(dfs::FsError::kSuccess, error);
    
    dfs::IFolderUPtr folder;
    error = this->getFs().openFolder("/", folder);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    std::vector<dfs::FileInfo> filesInfos;
    folder->readNextFileInfos(&filesInfos);
    
    ASSERT_EQ(1, filesInfos.size());
    EXPECT_EQ("test", filesInfos[0].name);
    EXPECT_EQ(dfs::FileType::kFolder, filesInfos[0].type);
    
    folder.reset();
    
    this->getFs().openFolder("/test", folder);
    folder->readNextFileInfos(&filesInfos);
    ASSERT_EQ(1, filesInfos.size());
    EXPECT_EQ("test", filesInfos[0].name);
    EXPECT_EQ(dfs::FileType::kFolder, filesInfos[0].type);
}

TYPED_TEST(HardLinksTest, CreateHardLinkForFolder)
{
    this->getFs().createFolder("/test",        dfs::Permissions::kAll);
    this->getFs().createFolder("/test/test",   dfs::Permissions::kAll);
    this->getFs().createFolder("/test/test/a", dfs::Permissions::kAll);
    this->getFs().createFolder("/test/test/b", dfs::Permissions::kAll);
    
    dfs::FsError error = this->getFs().createHardLink("/test/test_link", "/test/test");
    EXPECT_EQ(dfs::FsError::kSuccess, error);
    
    dfs::IFolderUPtr originalFolder;
    error = this->getFs().openFolder("/test/test", originalFolder);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    std::vector<dfs::FileInfo> originalFilesInfos;
    originalFolder->readNextFileInfos(&originalFilesInfos);
    
    dfs::IFolderUPtr linkedFolder;
    error = this->getFs().openFolder("/test/test_link", linkedFolder);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    std::vector<dfs::FileInfo> linkedFolderFilesInfos;
    linkedFolder->readNextFileInfos(&linkedFolderFilesInfos);
    
    ASSERT_EQ(2, linkedFolderFilesInfos.size());
    ASSERT_EQ(2, originalFilesInfos.size());
    
    for (int i = 0; i < 2; ++i)
    {
        EXPECT_EQ(linkedFolderFilesInfos[i].name, originalFilesInfos[i].name);
        EXPECT_EQ(linkedFolderFilesInfos[i].type, originalFilesInfos[i].type);
    }
}
