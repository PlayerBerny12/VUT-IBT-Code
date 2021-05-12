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
#include "../../src/api.h"

// Macro for testing if key exists in HTTP header (std::map<string, string>)
#define TEST_HEADER_EXIST(header, key) \
    try{ \
        header.at(key); \
    } \
    catch(const std::exception& e) \
    { \
        FAIL() << "Key: '" << key << "' does not exists in HTTP Response header."; \
    }

// Macro for testing if key exists and also if value is same as expected
// in HTTP header (std::map<string, string>)
#define TEST_HEADER_VALUE_EQ(header, key, value) \
    try{ \
        auto x = header.at(key).c_str(); \
        EXPECT_STREQ(x, value); \
    } \
    catch(const std::exception& e) \
    { \
        FAIL() << "Key: '" << key << "' does not exit in HTTP Response header."; \
    }

/**
 * Tříd APITest vytváří prostředí pro jednotlivé testy. Alokuje a dealokuje
 * potřebné struktury a objekty pro jednotlivé testy.
 */
class APITest : public ::testing::Test {
protected:
    APITest()
    {        
        auto database = Database();
        testAPI = new API("https://e526bc93-da2e-4001-94a3-d9fa02033458.mock.pstmn.io/", database);
    }

    ~APITest() override
    {        
        delete testAPI;
    }
    
    API *testAPI;
};

#endif