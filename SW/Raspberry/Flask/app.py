from flask import Flask, jsonify, render_template
from db import get_connection
from flask_cors import CORS

app = Flask(__name__)

CORS(app)

@app.route('/')
def home():
    return render_template('dashboard.html')

@app.route('/<name>')
def hello(name):
    return f"Hello, {name}!"

@app.route('/device/<n>/<sensorname>')
def data_device(n, sensorname):
    conn = get_connection()
    if conn is None:
        return jsonify({"error": "Erro na conexão com o banco"}), 500
    cursor = conn.cursor(dictionary=True)
    cursor.execute(f"SELECT sensorname, timestamp, sensortemperature, sensorhumidity, sensorpressure, voltagebattery FROM measures WHERE device = {n} AND sensorname = \"{sensorname}\" ORDER BY timestamp DESC LIMIT 500")
    rows = cursor.fetchall()

    for d in rows:
        d['timestamp'] = d['timestamp'].strftime("%Y-%m-%dT%H:%M:%S")

    cursor.close()
    conn.close()
    return jsonify(rows)

@app.route('/device/<n>/last')
def last_data(n):
    conn = get_connection()
    if conn is None:
        return jsonify({"error": "Error connection with db"}), 500
    cursor = conn.cursor(dictionary=True)
    cursor.execute(f"SELECT sensorname, timestamp, sensortemperature, sensorhumidity, sensorpressure, voltagebattery FROM measures WHERE device = {n} AND sensorname = \"aht20\" ORDER BY timestamp DESC LIMIT 1")
    rows = cursor.fetchall()

    # for d in rows:
    #     d['timestamp'] = d['timestamp'].strftime("%Y-%m-%dT%H:%M:%S")

    temperature = {'temperature': {'timestamp':rows[0]['timestamp'].strftime("%Y-%m-%dT%H:%M:%S"), 'value': rows[0]['sensortemperature']}}
    humidity = {'humidity': {'timestamp':rows[0]['timestamp'].strftime("%Y-%m-%dT%H:%M:%S"), 'value': rows[0]['sensorhumidity']}}

    data = []
    data.append(temperature)
    data.append(humidity)

    cursor.execute(f"SELECT sensorname, timestamp, sensortemperature, sensorhumidity, sensorpressure, voltagebattery FROM measures WHERE device = {n} AND sensorname = \"bmp280\" ORDER BY timestamp DESC LIMIT 1")
    rows = cursor.fetchall()

    pressure = {'pressure': {'timestamp':rows[0]['timestamp'].strftime("%Y-%m-%dT%H:%M:%S"), 'value': rows[0]['sensorpressure']}}
    data.append(pressure)

    cursor.execute(f"SELECT sensorname, timestamp, sensortemperature, sensorhumidity, sensorpressure, voltagebattery FROM measures WHERE device = {n} AND sensorname = \"battery\" ORDER BY timestamp DESC LIMIT 1")
    rows = cursor.fetchall()

    battery = {'battery': {'timestamp':rows[0]['timestamp'].strftime("%Y-%m-%dT%H:%M:%S"), 'value': rows[0]['voltagebattery']}}
    data.append(battery)

    print(data)
    cursor.close()
    conn.close()
    return jsonify(data)

@app.route('/data')
def get_data():
    conn = get_connection()
    if conn is None:
        return jsonify({"error": "Erro na conexão com o banco"}), 500
    cursor = conn.cursor(dictionary=True)
    cursor.execute("SELECT sensorname, timestamp, sensortemperature, sensorhumidity, sensorpressure FROM measures ORDER BY timestamp DESC")

    rows = cursor.fetchall()

    for i,measures in enumerate(rows):
        print(f'measure {i}')
        for k,v in measures.items():
            print(f'{k}: {v}')

    cursor.close()
    conn.close()
    return jsonify(rows)



if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)