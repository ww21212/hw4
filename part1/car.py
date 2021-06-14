import time
import serial
import sys,tty,termios

# XBee setting
serdev = '/dev/ttyUSB0'
s = serial.Serial(serdev, 9600, timeout=3)

"""
s.write("+++".encode())
char = s.read(2)
print("Enter AT mode.")
print(char.decode())
s.write("ATMY <0x135>\r\n".encode())
char = s.read(3)
print("Set MY <0x135>.")
print(char.decode())
s.write("ATDL <0x235>\r\n".encode())
char = s.read(3)
print("Set DL <0x235>.")
print(char.decode())
s.write("ATID <0x1>\r\n".encode())
char = s.read(3)
print("Set PAN ID <0x1>.")
print(char.decode())
s.write("ATWR\r\n".encode())
char = s.read(3)
print("Write config.")
print(char.decode())
s.write("ATMY\r\n".encode())
char = s.read(4)
print("MY :")
print(char.decode())
s.write("ATDL\r\n".encode())
char = s.read(4)
print("DL : ")
print(char.decode())
s.write("ATCN\r\n".encode())
char = s.read(3)
print("Exit AT mode.")
print(char.decode())
"""

print('Start parking')
s.write("/parking/run 5.0 3.0 west\n".encode())

s.close()