//
//  FileIoTests.cpp
//  DistributedFs
//
//  Created by Роман on 7/19/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include <gtest/gtest.h>

#include "FsTestFixtures.h"

TYPED_TEST(FileIoTest, OpenFileFail)
{
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = this->getFs().openFile("/", dfs::FileOpenMode::kRead, file);
    EXPECT_EQ(dfs::FsError::fileHasWrongType, error);
    
    error = this->getFs().openFile("/test", dfs::FileOpenMode::kRead, file);
    EXPECT_EQ(dfs::FsError::fileNotFound, error);
}

TYPED_TEST(FileIoTest, CreateFile)
{
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = this->getFs().openFile("/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::success, error);
    EXPECT_NE(nullptr, file.get());
    
    this->getFs().createFolder("/test1", dfs::Permissions::kAll);
    
    file.reset();
    error = this->getFs().openFile("/test1/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::success, error);
    EXPECT_NE(nullptr, file.get());
}

TYPED_TEST(FileIoTest, ReadEmptyFileTest)
{
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = this->getFs().openFile("/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::success, error);
    
    file.reset();
    
    error = this->getFs().openFile("/test", dfs::FileOpenMode::kRead, file);
    ASSERT_EQ(dfs::FsError::success, error);
    
    char dummy[10] = {};
    size_t read = file->read(dummy, 10);
    EXPECT_EQ(0, read);
}

TYPED_TEST(FileIoTest, ReadWriteFileTest)
{
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = this->getFs().openFile("/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::success, error);
    
    char testString1[] = "test string";
    size_t written = file->write(testString1, sizeof(testString1));
    ASSERT_EQ(sizeof(testString1), written);
    
    file.reset();
    
    error = this->getFs().openFile("/test", dfs::FileOpenMode::kRead, file);
    ASSERT_EQ(dfs::FsError::success, error);
    
    char testString2[100] = {};
    size_t read = file->read(testString2, sizeof(testString2));
    EXPECT_EQ(sizeof(testString1), read);
    
    EXPECT_STREQ("test string", testString2);
}

TYPED_TEST(FileIoTest, ReadNotAllBytes)
{
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = this->getFs().openFile("/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::success, error);
    
    char originalBuffer[] = "1234567890";
    file->write(originalBuffer, 10);
    
    file.reset();
    
    error = this->getFs().openFile("/test", dfs::FileOpenMode::kRead, file);
    ASSERT_EQ(dfs::FsError::success, error);
    
    char resultBuffer[20] = {0};
    size_t readBytes = file->read(resultBuffer, 20);
    
    EXPECT_EQ(10, readBytes);
    EXPECT_STREQ("1234567890", resultBuffer);
}

TYPED_TEST(FileIoTest, ReadChunksFileTest)
{
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = this->getFs().openFile("/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::success, error);
    
    char testString1[] = "123456789";
    size_t written = file->write(testString1, 9);
    ASSERT_EQ(9, written);
    
    file.reset();
    
    error = this->getFs().openFile("/test", dfs::FileOpenMode::kRead, file);
    ASSERT_EQ(dfs::FsError::success, error);
    
    char testString2[2] = {};
    size_t read = file->read(testString2, 2);
    ASSERT_EQ(read, 2);
    EXPECT_EQ('1', testString2[0]);
    EXPECT_EQ('2', testString2[1]);
    
    read = file->read(testString2, 2);
    ASSERT_EQ(read, 2);
    EXPECT_EQ('3', testString2[0]);
    EXPECT_EQ('4', testString2[1]);
    
    read = file->read(testString2, 2);
    ASSERT_EQ(read, 2);
    EXPECT_EQ('5', testString2[0]);
    EXPECT_EQ('6', testString2[1]);
    
    read = file->read(testString2, 2);
    ASSERT_EQ(read, 2);
    EXPECT_EQ('7', testString2[0]);
    EXPECT_EQ('8', testString2[1]);
    
    read = file->read(testString2, 2);
    ASSERT_EQ(read, 1);
    EXPECT_EQ('9', testString2[0]);
}

TYPED_TEST(FileIoTest, WriteChunksFileTest)
{
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = this->getFs().openFile("/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::success, error);
    
    char testString1[] = "123456789";
    file->write(testString1, 2);
    file->write(testString1 + 2, 2);
    file->write(testString1 + 4, 2);
    file->write(testString1 + 6, 2);
    file->write(testString1 + 8, 2);
    
    file.reset();
    
    error = this->getFs().openFile("/test", dfs::FileOpenMode::kRead, file);
    ASSERT_EQ(dfs::FsError::success, error);
    
    char testString2[10] = {};
    file->read(testString2, 10);
    EXPECT_STREQ("123456789", testString2);
}

TYPED_TEST(FileIoTest, ReadSeekFromBeginTest)
{
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = this->getFs().openFile("/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::success, error);
    
    char originalBuffer[] = "1234567890";
    file->write(originalBuffer, 10);
    file.reset();
    
    error = this->getFs().openFile("/test", dfs::FileOpenMode::kRead, file);
    ASSERT_EQ(dfs::FsError::success, error);
    
    char resultBuffer[10] = {0};
    file->seek(0, dfs::SeekPosition::kBegin);
    file->read(resultBuffer, 2);
    EXPECT_STREQ("12", resultBuffer);
    
    file->seek(4, dfs::SeekPosition::kBegin);
    file->read(resultBuffer, 2);
    EXPECT_STREQ("56", resultBuffer);
    
    std::strcpy(resultBuffer, "");
    file->seek(12, dfs::SeekPosition::kBegin);
    size_t readBytes = file->read(resultBuffer, 2);
    EXPECT_EQ(0, readBytes);
}

TYPED_TEST(FileIoTest, ReadSeekFromEndTest)
{
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = this->getFs().openFile("/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::success, error);
    
    char originalBuffer[] = "1234567890";
    file->write(originalBuffer, 10);
    file.reset();
    
    error = this->getFs().openFile("/test", dfs::FileOpenMode::kRead, file);
    ASSERT_EQ(dfs::FsError::success, error);
    
    file->seek(0, dfs::SeekPosition::kEnd);
    size_t readBytes = file->read(nullptr, 2);
    EXPECT_EQ(0, readBytes);
    
    char resultBuffer[10] = {0};
    
    file->seek(2, dfs::SeekPosition::kEnd);
    file->read(resultBuffer, 2);
    EXPECT_STREQ("90", resultBuffer);
    
    file->seek(6, dfs::SeekPosition::kEnd);
    file->read(resultBuffer, 2);
    EXPECT_STREQ("56", resultBuffer);
    
    file->seek(20, dfs::SeekPosition::kEnd);
    file->read(resultBuffer, 2);
    EXPECT_STREQ("12", resultBuffer);
}

TYPED_TEST(FileIoTest, ReadSeekFromCurrentTest)
{
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = this->getFs().openFile("/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::success, error);
    
    char originalBuffer[] = "1234567890";
    file->write(originalBuffer, 10);
    file.reset();
    
    error = this->getFs().openFile("/test", dfs::FileOpenMode::kRead, file);
    ASSERT_EQ(dfs::FsError::success, error);
    
    char resultBuffer[10] = {};
    file->read(resultBuffer, 2);
    file->seek(2, dfs::SeekPosition::kCurrent);
    file->read(resultBuffer, 2);
    EXPECT_STREQ("56", resultBuffer);
    
    file->seek(20, dfs::SeekPosition::kCurrent);
    size_t readBytes = file->read(resultBuffer, 2);
    EXPECT_EQ(0, readBytes);
}
