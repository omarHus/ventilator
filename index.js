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

    // TODO: Move this to a button onClick, where the value is updated from UI
    port.write("OXYGEN = 20");
});

// UI --> ARDUINO (ASYNC)
// OXYGEN LEVEL: 20 - 100
// PEAK EXPIRATION PRESSURE: 0 - 30
// PEAK INSPIRATION PRESSURE: 0 - 30
// PEAK EXPIRATION <= PEAK INSPIRATION

// ARDUINO --> UI (SYNC @ 1 HZ ?)
// PRESSURE: 0 - 30
// FLOW: 0 - X
// VOLUME: 0 - X
// NOTIFICATIONS: UPDATES
