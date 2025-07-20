import mysql.connector as my_sql
import os
from dotenv import load_dotenv

dotenv_path = os.path.join(os.path.dirname(os.path.dirname(__file__)), '.env')
load_dotenv(dotenv_path)

def get_connection():
    try:
        mysql = my_sql.connect(user=os.getenv('DB_USER'), password=os.getenv('DB_PASS'),
                                host='localhost',
                                database=os.getenv('DB_NAME'))
        print(f'Connection success!')
        if mysql.is_connected():
            return mysql
    
    except mysql.Error as err:
        if err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
            print("Something is wrong with your user name or password")
        elif err.errno == errorcode.ER_BAD_DB_ERROR:
            print("Database does not exist")
        else:
            print(err)
