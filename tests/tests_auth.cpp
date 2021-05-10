// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 2.12.2020                                               ** //
// **   Module: Tests API Auth - code                                    ** //
// ************************************************************************ //

#include "tests.h"

TEST_F(APITest, auth_key_get_200)
{
    map<string, string> header;

    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 200");

    ASSERT_EQ(testAPI->auth_key_get(&header), 200);
    
    TEST_HEADER_EXIST(header, "date");
    
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");
    TEST_HEADER_VALUE_EQ(header, "x-api-key", "s5adg65as5nd99fuh");
}

TEST_F(APITest, auth_key_get_401)
{
    map<string, string> header;

    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 401");

    ASSERT_EQ(testAPI->auth_key_get(&header), 401);
    
    TEST_HEADER_EXIST(header, "date");
    
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");    
}

TEST_F(APITest, auth_key_post_201)
{
    map<string, string> header;

    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 201");

    string username("username");
    string password("password");
    ASSERT_EQ(testAPI->auth_key_post(username, password, &header), 201);
    
    TEST_HEADER_EXIST(header, "date");
    
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");
    TEST_HEADER_VALUE_EQ(header, "x-api-key", "t12asd3maa123gko");
}

TEST_F(APITest, auth_key_post_401)
{
    map<string, string> header;

    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 401");

    string username("username");
    string password("password");
    ASSERT_EQ(testAPI->auth_key_post(username, password, &header), 401);
    
    TEST_HEADER_EXIST(header, "date");
    
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");    
}

TEST_F(APITest, auth_key_delete_204)
{
    map<string, string> header;

    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 204");

    ASSERT_EQ(testAPI->auth_key_delete(&header), 204);
    
    TEST_HEADER_EXIST(header, "date");    
}

TEST_F(APITest, auth_key_delete_401)
{
    map<string, string> header;

    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 401");

    ASSERT_EQ(testAPI->auth_key_delete(&header), 401);
    
    TEST_HEADER_EXIST(header, "date");
}