433_sensor_BME280

Wireless 433Mhz weather sensor using BME280 and Attiny85

Files:

Attiny_BME280_en.ino  - source file for Arduino
Weather_sensor.jpg  -  schematic file
README.md  -  this file

I wanted a weather sensor for my smart home system. There is no possibility and desire to drag the wires, so I decided to make a radio sensor. The details were close at hand and it took 3 hours to complete. The sensor consumes 0.02 mA in sleep mode. According to calculations, the battery will last more than a year. I did the coding of the data sending as convenient for me, since the system has many sensors and buttons with the rcswitch 433 MHz protocol. If desired, you can remake it for yourself. The receiver is connected to the ESP8266 and the codes are transmitted via MQTT to the Orange Pi with the Openhab server. I decipher the codes in the rules.
