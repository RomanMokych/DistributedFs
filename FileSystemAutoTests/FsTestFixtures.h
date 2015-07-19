//
//  BaseFsTestFixture.h
//  DistributedFs
//
//  Created by Роман on 7/19/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#pragma once 

#include <gtest/gtest.h>

#include "InMemoryFs.h"

template <class FsT>
class BaseFsTest : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        m_fs.reset(new FsT);
    }
    
    virtual void TearDown()
    {
        m_fs.reset();
    }
    
    FsT& getFs()
    {
        return *m_fs;
    }
    
protected:
    std::unique_ptr<FsT> m_fs;
};

template <class FsT>
class GeneralFsTest : public BaseFsTest<FsT>
{};

template <class FsT>
class FileIoTest : public BaseFsTest<FsT>
{};

template <class FsT>
class FolderTest : public BaseFsTest<FsT>
{};

typedef ::testing::Types<dfs::InMemoryFs> FileSystems;

TYPED_TEST_CASE(GeneralFsTest, FileSystems);
TYPED_TEST_CASE(FolderTest,    FileSystems);
TYPED_TEST_CASE(FileIoTest,    FileSystems);
