# 🖥️ Software - Dashboard and Backend

This folder contains the software layer of the project responsible for collecting, storing, and visualizing data from the embedded system. It includes a backend API (Flask or Django), a MySQL-compatible database (MariaDB), and a frontend dashboard using Chart.js.

---

## 🎯 Purpose

The software part is designed to:

- Receive sensor data from the firmware (e.g., via MQTT or HTTP)
- Store the data in a local database on a Raspberry Pi
- Expose a RESTful API to serve the data
- Display the data in an interactive dashboard using Chart.js

## 🧰 Tech Stack

| Layer       | Technology        |
|-------------|-------------------|
| Backend     | Flask or Django   |
| Database    | MariaDB (MySQL)   |
| Frontend    | HTML, CSS, JS     |
| Charts      | Chart.js          |
| Hosting     | Raspberry Pi      |

---

## 🧱 Learning Focus
- How to set up a local backend with persistent storage

- Creating REST APIs for embedded data ingestion

- Using Chart.js to visualize real-time or historical data

- Deploying and maintaining applications on a Raspberry Pi

## 📄 License
This software is released under the MIT License.

