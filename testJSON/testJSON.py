import time
import json
import serial

ser = serial.Serial('/dev/cu.usbmodem1411', 9600)
print(ser.name)

# Read the "Hello World" line
starter_line = ser.readline()
print(starter_line)

# Read the JSON data sent from Arduino
json_line = ser.readline()
print(json_line)

json_data = json.loads(json_line)
print(json_data)

# Write JSON data to the Arduino
test_dict = {'p1': 19, 'v1': 4, 'action': 'update'}
test_dict_serialized = json.dumps(test_dict)
ser.write(test_dict_serialized.encode('utf-8'))

# Read back the test result line
test_line = ser.readline()
print(test_line)

test_line = ser.readline()
print(test_line)

test_line = ser.readline()
print(test_line)

ser.close()
