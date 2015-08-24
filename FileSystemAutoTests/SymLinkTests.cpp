//
//  SymLinkTests.cpp
//  DistributedFs
//
//  Created by Роман on 8/16/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include <gtest/gtest.h>

#include "FsTestFixtures.h"

TYPED_TEST(SymLinksTest, CreateSymLinkFail)
{
    dfs::IFileSystem& fs = this->getFs();
    
    dfs::FsError error = fs.createSymLink("/", "/test");
    EXPECT_EQ(dfs::FsError::fileExists, error);
    
    error = fs.createFolder("/test", dfs::Permissions::kAll);
    ASSERT_EQ(dfs::FsError::success, error);
    
    error = fs.createSymLink("/test", "/test");
    EXPECT_EQ(dfs::FsError::fileExists, error);
    
    error = fs.createSymLink("/test1/test", "/test");
    EXPECT_EQ(dfs::FsError::fileNotFound, error);
}

TYPED_TEST(SymLinksTest, ReadSymLinkFail)
{
    dfs::IFileSystem& fs = this->getFs();
    
    dfs::Path symLinkValue;
    dfs::FsError error = fs.readSymLink("/", &symLinkValue);
    EXPECT_EQ(dfs::FsError::fileHasWrongType, error);

    error = fs.createFolder("/test", dfs::Permissions::kAll);
    ASSERT_EQ(dfs::FsError::success, error);
    
    error = fs.readSymLink("/test", &symLinkValue);
    EXPECT_EQ(dfs::FsError::fileHasWrongType, error);
    
    error = fs.readSymLink("/test/test", &symLinkValue);
    EXPECT_EQ(dfs::FsError::fileNotFound, error);
}

TYPED_TEST(SymLinksTest, ReadSymLink)
{
    dfs::IFileSystem& fs = this->getFs();
    
    dfs::Path symLinkValue;
    dfs::FsError error = fs.createSymLink("/test1", "/test/test");
    ASSERT_EQ(dfs::FsError::success, error);
    
    error = fs.readSymLink("/test1", &symLinkValue);
    ASSERT_EQ(dfs::FsError::success, error);
    EXPECT_EQ("/test/test", symLinkValue);
    
    error = fs.createSymLink("/test2", "/test/test/test");
    ASSERT_EQ(dfs::FsError::success, error);
    
    error = fs.readSymLink("/test2", &symLinkValue);
    ASSERT_EQ(dfs::FsError::success, error);
    EXPECT_EQ("/test/test/test", symLinkValue);
}

TYPED_TEST(SymLinksTest, CreateSymLinkToFolder)
{
    dfs::IFileSystem& fs = this->getFs();
    
    dfs::FsError error = fs.createFolder("/test", dfs::Permissions::kAll);
    ASSERT_EQ(dfs::FsError::success, error);
    
    error = fs.createFolder("/test/A", dfs::Permissions::kAll);
    ASSERT_EQ(dfs::FsError::success, error);
    
    error = fs.createFolder("/test/B", dfs::Permissions::kAll);
    ASSERT_EQ(dfs::FsError::success, error);
    
    error = fs.createFolder("/test/C", dfs::Permissions::kAll);
    ASSERT_EQ(dfs::FsError::success, error);
    
    error = fs.createSymLink("/test_link", "/test");
    ASSERT_EQ(dfs::FsError::success, error);
    
    dfs::IFolderUPtr folder;
    error = fs.openFolder("/test_link", folder);
    ASSERT_EQ(dfs::FsError::success, error);
    
    std::vector<dfs::FileInfo> fileInfos;
    folder->readNextFileInfos(&fileInfos);
    
    ASSERT_EQ(3, fileInfos.size());
    EXPECT_EQ("A", fileInfos[0].name);
    EXPECT_EQ("B", fileInfos[1].name);
    EXPECT_EQ("C", fileInfos[2].name);
}

TYPED_TEST(SymLinksTest, CreateSymLinkToFile)
{
    dfs::IFileSystem& fs = this->getFs();
    
    dfs::FsError error = fs.createFolder("/test", dfs::Permissions::kAll);
    ASSERT_EQ(dfs::FsError::success, error);
    
    error = fs.createFolder("/test/test", dfs::Permissions::kAll);
    ASSERT_EQ(dfs::FsError::success, error);
    
    error = fs.createFolder("/test/test/test", dfs::Permissions::kAll);
    ASSERT_EQ(dfs::FsError::success, error);
    
    dfs::IFileUPtr file;
    error = fs.openFile("/test/test/test/file", dfs::FileOpenMode::kCreate | dfs::FileOpenMode::kRead, file);
    ASSERT_EQ(dfs::FsError::success, error);
    
    file->write("string", 7);
    file.reset();
    
    error = fs.createSymLink("/link", "/test/test/test/file");
    ASSERT_EQ(dfs::FsError::success, error);
    
    error = fs.openFile("/link", dfs::FileOpenMode::kRead, file);
    ASSERT_EQ(dfs::FsError::success, error);
    
    char str[7];
    size_t bytesRead = file->read(str, sizeof(str));
    ASSERT_EQ(sizeof(str), bytesRead);
    
    EXPECT_STREQ("string", str);
}

TYPED_TEST(SymLinksTest, CreateSymLinkToNotExistedItem)
{
    dfs::IFileSystem& fs = this->getFs();
    
    dfs::FsError error = fs.createSymLink("/link", "/not/existed/path");
    ASSERT_EQ(dfs::FsError::success, error);
    
    dfs::IFolderUPtr folder;
    error = fs.openFolder("/link", folder);
    EXPECT_EQ(dfs::FsError::fileNotFound, error);
}

TYPED_TEST(SymLinksTest, CreateSymLinkToSymLink)
{
    dfs::IFileSystem& fs = this->getFs();
    
    dfs::FsError error = fs.createFolder("/test", dfs::Permissions::kAll);
    ASSERT_EQ(dfs::FsError::success, error);
    
    error = fs.createFolder("/test/A", dfs::Permissions::kAll);
    ASSERT_EQ(dfs::FsError::success, error);
    
    error = fs.createSymLink("/link1", "/test");
    ASSERT_EQ(dfs::FsError::success, error);
    
    error = fs.createSymLink("/link2", "/link1");
    ASSERT_EQ(dfs::FsError::success, error);
    
    dfs::IFolderUPtr folder;
    error = fs.openFolder("/link2", folder);
    ASSERT_EQ(dfs::FsError::success, error);
    
    std::vector<dfs::FileInfo> fileInfos;
    folder->readNextFileInfos(&fileInfos);
    
    ASSERT_EQ(1, fileInfos.size());
    EXPECT_EQ("A", fileInfos[0].name);
}
