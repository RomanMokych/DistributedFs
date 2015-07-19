//
//  GeneralFsTestss.cpp
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

TEST(GeneralFsTests, GetModificationTimeFailure)
{
    auto fs = createFileSystem();

    std::time_t time;
    dfs::FsError error = fs->getModificationTime("/test", &time);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TEST(GeneralFsTests, GetModificationTimeAfterCreation)
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

TEST(GeneralFsTests, GetFolderModificationTimeAfterNodeAdding)
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

TEST(GeneralFsTests, SetModificationTimeFailure)
{
    auto fs = createFileSystem();
    
    dfs::FsError error = fs->setModificationTime("/test", 1);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TEST(GeneralFsTests, SetModificationTime)
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

TEST(GeneralFsTests, GetCreationTimeFailure)
{
    auto fs = createFileSystem();
    
    std::time_t time;
    dfs::FsError error = fs->getCreationTime("/test", &time);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TEST(GeneralFsTests, GetCreationTimeAfterCreation)
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

TEST(GeneralFsTests, SetCreationTimeFailure)
{
    auto fs = createFileSystem();
    
    dfs::FsError error = fs->setCreationTime("/test", 1);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TEST(GeneralFsTests, SetCreationTime)
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

TEST(GeneralFsTests, GetExtendedAttributeFail)
{
    auto fs = createFileSystem();
    
    dfs::FsError error = fs->getExtendedAttribute("/test", "attribute", nullptr);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TEST(GeneralFsTests, GetNotExistedExtendedAttribute)
{
    auto fs = createFileSystem();
    
    dfs::FsError error = fs->getExtendedAttribute("/", "attribute", nullptr);
    EXPECT_EQ(dfs::FsError::kAttributeNotFound, error);
}

TEST(GeneralFsTests, SetExtendedAttributeFail)
{
    auto fs = createFileSystem();
    
    std::string attribute = "attribute_value";
    std::vector<char> expectedAttribute(attribute.begin(), attribute.end());
    dfs::FsError error = fs->setExtendedAttribute("/test", "attribute", expectedAttribute.data(), expectedAttribute.size());
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TEST(GeneralFsTests, SetExtendedAttribute)
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

TEST(GeneralFsTests, DeleteExtendedAttributeFail)
{
    auto fs = createFileSystem();
    
    dfs::FsError error = fs->deleteExtendedAttribute("/test", "attribute");
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TEST(GeneralFsTests, DeleteNotEsistedExtendedAttribute)
{
    auto fs = createFileSystem();
    
    dfs::FsError error = fs->deleteExtendedAttribute("/", "attribute");
    EXPECT_EQ(dfs::FsError::kSuccess, error);
}

TEST(GeneralFsTests, DeleteExtendedAttribute)
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

TEST(GeneralFsTests, GetAllExtendedAttributesFail)
{
    auto fs = createFileSystem();
    
    dfs::FsError error = fs->getAllExtendedAttributes("/test", nullptr);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TEST(GeneralFsTests, GetAllExtendedAttributesOfEmptyRoot)
{
    auto fs = createFileSystem();
    
    std::vector<std::string> attributeNames;
    dfs::FsError error = fs->getAllExtendedAttributes("/", &attributeNames);
    ASSERT_EQ(dfs::FsError::kSuccess, error);

    EXPECT_TRUE(attributeNames.empty());
}

TEST(GeneralFsTests, GetAllExtendedAttributes)
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

TEST(GeneralFsTests, CreateHardLinkFail)
{
    auto fs = createFileSystem();
    
    dfs::FsError error = fs->createHardLink("/test", "/test1");
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
    
    error = fs->createHardLink("/test/test", "/");
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
    
    error = fs->createHardLink("/", "/");
    EXPECT_EQ(dfs::FsError::kFileExists, error);
}

TEST(GeneralFsTests, CreateHardLinkForRoot)
{
    auto fs = createFileSystem();
    
    dfs::FsError error = fs->createHardLink("/test", "/");
    EXPECT_EQ(dfs::FsError::kSuccess, error);
    
    dfs::IFolderUPtr folder;
    fs->openFolder("/", folder);
    
    std::vector<dfs::FileInfo> filesInfos;
    folder->readNextFileInfos(&filesInfos);
    
    ASSERT_EQ(1, filesInfos.size());
    EXPECT_EQ("test", filesInfos[0].name);
    EXPECT_EQ(dfs::FileType::kFolder, filesInfos[0].type);

    
    folder.reset();

    fs->openFolder("/test", folder);
    folder->readNextFileInfos(&filesInfos);
    ASSERT_EQ(1, filesInfos.size());
    EXPECT_EQ("test", filesInfos[0].name);
    EXPECT_EQ(dfs::FileType::kFolder, filesInfos[0].type);
}

TEST(GeneralFsTests, CreateHardLinkForFolder)
{
    auto fs = createFileSystem();
    
    fs->createFolder("/test",        dfs::Permissions::kAll);
    fs->createFolder("/test/test",   dfs::Permissions::kAll); 
    fs->createFolder("/test/test/a", dfs::Permissions::kAll);
    fs->createFolder("/test/test/b", dfs::Permissions::kAll);
    
    dfs::FsError error = fs->createHardLink("/test/test_link", "/test/test");
    EXPECT_EQ(dfs::FsError::kSuccess, error);
    
    dfs::IFolderUPtr originalFolder;
    fs->openFolder("/test/test", originalFolder);

    std::vector<dfs::FileInfo> originalFilesInfos;
    originalFolder->readNextFileInfos(&originalFilesInfos);
    
    dfs::IFolderUPtr linkedFolder;
    fs->openFolder("/test/test_link", linkedFolder);

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
