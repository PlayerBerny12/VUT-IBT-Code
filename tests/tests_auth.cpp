// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 2.12.2020                                               ** //
// **   Module: Tests API Auth - code                                    ** //
// ************************************************************************ //

#include "tests.h"

/*
 * Testing auth_key_get function. Expected response 200 OK
 */
TEST_F(APITest, auth_key_get_200)
{
    map<string, string> header;

    // Set expected response
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 200");

    // Execute request and test returned value
    ASSERT_EQ(testAPI->auth_key_get(&header), 200);

    // Check if presented in HTTP header
    TEST_HEADER_EXIST(header, "date");
    
    // Check if value is as expected in HTTP header
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");
    TEST_HEADER_VALUE_EQ(header, "x-api-key", "s5adg65as5nd99fuh");
}

/*
 * Testing auth_key_get function. Expected response 401 Unauthorized
 */
TEST_F(APITest, auth_key_get_401)
{
    map<string, string> header;

    // Set expected response
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 401");

    // Execute request and test returned value
    ASSERT_EQ(testAPI->auth_key_get(&header), 401);
    
    // Check if presented in HTTP header
    TEST_HEADER_EXIST(header, "date");
    
    // Check if value is as expected in HTTP header
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");    
}

/*
 * Testing auth_key_post function. Expected response 201 Created
 */
TEST_F(APITest, auth_key_post_201)
{
    map<string, string> header;
    string username("username");
    string password("password");

    // Set expected response
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 201");

    // Execute request and test returned value
    ASSERT_EQ(testAPI->auth_key_post(username, password, &header), 201);
    
    // Check if presented in HTTP header
    TEST_HEADER_EXIST(header, "date");
    
    // Check if value is as expected in HTTP header
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");
    TEST_HEADER_VALUE_EQ(header, "x-api-key", "t12asd3maa123gko");
}

/*
 * Testing auth_key_post function. Expected response 401 Unauthorized
 */
TEST_F(APITest, auth_key_post_401)
{
    map<string, string> header;
    string username("username");
    string password("password");

    // Set expected response
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 401");

    // Execute request and test returned value
    ASSERT_EQ(testAPI->auth_key_post(username, password, &header), 401);

    // Check if presented in HTTP header
    TEST_HEADER_EXIST(header, "date");
    
    // Check if value is as expected in HTTP header
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");    
}

/*
 * Testing auth_key_delete function. Expected response 204 No Content
 */
TEST_F(APITest, auth_key_delete_204)
{
    map<string, string> header;

    // Set expected response
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 204");

    // Execute request and test returned value
    ASSERT_EQ(testAPI->auth_key_delete(&header), 204);
    
    // Check if presented in HTTP header
    TEST_HEADER_EXIST(header, "date");    
}

/*
 * Testing auth_key_delete function. Expected response 401 Unauthorized
 */
TEST_F(APITest, auth_key_delete_401)
{
    map<string, string> header;

    // Set expected response
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 401");

    // Execute request and test returned value
    ASSERT_EQ(testAPI->auth_key_delete(&header), 401);
    
    // Check if presented in HTTP header
    TEST_HEADER_EXIST(header, "date");
}