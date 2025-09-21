async function fetchData(str) { 
    const response = await fetch(str);
    const data = await response.json();
    return data;
}

async function updateInfo() {
    data = await fetchData(`http://192.168.15.71:5000/device/2/last`);
    temperature = data[0].temperature.value;
    document.getElementById("lastTemperature").innerHTML = `${temperature} °C`;

    humidity = data[1].humidity.value;
    document.getElementById("lastHumidity").innerHTML = `${humidity} %`;

    pressure = data[2].pressure.value;
    document.getElementById("lastPressure").innerHTML = `${pressure} Pa`;

    battery = data[3].battery.value;
    document.getElementById("lastBattery").innerHTML = `${battery/1000} V`;

    lastTime = data[0].temperature.timestamp;
    document.getElementById("lastTimeUpdated").innerHTML = `${lastTime}`;
}
  
document.getElementById("updateBtn").addEventListener("click",updateInfo);


function filterData(interval, data) {
    const now = new Date();
    let start;

    switch (interval) {
    case '30m':
        start = new Date(now.getTime() - 1 * 30 * 60 * 1000);
        break;
    case '1h':
        start = new Date(now.getTime() - 1 * 60 * 60 * 1000);
        break;
    case '6h':
        start = new Date(now.getTime() - 6 * 60 * 60 * 1000);
        break;
    case '1d':
        start = new Date(now.getTime() - 24 * 60 * 60 * 1000);
        break;
    case '3d':
        start = new Date(now.getTime() - 3 * 24 * 60 * 60 * 1000);
        console.log("Start", start);
        break;
    default:
        start = new Date(0); // sem filtro
    }

    return data.filter(p => p.x >= start);
}

function parseData(data) {
    data = data.reverse()
    const labels = data.map(item => new Date(item.timestamp));
    const temperature = data.map(item => item.sensortemperature);

    const humidity = data.map(item => item.sensorhumidity);
    const pressure = data.map(item => item.sensorpressure);

    const voltage = data.map(item => item.voltagebattery/1000)
    return { labels, temperature, humidity, pressure, voltage};
}

async function createChart(chartId, deviceNum, interval, sensorChoice) {
    const dataaht20 = await fetchData(`http://192.168.15.71:5000/device/${deviceNum}/aht20`);
    const databmp280 = await fetchData(`http://192.168.15.71:5000/device/${deviceNum}/bmp280`);
    const databattery = await fetchData(`http://192.168.15.71:5000/device/${deviceNum}/battery`);
    const { labels: labels_aht20, temperature: sensorTemperature_aht20, humidity: sensorHumidity_aht20, pressure: sensorPressure_aht20 } = parseData(dataaht20);
    const { labels: labels_bmp280, temperature: sensorTemperature_bmp280, humidity: sensorHumidity_bmp280, pressure: sensorPressure_bmp280 } = parseData(databmp280);
    const { labels: labels_voltage, t, h, p ,voltage} = parseData(databattery);
    const ctx = document.getElementById(chartId).getContext('2d');

    dataTempAht20 = labels_aht20.map((ts, i) => ({ x: ts, y: sensorTemperature_aht20[i] }));
    dataHumAht20 = labels_aht20.map((ts, i) => ({ x: ts, y: sensorHumidity_aht20[i] }));
    
    dataTempBmp280 = labels_bmp280.map((ts, i) => ({ x: ts, y: sensorTemperature_bmp280[i] }));
    dataPresBmp280 = labels_bmp280.map((ts, i) => ({ x: ts, y: sensorPressure_bmp280[i] }));

    dataBattery = labels_voltage.map((ts,i) => ({x: ts, y: voltage[i]}));

    if (charts[chartId]) {
        charts[chartId].destroy();
    }

    let label, unit, text, data;
    let dataset = [];
    if(sensorChoice == "battery"){
        label = "Battery Voltage";
        unit = "mV";
        data = filterData(interval, dataBattery);
        text = "Voltage (mV)";
    }
    if(sensorChoice == "humidity"){
        label = "Humidity";
        unit = "%";
        data = filterData(interval, dataHumAht20);
        text = "Humidity (%)";
    }
    if(sensorChoice == "pressure"){
        label = "Pressure";
        unit = "Pa";
        data = filterData(interval, dataPresBmp280);
        text = "Pressure (Pa)";
    }
    if(sensorChoice == "temperature") {
        label = "Temperature AHT20";
        unit = "°C";
        data = filterData(interval, dataTempAht20);
        dataset.push({label:label, data:data, borderColor: 'red',backgroundColor: 'red', fill:false, tension:0.1});
        
        label = "Temperature BMP280";
        data = filterData(interval, dataTempBmp280);
        text = "Temperature  (°C)";
    }
    dataset.push({label:label, data:data, borderColor: 'blue',backgroundColor: 'blue', fill:false, tension:0.1});

    charts[chartId] = new Chart(ctx, {
        type: 'line',
        data: {
            datasets: dataset,
        },
        options: {
            parsing: true, // usa dados {x,y} direto
            scales: {
            x: {
                type: 'time',     // eixo tempo
                time: {
                unit: 'minute'  // unidade no eixo X
                },
                title: {
                display: true,
                text: 'Time'
                },
            },
            y: {
                title: {
                display: true,
                text: text,
                },
            }
            }
        }
    });
}

const charts = {}
let interval = '1h';
let sensorChoice2 = 'temperature';

createChart('chartDevice2', 2, interval, sensorChoice2);

document.getElementById('interval').addEventListener('change', function () {
    interval = this.value;
    createChart('chartDevice2', 2, this.value, sensorChoice2);
});
document.getElementById('sensorChoice2').addEventListener('change', function () {
    sensorChoice2 = this.value;
    createChart('chartDevice2', 2, interval, this.value);
});