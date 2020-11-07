from time import sleep
import serial
ser = serial.Serial('/dev/ttyS3')
print(ser.name)
sleep(10)
print('done sleeping lets send out the stuff')

for count in range(9,40):
    cmd = b'\x91' + bytes(count) + b'\x00'
    ser.write(cmd)
    sleep(0.5)

ser.close()