var RF24Addon = require('./build/Release/RF24Addon');

var radio = new RF24Addon.RF24Wrapper();

//radio.setChannel(10);
console.log("Channel: " + radio.getChannel());

//radio.setGpioCEPin(25);
console.log("GPIO CE Pin: " + radio.getGpioCEPin());

//radio.setGpioIRQPin(0);
console.log("GPIO IRQ Pin: " + radio.getGpioIRQPin());

//radio.setPayloadSize(10);
console.log("Payload Size: " + radio.getPayloadSize());

//radio.setLoopDelay(1000);
console.log("Loop Delay: " + radio.getLoopDelay());

//radio.setRetriesDelay(25);
console.log("Retries Delay: " + radio.getRetriesDelay());

//radio.setRetriesCount(20);
console.log("Retries Count: " + radio.getRetriesCount());

//radio.setAutoAck(0);
console.log("Auto Ack: " + radio.getAutoAck());

//radio.setDataRate(2);
console.log("DataRate: " + radio.getDataRate());

radio.setReadPipes([0x5250493031]);
console.log("Read pipes: " + radio.getReadPipes());
radio.setWritePipes([0x4152443031]);
console.log("Write pipes: " + radio.getWritePipes());

radio.initRadio();
radio.loop(function(msg) {
	console.log("Stringa di ricezione: " + msg);
	radio.tx("asdfasdf", function(status) {
		console.log("TX Status: " + status);
	});
});