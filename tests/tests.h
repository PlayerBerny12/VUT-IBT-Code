// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 4.4.2021                                                ** //
// **   Module: Test Main - header                                       ** //
// ************************************************************************ //

#ifndef VDU_TESTS_H
#define VDU_TESTS_H

#include "gtest/gtest.h"
#include <map>
#include "../src/api.h"

#define TEST_HEADER_EXIST(header, key) \
    try{ \
        header.at(key); \
    } \
    catch(const std::exception& e) \
    { \
        FAIL() << "Key: '" << key << "' does not exists in HTTP Response header."; \
    }

#define TEST_HEADER_VALUE_EQ(header, key, value) \
    try{ \
        auto x = header.at(key).c_str(); \
        EXPECT_STREQ(x, value); \
    } \
    catch(const std::exception& e) \
    { \
        FAIL() << "Key: '" << key << "' does not exit in HTTP Response header."; \
    }

class APITest : public ::testing::Test {
protected:
    APITest()
    {
        // You can do set-up work for each test here.                        
        auto database = Database();
        testAPI = new API("https://e526bc93-da2e-4001-94a3-d9fa02033458.mock.pstmn.io/", database);
    }

    ~APITest() override
    {
        // You can do clean-up work that doesn't throw exceptions here.
        delete testAPI;
    }

    void SetUp() override
    {
        // Code here will be called immediately after the constructor (right
        // before each test).
    }

    void TearDown() override
    {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }
    
    API *testAPI;
};

#endif