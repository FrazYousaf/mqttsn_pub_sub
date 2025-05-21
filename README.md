# MQTT-SN Gateway and Client Communication Setup  
*By Fraz Yousaf*

This project demonstrates how to configure and test an MQTT-SN (MQTT for Sensor Networks) setup using Eclipse Paho's `mqtt-sn.embedded-c` implementation. It includes running a local MQTT broker, MQTT-SN Gateway, and simple publisher/subscriber clients with **support for dynamic topics and multiple clients**.

---

## 🆕 Changelog (v1.1)
- **Dynamic Topics**: Topics are no longer hardcoded (`tt`) - use any name (e.g., `t1`, `sensor/data`) at runtime.
- **Multiple Subscribers**: Multiple clients can subscribe to the same topic and receive messages simultaneously.
- **Multiple Publishers**: Multiple publishers can send messages to the same/different topics.
- **Client ID Handling**: Unique client IDs prevent conflicts between subscribers.
- **Bug Fixes**: Resolved `PINGREQ` errors and broker disconnections.

---

## 🔧 Environment

- **Operating System:** Ubuntu (WSL or native)
- **MQTT Broker:** Mosquitto
- **MQTT-SN Gateway:** From `paho.mqtt-sn.embedded-c/MQTTSNGateway`
- **MQTT-SN Clients:** `qos0pub`, `pub0sub1` (from `MQTTSNPacket/samples`)

---

## 📦 Setup Instructions

### 1. Install Dependencies
```bash
sudo apt update
sudo apt install git cmake build-essential mosquitto
2. Clone the Project (if not already cloned)
bash
git clone https://github.com/frazyousaf/mqtt-sn-demo.git
cd mqtt-sn-demo
3. Build the Project
bash
mkdir build
cd build
cmake ..
make
🚀 Running the MQTT-SN Setup
Run these in separate terminal tabs:

Step 1: Start the MQTT Broker (Mosquitto)
bash
mosquitto -v
Step 2: Start the MQTT-SN Gateway
bash
cd ~/paho.mqtt-sn.embedded-c/MQTTSNGateway/bin
./MQTT-SNGateway -f gateway.conf
🖥️ Client Examples
Case 1: Single Publisher, Multiple Subscribers
Publisher (send to topic t1):

bash
./qos0pub 127.0.0.1 10000 t1 "Hello from Publisher 1"
Subscriber 1 (Client ID: sub1):

bash
./pub0sub1 127.0.0.1 10000 t1 sub1
Subscriber 2 (Client ID: sub2):

bash
./pub0sub1 127.0.0.1 10000 t1 sub2
Case 2: Multiple Publishers, Single Subscriber
Publisher 1 (topic p1):

bash
./qos0pub 127.0.0.1 10000 p1 "Temp: 25°C"
Publisher 2 (topic p2):

bash
./qos0pub 127.0.0.1 10000 p2 "Humidity: 60%"
Subscriber (listen to both topics):

bash
# Terminal 1 (Topic: p1)
./pub0sub1 127.0.0.1 10000 p1 sub_temp

# Terminal 2 (Topic: p2)
./pub0sub1 127.0.0.1 10000 p2 sub_humidity
📌 Key Features
Dynamic Topics: Use any topic name (e.g., t1, p1).

Multi-Subscriber: All subscribers receive messages on the same topic.

Multi-Publisher: Multiple publishers can send to same/different topics.

Unique Client IDs: Required for subscribers (e.g., sub1, sub2).

📁 Project Structure (Key Files/Folders)
paho.mqtt-sn.embedded-c/
├── build/                      # Contains clients (qos0pub, pub0sub1)
├── MQTTSNGateway/bin/          # Gateway binary
├── MQTTSNGateway/gateway.conf  # Gateway config
└── README.md
👨💻 Author
Fraz Yousaf
PhD Student, University of Parma
Email: fraz.yousaf@unipr.it
GitHub: frazyousaf

Contributions and bug reports welcome! 🛠️