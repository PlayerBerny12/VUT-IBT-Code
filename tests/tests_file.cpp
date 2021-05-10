// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 2.12.2020                                               ** //
// **   Module: Tests API File - code                                    ** //
// ************************************************************************ //

#include "tests.h"

TEST_F(APITest, file_get_200)
{
    map<string, string> header;
    char *content = nullptr;
    size_t size = 0;
    
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 200");

    ASSERT_EQ(testAPI->file_get("test123", &header, &size, &content), 200);

    TEST_HEADER_EXIST(header, "date");
    TEST_HEADER_EXIST(header, "content-encoding");
    TEST_HEADER_EXIST(header, "content-type");
    TEST_HEADER_EXIST(header, "etag");
    
    TEST_HEADER_VALUE_EQ(header, "allow", "GET");    
    TEST_HEADER_VALUE_EQ(header, "content-location", "Test Filename");
    TEST_HEADER_VALUE_EQ(header, "content-md5", "e0642264a62cafede3e255c44f561449");
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");
    TEST_HEADER_VALUE_EQ(header, "last-modified", "Fri, 30 Apr 2021 11:39:57 GMT");

    EXPECT_EQ(size, 24);
    EXPECT_STREQ(content, "conent of requested file");
}

TEST_F(APITest, file_get_401)
{
    map<string, string> header;
    char *content = nullptr;
    size_t size = 0;
    
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 401");

    ASSERT_EQ(testAPI->file_get("test123", &header, &size, &content), 401);

    TEST_HEADER_EXIST(header, "date");
    
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");
    
    EXPECT_EQ(size, 0);
    EXPECT_STREQ(content, nullptr);
}

TEST_F(APITest, file_get_404)
{
    map<string, string> header;
    char *content = nullptr;
    size_t size = 0;
    
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 404");

    ASSERT_EQ(testAPI->file_get("test123", &header, &size, &content), 404);

    TEST_HEADER_EXIST(header, "date");
    
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");
    
    EXPECT_EQ(size, 0);
    EXPECT_STREQ(content, nullptr);
}

TEST_F(APITest, file_get_405)
{
    map<string, string> header;
    char *content = nullptr;
    size_t size = 0;
    
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 405");

    ASSERT_EQ(testAPI->file_get("test123", &header, &size, &content), 405);

    TEST_HEADER_EXIST(header, "date");
    
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");
    
    EXPECT_EQ(size, 0);
    EXPECT_STREQ(content, nullptr);
}

TEST_F(APITest, file_get_408)
{
    map<string, string> header;
    char *content = nullptr;
    size_t size = 0;
    
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 408");

    ASSERT_EQ(testAPI->file_get("test123", &header, &size, &content), 408);

    TEST_HEADER_EXIST(header, "date");
    
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");
    
    EXPECT_EQ(size, 0);
    EXPECT_STREQ(content, nullptr);
}

TEST_F(APITest, file_post_201)
{
    map<string, string> header;
    string content("test content");
    
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 201");

    ASSERT_EQ(testAPI->file_post("test123", &header, content.c_str()), 201);

    TEST_HEADER_EXIST(header, "date");
    TEST_HEADER_EXIST(header, "etag");
    
    TEST_HEADER_VALUE_EQ(header, "allow", "GET");
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");
}

TEST_F(APITest, file_post_205)
{
    map<string, string> header;
    string content("test content");
    
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 205");

    ASSERT_EQ(testAPI->file_post("test123", &header, content.c_str()), 205);

    TEST_HEADER_EXIST(header, "date");
    TEST_HEADER_EXIST(header, "etag");
    
    TEST_HEADER_VALUE_EQ(header, "allow", "GET");
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");
}

TEST_F(APITest, file_post_401)
{
    map<string, string> header;
    string content("test content");
    
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 401");

    ASSERT_EQ(testAPI->file_post("test123", &header, content.c_str()), 401);

    TEST_HEADER_EXIST(header, "date");    
        
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");
}

TEST_F(APITest, file_post_404)
{
    map<string, string> header;
    string content("test content");
    
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 404");

    ASSERT_EQ(testAPI->file_post("test123", &header, content.c_str()), 404);

    TEST_HEADER_EXIST(header, "date");    
        
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");
}

TEST_F(APITest, file_post_405)
{
    map<string, string> header;
    string content("test content");
    
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 405");

    ASSERT_EQ(testAPI->file_post("test123", &header, content.c_str()), 405);

    TEST_HEADER_EXIST(header, "date");    
        
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");
}

TEST_F(APITest, file_post_408)
{
    map<string, string> header;
    string content("test content");
    
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 408");

    ASSERT_EQ(testAPI->file_post("test123", &header, content.c_str()), 408);

    TEST_HEADER_EXIST(header, "date");    
        
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");
}

TEST_F(APITest, file_post_409)
{
    map<string, string> header;
    string content("test content");
    
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 409");

    ASSERT_EQ(testAPI->file_post("test123", &header, content.c_str()), 409);

    TEST_HEADER_EXIST(header, "date");    
        
    TEST_HEADER_VALUE_EQ(header, "expires", "Fri, 7 May 2021 13:12:47 GMT");
}

TEST_F(APITest, file_delete_204)
{
    map<string, string> header;
    
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 204");

    ASSERT_EQ(testAPI->file_delete(&header), 204);

    TEST_HEADER_EXIST(header, "date");        
}

TEST_F(APITest, file_delete_401)
{
    map<string, string> header;
    
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 401");

    ASSERT_EQ(testAPI->file_delete(&header), 401);

    TEST_HEADER_EXIST(header, "date");        
}

TEST_F(APITest, file_delete_404)
{
    map<string, string> header;
    
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 404");

    ASSERT_EQ(testAPI->file_delete(&header), 404);

    TEST_HEADER_EXIST(header, "date");        
}

TEST_F(APITest, file_delete_408)
{
    map<string, string> header;
    
    testAPI->request_header = curl_slist_append(testAPI->request_header, "x-mock-response-code: 408");

    ASSERT_EQ(testAPI->file_delete(&header), 408);

    TEST_HEADER_EXIST(header, "date");        
}