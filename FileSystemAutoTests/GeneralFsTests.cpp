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

TYPED_TEST(GeneralFsTest, GetExtendedAttributeFail)
{
    dfs::FsError error = this->getFs().getExtendedAttribute("/test", "attribute", nullptr);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TYPED_TEST(GeneralFsTest, GetNotExistedExtendedAttribute)
{
    dfs::FsError error = this->getFs().getExtendedAttribute("/", "attribute", nullptr);
    EXPECT_EQ(dfs::FsError::kAttributeNotFound, error);
}

TYPED_TEST(GeneralFsTest, SetExtendedAttributeFail)
{
    std::string attribute = "attribute_value";
    std::vector<char> expectedAttribute(attribute.begin(), attribute.end());
    dfs::FsError error = this->getFs().setExtendedAttribute("/test", "attribute", expectedAttribute.data(), expectedAttribute.size());
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TYPED_TEST(GeneralFsTest, SetExtendedAttribute)
{
    std::string attribute = "attribute_value";
    std::vector<char> expectedAttribute(attribute.begin(), attribute.end());
    dfs::FsError error = this->getFs().setExtendedAttribute("/", "attribute", expectedAttribute.data(), expectedAttribute.size());
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    std::vector<char> actualAttribute;
    error = this->getFs().getExtendedAttribute("/", "attribute", &actualAttribute);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    EXPECT_EQ(expectedAttribute, actualAttribute);
}

TYPED_TEST(GeneralFsTest, DeleteExtendedAttributeFail)
{
    dfs::FsError error = this->getFs().deleteExtendedAttribute("/test", "attribute");
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TYPED_TEST(GeneralFsTest, DeleteNotEsistedExtendedAttribute)
{
    dfs::FsError error = this->getFs().deleteExtendedAttribute("/", "attribute");
    EXPECT_EQ(dfs::FsError::kSuccess, error);
}

TYPED_TEST(GeneralFsTest, DeleteExtendedAttribute)
{
    std::string attribute = "attribute_value";
    std::vector<char> expectedAttribute(attribute.begin(), attribute.end());
    dfs::FsError error = this->getFs().setExtendedAttribute("/", "attribute", expectedAttribute.data(), expectedAttribute.size());
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    error = this->getFs().deleteExtendedAttribute("/", "attribute");
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    std::vector<char> actualAttribute;
    error = this->getFs().getExtendedAttribute("/", "attribute", &actualAttribute);
    EXPECT_EQ(dfs::FsError::kAttributeNotFound, error);
}

TYPED_TEST(GeneralFsTest, GetAllExtendedAttributesFail)
{
    dfs::FsError error = this->getFs().getAllExtendedAttributes("/test", nullptr);
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
}

TYPED_TEST(GeneralFsTest, GetAllExtendedAttributesOfEmptyRoot)
{
    std::vector<std::string> attributeNames;
    dfs::FsError error = this->getFs().getAllExtendedAttributes("/", &attributeNames);
    ASSERT_EQ(dfs::FsError::kSuccess, error);

    EXPECT_TRUE(attributeNames.empty());
}

TYPED_TEST(GeneralFsTest, GetAllExtendedAttributes)
{
    std::vector<std::string> attributeNames;
    dfs::FsError error = this->getFs().getAllExtendedAttributes("/", &attributeNames);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    EXPECT_TRUE(attributeNames.empty());
    
    std::string attribute = "attribute_value";
    std::vector<char> expectedAttribute(attribute.begin(), attribute.end());
    
    this->getFs().setExtendedAttribute("/", "attribute1", expectedAttribute.data(), expectedAttribute.size());
    this->getFs().setExtendedAttribute("/", "attribute2", expectedAttribute.data(), expectedAttribute.size());
    this->getFs().setExtendedAttribute("/", "attribute3", expectedAttribute.data(), expectedAttribute.size());
    
    error = this->getFs().getAllExtendedAttributes("/", &attributeNames);
    ASSERT_EQ(dfs::FsError::kSuccess, error);
    
    ASSERT_EQ(3, attributeNames.size());
    EXPECT_EQ("attribute1", attributeNames[0]);
    EXPECT_EQ("attribute2", attributeNames[1]);
    EXPECT_EQ("attribute3", attributeNames[2]);
}

TYPED_TEST(GeneralFsTest, CreateHardLinkFail)
{
    dfs::FsError error = this->getFs().createHardLink("/test", "/test1");
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
    
    error = this->getFs().createHardLink("/test/test", "/");
    EXPECT_EQ(dfs::FsError::kFileNotFound, error);
    
    error = this->getFs().createHardLink("/", "/");
    EXPECT_EQ(dfs::FsError::kFileExists, error);
}

TYPED_TEST(GeneralFsTest, CreateHardLinkForRoot)
{
    dfs::FsError error = this->getFs().createHardLink("/test", "/");
    EXPECT_EQ(dfs::FsError::kSuccess, error);
    
    dfs::IFolderUPtr folder;
    this->getFs().openFolder("/", folder);
    
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

TYPED_TEST(GeneralFsTest, CreateHardLinkForFolder)
{
    this->getFs().createFolder("/test",        dfs::Permissions::kAll);
    this->getFs().createFolder("/test/test",   dfs::Permissions::kAll); 
    this->getFs().createFolder("/test/test/a", dfs::Permissions::kAll);
    this->getFs().createFolder("/test/test/b", dfs::Permissions::kAll);
    
    dfs::FsError error = this->getFs().createHardLink("/test/test_link", "/test/test");
    EXPECT_EQ(dfs::FsError::kSuccess, error);
    
    dfs::IFolderUPtr originalFolder;
    this->getFs().openFolder("/test/test", originalFolder);

    std::vector<dfs::FileInfo> originalFilesInfos;
    originalFolder->readNextFileInfos(&originalFilesInfos);
    
    dfs::IFolderUPtr linkedFolder;
    this->getFs().openFolder("/test/test_link", linkedFolder);

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
