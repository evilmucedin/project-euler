#!/usr/bin/env python3

import serial

def safeDecode(bts):
    try:
        return bts.decode()
    except Exception:
        return "!decode error"

arduino = serial.Serial(port='/dev/ttyACM0', baudrate=9600, timeout=10.0)
with open("arduino.csv", "w") as fOut:
    print("Timestamp,AxX,AxY,AxZ,GX,GY,GZ", file=fOut)
    data = arduino.readline()
    data = safeDecode(arduino.readline())
    while data:
        parts = data.split(",")
        if len(parts) == 7:
            fOut.write(data)
        data = safeDecode(arduino.readline())
