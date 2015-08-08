//
//  FsTestFixture.h
//  DistributedFs
//
//  Created by Роман on 7/19/15.
//  Copyright (c) 2015 Роман. All rights reserved.
//

#pragma once 

#include <gtest/gtest.h>

#include "InMemoryFs.h"
#include "SQLiteFs.h"

template <class FsTestImpl>
class FsTest : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        m_impl.SetUp();
    }
    
    virtual void TearDown()
    {
        m_impl.TearDown();
    }
    
    dfs::IFileSystem& getFs()
    {
        return m_impl.getFs();
    }
    
protected:
    FsTestImpl m_impl;
};

template <class FsTestImpl>
class GeneralFsTest : public FsTest<FsTestImpl>
{};

template <class FsTestImpl>
class FileIoTest : public FsTest<FsTestImpl>
{};

template <class FsTestImpl>
class FolderTest : public FsTest<FsTestImpl>
{};

class InMemoryFsTest
{
public:
    void SetUp()
    {
        m_fs.reset(new dfs::InMemoryFs);
    }
    
    void TearDown()
    {
        m_fs.reset();
    }
    
    dfs::InMemoryFs& getFs()
    {
        return *m_fs;
    }
    
private:
    std::unique_ptr<dfs::InMemoryFs> m_fs;
};

class SQLiteFsTest
{
public:
    const std::string kSQLiteDbPath = "fs.sqlite";
    
public:
    void SetUp()
    {
        std::remove(kSQLiteDbPath.c_str());
        m_fs.reset(new dfs::SQLiteFs(kSQLiteDbPath));
    }
    
    void TearDown()
    {
        m_fs.reset();
        std::remove(kSQLiteDbPath.c_str());
    }
    
    dfs::SQLiteFs& getFs()
    {
        return *m_fs;
    }
    
private:
    std::unique_ptr<dfs::SQLiteFs> m_fs;
};

typedef ::testing::Types<InMemoryFsTest, SQLiteFsTest> FileSystems;

TYPED_TEST_CASE(GeneralFsTest, FileSystems);
TYPED_TEST_CASE(FolderTest,    FileSystems);
TYPED_TEST_CASE(FileIoTest,    FileSystems);
