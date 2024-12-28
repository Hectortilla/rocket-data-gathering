import serial
import re
from vpython import *

f = box(pos=vector(0, 0, 1), up=vector(0, 1, 0), size=vector(0.9, 0.5, 1.9), color=color.green)


pattern = r'\x1b\[0;32mI \(\d+\) IMU: roll:([-\d.]+) pitch:([-\d.]+) yaw:([-\d.]+)\x1b\[0m'

last_roll = 0
last_pitch = 0
last_yaw = 0

def set_orientation(obj, yaw, pitch, roll):
    cy, sy = cos(yaw), sin(yaw)
    cp, sp = cos(pitch), sin(pitch)
    cr, sr = cos(roll), sin(roll)
    
    obj.axis = vector(cy*cp, sy*cp, -sp)
    obj.up = vector(cy*sp*sr - sy*cr, sy*sp*sr + cy*cr, cp*sr)


with serial.Serial('/dev/ttyUSB0', 115200) as ser:
    while True:
        line = ser.readline().decode("utf-8")

        match = re.search(pattern, line)

        if match:
            roll = float(match.group(1))
            pitch = float(match.group(2))
            yaw = float(match.group(3))
            
            set_orientation(f, radians(pitch), radians(yaw), radians(roll))

            print(f"Roll: {roll}")
            print(f"Pitch: {pitch}")
            print(f"Yaw: {yaw}")

        else:
            pass
            # print("No match found")
