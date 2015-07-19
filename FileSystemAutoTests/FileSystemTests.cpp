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

TEST(FileSystemTest, RemoveFolderFailureTest)
{
    auto fs = createFileSystem();
    dfs::FsError error = fs->remove("/");
    EXPECT_EQ(error, dfs::FsError::kPermissionDenied);
    
    error = fs->remove("/test");
    EXPECT_EQ(error, dfs::FsError::kFileNotFound);
}

TEST(FileSystemTest, RemoveOneFolderTest)
{
    auto fs = createFileSystem();
    dfs::FsError error = fs->createFolder("/test1", dfs::Permissions::kRead);
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    error = fs->remove("/test1");
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    std::unique_ptr<dfs::IFolder> folder;
    error = fs->openFolder("/", folder);
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    std::vector<dfs::FileInfo> fileInfos;
    folder->readNextFileInfos(&fileInfos);
    EXPECT_TRUE(fileInfos.empty());
}

TEST(FileSystemTest, RemoveOneOfManyFoldersTest)
{
    auto fs = createFileSystem();
    fs->createFolder("/test1", dfs::Permissions::kRead);
    fs->createFolder("/test2", dfs::Permissions::kRead);
    fs->createFolder("/test3", dfs::Permissions::kRead);
    fs->createFolder("/test4", dfs::Permissions::kRead);
    
    dfs::FsError error = fs->remove("/test2");
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    std::unique_ptr<dfs::IFolder> folder;
    error = fs->openFolder("/", folder);
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    std::vector<dfs::FileInfo> fileInfos;
    folder->readNextFileInfos(&fileInfos);
    ASSERT_EQ(3, fileInfos.size());
    
    EXPECT_EQ(fileInfos[0].name, "test1");
    EXPECT_EQ(fileInfos[1].name, "test3");
    EXPECT_EQ(fileInfos[2].name, "test4");
}

TEST(FileSystemTest, RemoveManyFoldersTest)
{
    auto fs = createFileSystem();
    fs->createFolder("/test1", dfs::Permissions::kRead);
    fs->createFolder("/test2", dfs::Permissions::kRead);
    fs->createFolder("/test3", dfs::Permissions::kRead);
    fs->createFolder("/test4", dfs::Permissions::kRead);
    
    dfs::FsError error = fs->remove("/test1");
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    error = fs->remove("/test2");
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    error = fs->remove("/test3");
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    error = fs->remove("/test4");
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    std::unique_ptr<dfs::IFolder> folder;
    error = fs->openFolder("/", folder);
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    std::vector<dfs::FileInfo> fileInfos;
    folder->readNextFileInfos(&fileInfos);
    EXPECT_TRUE(fileInfos.empty());
}

