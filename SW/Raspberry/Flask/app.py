from flask import Flask, jsonify, render_template
from db import get_connection

app = Flask(__name__)

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