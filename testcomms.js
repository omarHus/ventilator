// Set up serial comms
const SerialPort = require('serialport');
const Readline   = require('@serialport/parser-readline');

// Connect to the particular arduino port
const port   = new SerialPort('/dev/cu.usbmodem1411', { baudRate: 9600 });
const parser = port.pipe(new Readline({ delimiter: '\n' }));

// Read the port data
port.on("open", () => {
  console.log('serial port open');
});

// write port data to serial
port.write('hello from node\n', (err) => {
    if (err) {
      return console.log('Error on write: ', err.message);
    }
    console.log('message written');
  });

parser.on('data', data =>{
  console.log('got word from arduino:', data);
});