TEST(FileSystemTest, GetModificationTimeFailure)
{
    auto fs = createFileSystem();

    std::time_t time;
    dfs::FsError error = fs->getModificationTime("/test", &time);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TEST(FileSystemTest, GetModificationTimeAfterCreation)
{
    std::time_t timeBeforeCreation = std::time(nullptr);
    auto fs = createFileSystem();
    
    std::time_t modificationTime;
    dfs::FsError error = fs->getModificationTime("/", &modificationTime);
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    EXPECT_LE(timeBeforeCreation, modificationTime);
    EXPECT_GE(modificationTime, std::time(nullptr));
    
    timeBeforeCreation = std::time(nullptr);
    
    fs->createFolder("/test", dfs::Permissions::kAll);
    error = fs->getModificationTime("/test", &modificationTime);
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    EXPECT_LE(timeBeforeCreation, modificationTime);
    EXPECT_GE(modificationTime, std::time(nullptr));
}

TEST(FileSystemTest, GetFolderModificationTimeAfterNodeAdding)
{
    auto fs = createFileSystem();
    
    std::time_t timeBeforeModification = std::time(nullptr);
    
    fs->createFolder("/test", dfs::Permissions::kAll);
    
    std::time_t modificationTime;
    dfs::FsError error = fs->getModificationTime("/", &modificationTime);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    EXPECT_LE(timeBeforeModification, modificationTime);
    EXPECT_GE(modificationTime, std::time(nullptr));
}

TEST(FileSystemTest, SetModificationTimeFailure)
{
    auto fs = createFileSystem();
    
    dfs::FsError error = fs->setModificationTime("/test", 1);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TEST(FileSystemTest, SetModificationTime)
{
    auto fs = createFileSystem();
    
    dfs::FsError error = fs->setModificationTime("/", 1);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    std::time_t modificationTime;
    error = fs->getModificationTime("/", &modificationTime);
    
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    EXPECT_EQ(1, modificationTime);
    
    fs->createFolder("/test", dfs::Permissions::kAll);
    error = fs->setModificationTime("/test", 1);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    error = fs->getModificationTime("/test", &modificationTime);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    EXPECT_EQ(1, modificationTime);
}

TEST(FileSystemTest, GetCreationTimeFailure)
{
    auto fs = createFileSystem();
    
    std::time_t time;
    dfs::FsError error = fs->getCreationTime("/test", &time);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TEST(FileSystemTest, GetCreationTimeAfterCreation)
{
    std::time_t timeBeforeCreation = std::time(nullptr);
    auto fs = createFileSystem();
    
    std::time_t modificationTime;
    dfs::FsError error = fs->getCreationTime("/", &modificationTime);
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    EXPECT_LE(timeBeforeCreation, modificationTime);
    EXPECT_GE(modificationTime, std::time(nullptr));
    
    timeBeforeCreation = std::time(nullptr);
    
    fs->createFolder("/test", dfs::Permissions::kAll);
    error = fs->getCreationTime("/test", &modificationTime);
    ASSERT_EQ(error, dfs::FsError::kSuccess);
    
    EXPECT_LE(timeBeforeCreation, modificationTime);
    EXPECT_GE(modificationTime, std::time(nullptr));
}

TEST(FileSystemTest, SetCreationTimeFailure)
{
    auto fs = createFileSystem();
    
    dfs::FsError error = fs->setCreationTime("/test", 1);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TEST(FileSystemTest, SetCreationTime)
{
    auto fs = createFileSystem();
    
    dfs::FsError error = fs->setCreationTime("/", 1);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    std::time_t modificationTime;
    error = fs->getCreationTime("/", &modificationTime);
    
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    EXPECT_EQ(1, modificationTime);
    
    fs->createFolder("/test", dfs::Permissions::kAll);
    error = fs->setCreationTime("/test", 1);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    error = fs->getCreationTime("/test", &modificationTime);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    EXPECT_EQ(1, modificationTime);
}

TEST(FileSystemTest, GetExtendedAttributeFail)
{
    auto fs = createFileSystem();
    
    dfs::FsError error = fs->getExtendedAttribute("/test", "attribute", nullptr);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TEST(FileSystemTest, GetNotExistedExtendedAttribute)
{
    auto fs = createFileSystem();
    
    dfs::FsError error = fs->getExtendedAttribute("/", "attribute", nullptr);
    EXPECT_EQ(dfs::FsError::kAttributeNotFound, error);
}

TEST(FileSystemTest, SetExtendedAttributeFail)
{
    auto fs = createFileSystem();
    
    std::string attribute = "attribute_value";
    std::vector<char> expectedAttribute(attribute.begin(), attribute.end());
    dfs::FsError error = fs->setExtendedAttribute("/test", "attribute", expectedAttribute.data(), expectedAttribute.size());
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TEST(FileSystemTest, SetExtendedAttribute)
{
    auto fs = createFileSystem();
    
    std::string attribute = "attribute_value";
    std::vector<char> expectedAttribute(attribute.begin(), attribute.end());
    dfs::FsError error = fs->setExtendedAttribute("/", "attribute", expectedAttribute.data(), expectedAttribute.size());
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    std::vector<char> actualAttribute;
    error = fs->getExtendedAttribute("/", "attribute", &actualAttribute);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    EXPECT_EQ(expectedAttribute, actualAttribute);
}

TEST(FileSystemTest, DeleteExtendedAttributeFail)
{
    auto fs = createFileSystem();
    
    dfs::FsError error = fs->deleteExtendedAttribute("/test", "attribute");
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TEST(FileSystemTest, DeleteNotEsistedExtendedAttribute)
{
    auto fs = createFileSystem();
    
    dfs::FsError error = fs->deleteExtendedAttribute("/", "attribute");
    EXPECT_EQ(dfs::FsError::kSuccess, error);
}

TEST(FileSystemTest, DeleteExtendedAttribute)
{
    auto fs = createFileSystem();
    
    std::string attribute = "attribute_value";
    std::vector<char> expectedAttribute(attribute.begin(), attribute.end());
    dfs::FsError error = fs->setExtendedAttribute("/", "attribute", expectedAttribute.data(), expectedAttribute.size());
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    error = fs->deleteExtendedAttribute("/", "attribute");
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    std::vector<char> actualAttribute;
    error = fs->getExtendedAttribute("/", "attribute", &actualAttribute);
    EXPECT_EQ(dfs::FsError::kAttributeNotFound, error);
}

TEST(FileSystemTest, GetAllExtendedAttributesFail)
{
    auto fs = createFileSystem();
    
    dfs::FsError error = fs->getAllExtendedAttributes("/test", nullptr);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TEST(FileSystemTest, GetAllExtendedAttributesOfEmptyRoot)
{
    auto fs = createFileSystem();
    
    std::vector<std::string> attributeNames;
    dfs::FsError error = fs->getAllExtendedAttributes("/", &attributeNames);
    ASSERT_EQ(dfs::FsError::kSuccess, error);

    EXPECT_TRUE(attributeNames.empty());
}

TEST(FileSystemTest, GetAllExtendedAttributes)
{
    auto fs = createFileSystem();
    
    std::vector<std::string> attributeNames;
    dfs::FsError error = fs->getAllExtendedAttributes("/", &attributeNames);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    EXPECT_TRUE(attributeNames.empty());
    
    std::string attribute = "attribute_value";
    std::vector<char> expectedAttribute(attribute.begin(), attribute.end());
    
    fs->setExtendedAttribute("/", "attribute1", expectedAttribute.data(), expectedAttribute.size());
    fs->setExtendedAttribute("/", "attribute2", expectedAttribute.data(), expectedAttribute.size());
    fs->setExtendedAttribute("/", "attribute3", expectedAttribute.data(), expectedAttribute.size());
    
    error = fs->getAllExtendedAttributes("/", &attributeNames);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    ASSERT_EQ(3, attributeNames.size());
    EXPECT_EQ("attribute1", attributeNames[0]);
    EXPECT_EQ("attribute2", attributeNames[1]);
    EXPECT_EQ("attribute3", attributeNames[2]);
}

TEST(FileSystemTest, OpenFileFail)
{
    auto fs = createFileSystem();
    
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = fs->openFile("/", dfs::FileOpenMode::kRead, file);
    EXPECT_EQ(dfs::FsError::kFileHasWrongType, error);

    error = fs->openFile("/test", dfs::FileOpenMode::kRead, file);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TEST(FileSystemTest, CreateFile)
{
    auto fs = createFileSystem();
    
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = fs->openFile("/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    EXPECT_NE(nullptr, file.get());
    
    fs->createFolder("/test1", dfs::Permissions::kAll);
    
    file.reset();
    error = fs->openFile("/test1/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    EXPECT_NE(nullptr, file.get());
}

TEST(FileSystemTest, ReadEmptyFileTest)
{
    auto fs = createFileSystem();
    
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = fs->openFile("/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    file.reset();
    
    error = fs->openFile("/test", dfs::FileOpenMode::kRead, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    char dummy[10] = {};
    size_t read = file->read(dummy, 10);
    EXPECT_EQ(0, read);
}

TEST(FileSystemTest, ReadWriteFileTest)
{
    auto fs = createFileSystem();
    
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = fs->openFile("/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    char testString1[] = "test string";
    size_t written = file->write(testString1, sizeof(testString1));
    ASSERT_EQ(sizeof(testString1), written);
    
    file.reset();
    
    error = fs->openFile("/test", dfs::FileOpenMode::kRead, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    char testString2[100] = {};
    size_t read = file->read(testString2, sizeof(testString2));
    EXPECT_EQ(sizeof(testString1), read);
    
    EXPECT_STREQ("test string", testString2);
}

TEST(FileSystemTest, ReadNotAllBytes)
{
    auto fs = createFileSystem();
    
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = fs->openFile("/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    char originalBuffer[] = "1234567890";
    file->write(originalBuffer, 10);
    
    file.reset();
    
    error = fs->openFile("/test", dfs::FileOpenMode::kRead, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    char resultBuffer[20] = {0};
    size_t readBytes = file->read(resultBuffer, 20);

    EXPECT_EQ(10, readBytes);
    EXPECT_STREQ("1234567890", resultBuffer);
}

TEST(FileSystemTest, ReadChunksFileTest)
{
    auto fs = createFileSystem();
    
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = fs->openFile("/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    char testString1[] = "123456789";
    size_t written = file->write(testString1, 9);
    ASSERT_EQ(9, written);
    
    file.reset();
    
    error = fs->openFile("/test", dfs::FileOpenMode::kRead, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
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

TEST(FileSystemTest, WriteChunksFileTest)
{
    auto fs = createFileSystem();
    
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = fs->openFile("/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    char testString1[] = "123456789";
    file->write(testString1, 2);
    file->write(testString1 + 2, 2);
    file->write(testString1 + 4, 2);
    file->write(testString1 + 6, 2);
    file->write(testString1 + 8, 2);
    
    file.reset();
    
    error = fs->openFile("/test", dfs::FileOpenMode::kRead, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    char testString2[10] = {};
    file->read(testString2, 10);
    EXPECT_STREQ("123456789", testString2);
}

TEST(FileSystemTest, ReadSeekFromBeginTest)
{
    auto fs = createFileSystem();
    
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = fs->openFile("/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    char originalBuffer[] = "1234567890";
    file->write(originalBuffer, 10);
    file.reset();
    
    error = fs->openFile("/test", dfs::FileOpenMode::kRead, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
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

TEST(FileSystemTest, ReadSeekFromEndTest)
{
    auto fs = createFileSystem();
    
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = fs->openFile("/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    char originalBuffer[] = "1234567890";
    file->write(originalBuffer, 10);
    file.reset();
    
    error = fs->openFile("/test", dfs::FileOpenMode::kRead, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
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

TEST(FileSystemTest, ReadSeekFromCurrentTest)
{
    auto fs = createFileSystem();
    
    std::unique_ptr<dfs::IFile> file;
    dfs::FsError error = fs->openFile("/test", dfs::FileOpenMode::kWrite | dfs::FileOpenMode::kCreate, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    char originalBuffer[] = "1234567890";
    file->write(originalBuffer, 10);
    file.reset();
    
    error = fs->openFile("/test", dfs::FileOpenMode::kRead, file);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    char resultBuffer[10] = {};
    file->read(resultBuffer, 2);
    file->seek(2, dfs::SeekPosition::kCurrent);
    file->read(resultBuffer, 2);
    EXPECT_STREQ("56", resultBuffer);
    
    file->seek(20, dfs::SeekPosition::kCurrent);
    size_t readBytes = file->read(resultBuffer, 2);
    EXPECT_EQ(0, readBytes);
}