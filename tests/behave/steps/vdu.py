import os
import subprocess
import sqlite3
from time import sleep
from behave import given, when, then

# Givens
@given("open URL \"{url}\"")
def step_impl(context, url):
    subprocess.run(["/usr/local/bin/vdu-app", "--url-handler", url], capture_output=True)

@given("append text to downloaded file")
def step_impl(context):
     with open("/mnt/vdu/Test Filename", "a") as file:
        file.write("\nappended text\n.")

@given("rename downloaded file")
def step_impl(context):
    os.rename("/mnt/vdu/Test Filename", "/mnt/vdu/Test")

@given("delete downloaded file")
def step_impl(context,):
    os.remove("/mnt/vdu/Test") 

# Thens
@then("file is downloaded to FUSE file system")
def step_impl(context):
    sleep(1)
    assert os.path.isfile("/mnt/vdu/Test Filename") 
    
    with open("/mnt/vdu/Test Filename") as file:
        data = file.read()

    assert data == "conent of requested file"

@then("metadata are stored in database")
def step_impl(context):
    sleep(1)
    cursor = context.cursor
    
    cursor.execute("SELECT * FROM Files WHERE Location = 'Test Filename'")
        
    data = cursor.fetchone()
    
    assert data[0] == "Test Filename"
    assert data[1] == "GET POST"
    assert data[2] == "text/html; charset=utf-8"
    assert data[3] == "Fri, 3 Sep 2021 13:12:47 GMT"
    assert data[5] == "abc123456789"
                       
@then("content is change in filesystem")
def step_impl(context):
    sleep(1)
    assert os.path.isfile("/mnt/vdu/Test Filename") 
    
    with open("/mnt/vdu/Test Filename") as file:
        data = file.read()

    assert data == "conent of requested file\nappended text\n."

@then("name of file is change in filesystem")
def step_impl(context):
    sleep(1)
    assert os.path.isfile("/mnt/vdu/Test")

@then("metadata are updated in database")
def step_impl(context):
    sleep(1)
    cursor = context.cursor
    
    cursor.execute("SELECT * FROM Files WHERE Location = 'Test'")
        
    data = cursor.fetchone()
    print(data)
    assert data[0] == "Test"
    assert data[1] == "GET POST"
    assert data[2] == "text/html; charset=utf-8"
    assert data[3] == "Fri, 3 Sep 2021 13:12:47 GMT"
    assert data[5] == "abc123456789"

@then("file is no longer in filesystem")
def step_impl(context):
    sleep(1)
    assert not os.path.isfile("/mnt/vdu/Test")

@then("metadata are deleted in database")
def step_impl(context):
    sleep(1)
    cursor = context.cursor
    
    cursor.execute("SELECT * FROM Files WHERE Location = 'Test'")
        
    data = cursor.fetchone()
    
    assert data == None  
