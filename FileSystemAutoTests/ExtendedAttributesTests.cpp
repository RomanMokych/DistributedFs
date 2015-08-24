//
//  ExtendedAttributesTests.cpp
//  DistributedFs
//
//  Created by Роман on 8/15/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#include "FsTestFixtures.h"

#include <gtest/gtest.h>

TYPED_TEST(ExtendedAttributesTest, GetExtendedAttributeFail)
{
    dfs::FsError error = this->getFs().getExtendedAttribute("/test", "attribute", nullptr);
    EXPECT_EQ(dfs::FsError::fileNotFound, error);
}

TYPED_TEST(ExtendedAttributesTest, GetNotExistedExtendedAttribute)
{
    dfs::FsError error = this->getFs().getExtendedAttribute("/", "attribute", nullptr);
    EXPECT_EQ(dfs::FsError::attributeNotFound, error);
}

TYPED_TEST(ExtendedAttributesTest, SetExtendedAttributeFail)
{
    std::string attribute = "attribute_value";
    std::vector<char> expectedAttribute(attribute.begin(), attribute.end());
    dfs::FsError error = this->getFs().setExtendedAttribute("/test", "attribute", expectedAttribute.data(), expectedAttribute.size());
    EXPECT_EQ(dfs::FsError::fileNotFound, error);
}

TYPED_TEST(ExtendedAttributesTest, SetExtendedAttribute)
{
    std::string attribute = "attribute_value";
    std::vector<char> expectedAttribute(attribute.begin(), attribute.end());
    dfs::FsError error = this->getFs().setExtendedAttribute("/", "attribute", expectedAttribute.data(), expectedAttribute.size());
    ASSERT_EQ(dfs::FsError::success, error);
    
    std::vector<char> actualAttribute;
    error = this->getFs().getExtendedAttribute("/", "attribute", &actualAttribute);
    ASSERT_EQ(dfs::FsError::success, error);
    
    EXPECT_EQ(expectedAttribute, actualAttribute);
}

TYPED_TEST(ExtendedAttributesTest, DeleteExtendedAttributeFail)
{
    dfs::FsError error = this->getFs().deleteExtendedAttribute("/test", "attribute");
    EXPECT_EQ(dfs::FsError::fileNotFound, error);
}

TYPED_TEST(ExtendedAttributesTest, DeleteNotEsistedExtendedAttribute)
{
    dfs::FsError error = this->getFs().deleteExtendedAttribute("/", "attribute");
    EXPECT_EQ(dfs::FsError::success, error);
}

TYPED_TEST(ExtendedAttributesTest, DeleteExtendedAttribute)
{
    std::string attribute = "attribute_value";
    std::vector<char> expectedAttribute(attribute.begin(), attribute.end());
    dfs::FsError error = this->getFs().setExtendedAttribute("/", "attribute", expectedAttribute.data(), expectedAttribute.size());
    ASSERT_EQ(dfs::FsError::success, error);
    
    error = this->getFs().deleteExtendedAttribute("/", "attribute");
    ASSERT_EQ(dfs::FsError::success, error);
    
    std::vector<char> actualAttribute;
    error = this->getFs().getExtendedAttribute("/", "attribute", &actualAttribute);
    EXPECT_EQ(dfs::FsError::attributeNotFound, error);
}

TYPED_TEST(ExtendedAttributesTest, GetAllExtendedAttributesFail)
{
    dfs::FsError error = this->getFs().getAllExtendedAttributes("/test", nullptr);
    EXPECT_EQ(dfs::FsError::fileNotFound, error);
}

TYPED_TEST(ExtendedAttributesTest, GetAllExtendedAttributesOfEmptyRoot)
{
    std::vector<std::string> attributeNames;
    dfs::FsError error = this->getFs().getAllExtendedAttributes("/", &attributeNames);
    ASSERT_EQ(dfs::FsError::success, error);
    
    EXPECT_TRUE(attributeNames.empty());
}

TYPED_TEST(ExtendedAttributesTest, GetAllExtendedAttributes)
{
    std::vector<std::string> attributeNames;
    dfs::FsError error = this->getFs().getAllExtendedAttributes("/", &attributeNames);
    ASSERT_EQ(dfs::FsError::success, error);
    
    EXPECT_TRUE(attributeNames.empty());
    
    std::string attribute = "attribute_value";
    std::vector<char> expectedAttribute(attribute.begin(), attribute.end());
    
    this->getFs().setExtendedAttribute("/", "attribute1", expectedAttribute.data(), expectedAttribute.size());
    this->getFs().setExtendedAttribute("/", "attribute2", expectedAttribute.data(), expectedAttribute.size());
    this->getFs().setExtendedAttribute("/", "attribute3", expectedAttribute.data(), expectedAttribute.size());
    
    error = this->getFs().getAllExtendedAttributes("/", &attributeNames);
    ASSERT_EQ(dfs::FsError::success, error);
    
    ASSERT_EQ(3, attributeNames.size());
    EXPECT_EQ("attribute1", attributeNames[0]);
    EXPECT_EQ("attribute2", attributeNames[1]);
    EXPECT_EQ("attribute3", attributeNames[2]);
}
