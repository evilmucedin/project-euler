#!/usr/bin/env python3

import serial

arduino = serial.Serial(port='/dev/ttyACM0', baudrate=9600, timeout=10.0)
with open("arduino.csv", "w") as fOut:
    data = arduino.readline().decode()
    data = arduino.readline().decode()
    while data:
        parts = data.split(",")
        if len(parts) == 7:
            fOut.write(data)
        data = arduino.readline().decode()
