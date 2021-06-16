import time
import serial
import sys,tty,termios

# XBee setting
serdev = '/dev/ttyUSB0'
s = serial.Serial(serdev, 9600, timeout=3)

print('Start parking')
# Setting d1 and d2
s.write("/parking/run 5.0 3.0 west\n".encode())

s.close()