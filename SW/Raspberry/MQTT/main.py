import paho.mqtt.client as paho
import mysql.connector as my_sql
import json
import time

from dotenv import load_dotenv
import os

load_dotenv()

try:
    mysql = my_sql.connect(user=os.getenv('DB_USER'), password=os.getenv('DB_PASS'),
                            host='localhost',
                            database=os.getenv('DB_NAME'))
    print(f'Connection success!')
except mysql.Error as err:
    if err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
        print("Something is wrong with your user name or password")
    elif err.errno == errorcode.ER_BAD_DB_ERROR:
        print("Database does not exist")
    else:
        print(err)

print(mysql)
cursor = mysql.cursor()


def on_connect(client, userdata, flags, rc):
    print('CONNACK received with code %d.' % (rc))

def on_message(client, userdata, msg):
    print(str(msg.timestamp)+" "+msg.topic+" "+str(msg.qos)+" "+str(msg.payload))
    data = json.loads(msg.payload.decode('utf-8'))
    print(data)
    deviceNum = data.get("device")
    sensor = data.get("sensor")
    temperature = data.get("t", None)
    humidity = data.get("h", None)
    pressure = data.get("p", None)

    add_measure = ("INSERT INTO measures "
                   "(device, sensorname, sensortemperature, sensorhumidity, sensorpressure) "
                   "VALUES (%s, %s, %s, %s, %s)")
    data_measure = (deviceNum, sensor, temperature, humidity, pressure)

    cursor.execute(add_measure, data_measure)
    mysql.commit()
    print("Measure inserted successfully!")

    client.publish('OK/', 'ok!')


client = paho.Client()
client.on_message = on_message
client.on_connect = on_connect
client.connect('localhost', 1883)
client.subscribe('measure/')
client.loop_start()

while True:
    time.sleep(0.1)
