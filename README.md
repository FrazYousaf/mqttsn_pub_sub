
# MQTT-SN Gateway and Client Communication Setup  
*By Fraz Yousaf*

This project demonstrates how to configure and test an MQTT-SN (MQTT for Sensor Networks) setup using Eclipse Paho's `mqtt-sn.embedded-c` implementation. It includes running a local MQTT broker, MQTT-SN Gateway, and simple publisher/subscriber clients.

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
```

### 2. Clone the Project (if not already cloned)

```bash
git clone https://github.com/frazyousaf/mqtt-sn-demo.git
cd mqtt-sn-demo
```

> *If you are working inside an existing cloned directory such as `~/paho.mqtt-sn.embedded-c`, skip this step.*

### 3. Build the Project

```bash
mkdir build
cd build
cmake ..
make
```

---

## 🚀 Running the MQTT-SN Setup

Run the following steps in **separate terminal tabs or windows**:

### Step 1: Start the MQTT Broker (Mosquitto)

```bash
mosquitto -v
```

> This will start the Mosquitto broker on `localhost:1883`.

### Step 2: Start the MQTT-SN Gateway

```bash
cd ~/paho.mqtt-sn.embedded-c/MQTTSNGateway/bin
./MQTT-SNGateway -f gateway.conf
```

> Make sure your `gateway.conf` file is correctly configured to connect to the Mosquitto broker.

### Step 3: Run the Publisher Client

```bash
cd ~/paho.mqtt-sn.embedded-c/build/MQTTSNPacket/samples
./qos0pub 127.0.0.1 10000 tt "Hello my name is fraz yousaf"
```

> This sends the message `"Hello my name is fraz yousaf"` to topic `tt` on port `10000`.

### Step 4: Run the Subscriber Client

```bash
./pub0sub1 127.0.0.1 10000 tt
```

> This subscribes to topic `tt` and waits to receive messages.

---

## 📁 Project Structure (Key Files/Folders)

```
paho.mqtt-sn.embedded-c/
├── build/                      # Build directory after cmake
│   └── MQTTSNPacket/samples/   # Contains publisher and subscriber clients
├── MQTTSNGateway/bin/          # Contains MQTT-SN Gateway binary
├── MQTTSNGateway/gateway.conf  # Gateway configuration file
├── README.md                   # This file
└── .gitignore
```

---

## 👨‍💻 Author

**Fraz Yousaf**  
PhD Student, University of Parma  
Email: [fraz.yousaf@unipr.it](mailto:fraz.yousaf@unipr.it)  
GitHub: [frazyousaf](https://github.com/frazyousaf)

---

*Feel free to open issues or contribute!*
