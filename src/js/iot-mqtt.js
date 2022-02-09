var clientId = "ws" + Math.random(); 
// Create a client instance
var client = new Paho.MQTT.Client("35.247.218.237", 8083, clientId);

// set callback handlers
client.onConnectionLost = onConnectionLost;
client.onMessageArrived = onMessageArrived;

// connect the client
client.connect({onSuccess:onConnect});
acelerX = 0;
acelerY = 0;
acelerZ = 0;
giroX = 0;
giroY = 0;
giroZ = 0;



// called when the client connects
function onConnect() {
  // Once a connection has been made, make a subscription and send a message.
  console.log("Conectado MQTT-WebSocket");
    client.subscribe("IoT/ACX");    
    client.subscribe("IoT/ACY");
    client.subscribe("IoT/ACZ");
    client.subscribe("IoT/GYX");
    client.subscribe("IoT/GYY");
    client.subscribe("IoT/GYZ");


    }

// called when the client loses its connection
function onConnectionLost(responseObject) {
  if (responseObject.errorCode !== 0) {
    console.log("Conexión perdida:"+responseObject.errorMessage);
  }
}


// called when a message arrives
function onMessageArrived(message) {
  console.log(message.destinationName + ": " + message.payloadString);
    
  if(message.destinationName == 'IoT/ACX')
    {
         //document.getElementById("AcX").textContent =  message.payloadString ;
        acelerX = parseFloat(message.payloadString);
    } 
    if(message.destinationName == 'IoT/ACY')
    {
        // document.getElementById("AcY").textContent =  message.payloadString ;
        acelerY = parseFloat(message.payloadString);
    } 
    if(message.destinationName == 'IoT/ACZ')
    {
         //document.getElementById("AcZ").textContent =  message.payloadString ;
        acelerZ = parseFloat(message.payloadString);
    } 
    if(message.destinationName == 'IoT/GYX')
    {
         //document.getElementById("GyX").textContent =  message.payloadString ;
        giroX = parseFloat(message.payloadString);
    }
    if(message.destinationName == 'IoT/GYY')
    {
         //document.getElementById("GyY").textContent =  message.payloadString ;
         giroY = parseFloat(message.payloadString);

    }   if(message.destinationName == 'IoT/GYZ')
    {
         //document.getElementById("GyZ").textContent =  message.payloadString ;
         giroZ = parseFloat(message.payloadString);
    } 
}
