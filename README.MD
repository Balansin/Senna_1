Ayrton Senna was the greatest brazillian racer. Started kart racing in 1973 to become F-1 world champion in 1990. Senna started small to great accomplishments, and that's what this project is about: great accomplishment throught purpose, experimantation and praxis.

## Project Components

### Hardware
- **Raspberry Pi (3 or newer)**: Runs the Raspbian operating system, hosts the MQTT broker, logs data to a CSV file, and renders the visualization.
- **ESP32-WROOM DEVKIT V1**: Collects accelerometer data from the ADXL355 sensor and publishes it to the MQTT broker.
- **ADXL355 Accelerometer**: Provides high-precision 3-axis acceleration data (X, Y, Z axes).

### Software
- **Arduino IDE or PlatformIO**: Used for programming the ESP32 to read accelerometer data and publish it via MQTT.
- **Paho MQTT Client (Python)**: Handles MQTT communication on the Raspberry Pi to subscribe to accelerometer data and log it to a CSV file.
- **SDL2 Library (C++)**: Renders real-time plots of the accelerometer data on the Raspberry Pi.
- **Python 3**: Runs the MQTT data logging script.
- **Raspbian OS**: Operating system for the Raspberry Pi.

## System Architecture
1. **Data Acquisition**:
   - The ESP32 reads acceleration data (X, Y, Z) from the ADXL355 accelerometer.
   - The ESP32 publishes this data as JSON messages to the MQTT topic `esp32/adxl355` on the MQTT broker running at `192.168.100.45:1883`.

2. **Data Logging**:
   - A Python script (`salvar_mqtt_csv_grafico.py`) on the Raspberry Pi subscribes to the `esp32/adxl355` topic.
   - The script decodes incoming JSON messages, extracts the X, Y, and Z values, and logs them to a CSV file (`dados_mqtt.csv`) along with a timestamp.
   - The CSV file is stored in `/home/safena/Senna_1/Log/` and includes a header row: `tempo_s, x, y, z`.

3. **Data Visualization**:
   - A C++ program (`sdl_plot.cpp`) reads the CSV file and visualizes the most recent 100 data points as three line plots (red for X, green for Y, blue for Z).
   - The visualization uses SDL2 to render a window of 800x600 pixels, with a grid and axes for reference.
   - The Y-axis is scaled to display values between -2.0 and 2.0, mapped to the window height.
   - The plot updates every 100ms to reflect new data.

## File Structure
- **sdl_plot.cpp**: C++ program for real-time visualization of accelerometer data using SDL2.
- **salvar_mqtt_csv_grafico.py**: Python script for subscribing to MQTT messages and logging data to a CSV file.
- **dados_mqtt.csv**: Output CSV file storing timestamped accelerometer data.
- **README.MD**: Brief project overview and prerequisites.
- **LICENSE**: Placeholder for licensing information (currently empty).
- **Use**: Placeholder for usage instructions (currently empty).

## Setup Instructions

### Prerequisites
- **Hardware**:
  - Raspberry Pi (3 or newer) with Raspbian installed.
  - ESP32-WROOM DEVKIT V1.
  - ADXL355 accelerometer connected to the ESP32.
- **Software**:
  - Install Arduino IDE or PlatformIO for ESP32 development.
  - Install Python 3 and the `paho-mqtt` library on the Raspberry Pi (`pip install paho-mqtt`).
  - Install SDL2 development libraries on the Raspberry Pi (`sudo apt-get install libsdl2-dev`).
  - Set up an MQTT broker (e.g., Mosquitto) on the Raspberry Pi or another device at `192.168.100.45:1883`.

### Installation
1. **ESP32 Setup**:
   - Program the ESP32 using Arduino IDE or PlatformIO to read ADXL355 data and publish it to the MQTT topic `esp32/adxl355`.
   - Ensure the ESP32 is connected to the same network as the MQTT broker.

2. **Raspberry Pi Setup**:
   - Clone or copy the project files to `/home/safena/Senna_1/`.
   - Compile the SDL2 visualization program:
     ```bash
     g++ sdl_plot.cpp -o sdl_plot -lSDL2
     ```
   - Ensure the MQTT broker is running and accessible at `192.168.100.45:1883`.

3. **Running the System**:
   - Start the MQTT logging script:
     ```bash
     python3 salvar_mqtt_csv_grafico.py
     ```
   - Run the visualization program:
     ```bash
     ./sdl_plot
     ```
   - The Python script will log incoming MQTT messages to `dados_mqtt.csv`, and the SDL2 program will display a real-time plot of the data.

## Usage
- **Data Logging**:
  - The Python script continuously listens for MQTT messages and appends data to `dados_mqtt.csv`.
  - The CSV file can be analyzed later or used by other applications.
- **Visualization**:
  - The SDL2 program displays a window with three colored lines representing the X, Y, and Z acceleration values.
  - The plot shows the most recent 100 data points, with the X-axis representing time and the Y-axis representing acceleration values (-2.0 to 2.0).
  - Close the SDL2 window to stop the visualization.

## Limitations
- The visualization is limited to the most recent 100 data points to maintain performance.
- The system assumes a stable network connection between the ESP32, MQTT broker, and Raspberry Pi.
- The CSV file path is hardcoded (`/home/safena/Senna_1/Log/dados_mqtt.csv`).
- No error handling for missing or corrupted CSV files.