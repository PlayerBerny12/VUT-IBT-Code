import sqlite3
from pathlib import Path

def before_all(context):
    connection = sqlite3.connect(str(Path.home()) + "/.local/share/vdu/vdu.db")
    cursor = connection.cursor()
    
    context.connection = connection
    context.cursor = cursor

def after_all(context):
    connection = context.connection
    connection.close()