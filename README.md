# Temperature And Humidity Sensor
I built this project because I wanted to try out embedded programming at the beginning of 2026. Once I had a working circuit, I combined it with my current knowledge of AWS to create an IoT data pipeline.

I utilized a temperature and humidity sensor to collect data from my room using an ESP32 as the microcontroller. The ESP32 has built-in WiFi and Bluetooth capabilities, which allowed me to send the collected data to the Cloud via the internet using the MQTT protocol.

I registered my device as a thing on IoT Core, allowing AWS to receive data I transmit. That data is then stored in an S3 Bucket acting as a data lake via Data Fire Hose.

Now that the data is stored, I can query it from Power BI. I downloaded the ODBC driver from AWS, which allows me to query information from the S3 Bucket using Athena. I extracted the exact data I wanted using a custom SQL query and plotted the results for presentation in Power BI.

Overall, this was a great project. I learned about embedded systems and got a taste of programming devices for the Internet of Things. I'm glad that I applied more of my AWS knowledge.

Below are the plotted results of my readings and the architecture diagram of the services used.
