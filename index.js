// Set up serial comms
const SerialPort = require('serialport');
const Readline   = require('@serialport/parser-readline');

// Connect to the particular arduino port
const port   = new SerialPort('/dev/cu.usbmodem1411', { baudRate: 9600 });
const parser = port.pipe(new Readline({ delimiter: '\n' }));

function write() //for writing
{
    port.on('data', function (data) 
    {
        port.write("Write your data here");
    });
}

function read () // for reading
{
    port.on('data', function(data)
    {
        console.log(data); 
    });
}

// Read the port data
port.on("open", () => {
  console.log('serial port open');

  write();
});
// parser.on('data', data =>{
//     console.log('got word from arduino:', data);
//   });

// // write port data to serial
// port.write('hello from node\n', (err) => {
//     if (err) {
//       return console.log('Error on write: ', err.message);
//     }
//     console.log('message written');
//   });



