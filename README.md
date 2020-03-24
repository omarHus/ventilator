# ventilator

## Arduino

Setup
- Find the name of the port used by your arduino by going to the arduino IDE > tools > port.
- Upload the code to your arduino

## Node

To run the node file on your terminal:
- make sure you have npm
- add module serialport: npm install serialport
- open index.js and make sure the correct arduino port is specified (we should write some code to make sure this is done automatically)
- then type this command in the terminal: node index.js