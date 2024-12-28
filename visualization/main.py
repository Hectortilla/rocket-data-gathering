import serial
import re
from vpython import *

f = box(pos=vector(0, 0, 1), up=vector(0, 1, 0), size=vector(0.9, 0.5, 1.9), color=color.green)


pattern = r'\x1b\[0;32mI \(\d+\) IMU: roll:([-\d.]+) pitch:([-\d.]+) yaw:([-\d.]+)\x1b\[0m'

last_roll = 0
last_pitch = 0
last_yaw = 0

with serial.Serial('/dev/ttyUSB0', 115200) as ser:
    while True:
        line = ser.readline().decode("utf-8")

        match = re.search(pattern, line)

        if match:
            roll = float(match.group(1))
            pitch = float(match.group(2))
            yaw = float(match.group(3))

            f.rotate(angle=radians(roll - last_roll), axis=vector(1, 0, 0))
            f.rotate(angle=radians(pitch - last_pitch), axis=vector(0, 0, 1))
            f.rotate(angle=radians((yaw - last_yaw) * -1), axis=vector(0, 1, 0))

            last_roll = roll
            last_pitch = pitch
            last_yaw = yaw

            print(f"Roll: {last_roll}")
            print(f"Pitch: {last_pitch}")
            print(f"Yaw: {last_yaw}")

        else:
            pass
            # print("No match found")
