⚡ IoT-Based Power Monitoring System using ESP32 & AWS

This project demonstrates a real-time power monitoring system built using an ESP32 microcontroller, capable of measuring voltage and current across a connected load and visualizing the data on the AWS Cloud. The system leverages AWS IoT Core for secure MQTT communication and AWS CloudWatch for live data visualization through dynamic line graphs.

🧠 Overview

The system continuously measures:

Voltage using a voltage divider circuit

Current using a Hall-effect sensor (ACS712 / similar)

The ESP32 reads these analog signals, calculates instantaneous power, and publishes the readings to AWS IoT Core over MQTT. The data is then visualized on AWS CloudWatch, enabling remote real-time monitoring and energy usage analysis.

⚙️ Features

Real-time measurement of voltage, current, and power

Wi-Fi enabled IoT connectivity using ESP32

Secure data publishing via AWS IoT Core (MQTT)

Live visualization and analytics using AWS CloudWatch

Scalable for smart energy management applications

🧩 Hardware Components
Component	Description
ESP32	Main microcontroller for data acquisition and transmission
Hall-effect sensor	Measures load current (e.g., ACS712)
Voltage divider	Steps down AC/DC voltage for safe analog input
Resistors	For voltage scaling and calibration
Power supply	5V regulated supply for ESP32 and sensors
🛠️ Software & Cloud Services
Software / Service	Purpose
Arduino IDE	Programming and uploading ESP32 code
AWS IoT Core	Device authentication and MQTT communication
AWS CloudWatch	Real-time data visualization and monitoring
Python / Excel (optional)	For offline data analysis

🚀 Getting Started
1. Hardware Setup

Connect the Hall sensor to an analog input pin of ESP32.

Connect the voltage divider output to another analog input.

Power the ESP32 using USB or a 5V regulated source.

2. AWS Setup

Create an AWS IoT Thing, attach a policy, and download certificates.

Configure AWS IoT Core and link it to CloudWatch for data visualization.

3. Code Configuration

Open the .ino file in Arduino IDE.

Update Wi-Fi credentials and AWS IoT endpoint details.

Upload the code to the ESP32.

4. Run the System

Once powered, ESP32 connects to Wi-Fi and starts publishing data.

View real-time voltage, current, and power data on AWS CloudWatch dashboards.

📊 Output Example

Real-time line graphs of voltage, current, and power

Cloud-stored data for trend analysis and efficiency monitoring

🧾 Future Improvements

Add overload and fault detection

Integrate with AWS DynamoDB for historical data storage

Develop a mobile/web dashboard using AWS Amplify

🧑‍💻 Author

Ayan Sen

B.Tech Electrical and Electronics Engineering | VIT Vellore
