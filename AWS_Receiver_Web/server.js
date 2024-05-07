// "use strict";

const express = require('express');
const cors = require('cors');
var awsIot = require('aws-iot-device-sdk');
var msgs = []
const socketio = require('socket.io')


var device = awsIot.device({
    keyPath: "C:/Users/acasi/OneDrive/Documents/ITESO/Octavo Semestre/IOT/IoT_hearing_impairment/bee5fbf52-private.pem.key",
    certPath: "C:/Users/acasi/OneDrive/Documents/ITESO/Octavo Semestre/IOT/IoT_hearing_impairment/bee5fbf52-certificate.pem.crt",
    caPath: 'C:/Users/acasi/OneDrive/Documents/ITESO/Octavo Semestre/IOT/IoT_hearing_impairment/AmazonRootCA1.pem',
    clientId: 'esp32_test',
    host: 'a3fqgfoikj274h-ats.iot.us-east-2.amazonaws.com'
});

device
    .on('connect', function () {
        console.log('connected!');
        device.subscribe('sensor');
    });

const app = express();
const port = 2000;
const router = express.Router();
app.use(cors());
app.use(router);



const server = app.listen(port, () => {
    console.log('app is running in port: ' + port);
});

const io = socketio(server, {
    cors: {
        origin: '*',
        methods: ['GET', 'POST']
    }
});

router.get('/', (req, res) => {
    res.sendFile(__dirname + '/index.html');
});

io.on('connection', (socket) => {
    console.log('Someone online!' + new Date().getTime());
    //io.emit('msgs', JSON.stringify(msgs));
});
// Set handler for the device, it will get the messages from subscribers topics.
device
    .on('message', (topic, payload) => {
        message = Buffer.from(payload).toString();
        console.log('message', topic, message);
        msgs.push(message);
        io.emit('msgs', JSON.stringify(message));
    });

device
    .on('error', function (topic, payload) {
        console.log('Error:', topic, payload);
    });


router.get('/msgs', (req, res) => {
    res.send(JSON.stringify(msgs));
});