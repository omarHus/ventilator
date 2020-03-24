// Set up serial comms
const SerialPort = require('serialport');
const Readline   = require('@serialport/parser-readline');

// Connect to the particular arduino port
const port   = new SerialPort('/dev/cu.usbmodem14101', { baudRate: 9600 });
const parser = port.pipe(new Readline({ delimiter: '\n' }));

// For reading
function read()
{
    port.on('data', function(data)
    {
        console.log(data);
    });
}

// Callback on serial port open
port.on("open", () => {
  console.log('serial port open');
});

// Callback on serial port data
parser.on('data', data =>{
    console.log(data);
    port.write("TODO: Figure out input");
});
