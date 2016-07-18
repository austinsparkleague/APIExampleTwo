// Function to send a message to the Pebble using AppMessage API
// We are currently only sending a message using the "status" appKey defined in appinfo.json/Settings
function sendMessage(weather, temp) {
  var weatherString = "It is " + weather.toLowerCase() + " in Austin, about " + Math.floor(temp) + " degrees.";
  Pebble.sendAppMessage({"message": weatherString}, messageSuccessHandler, messageFailureHandler);
}

// Called when the message send attempt succeeds
function messageSuccessHandler() {
  console.log("Message send succeeded.");  
}

// Called when the message send attempt fails
function messageFailureHandler() {
  console.log("Message send failed.");
  sendMessage();
}

// Called when JS is ready
Pebble.addEventListener("ready", function(e) {
  sleep(1000);
  var xmlhttp = new XMLHttpRequest();
  var url = "https://api.forecast.io/forecast/7f41d605d25b4f0b1622d75ea0c138de/30.267153,-97.743061";

  xmlhttp.onreadystatechange = function() {
      if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
          var infoFromAPI = JSON.parse(xmlhttp.responseText);
          var summary = infoFromAPI.currently.summary;
          var temp = infoFromAPI.currently.temperature;
        
          sendMessage(summary, temp);
      }
  };
  
  xmlhttp.open("GET", url, true);
  xmlhttp.send();
  
});

function sleep(milliseconds) {
  var start = new Date().getTime();
  for (var i = 0; i < 1e7; i++) {
    if ((new Date().getTime() - start) > milliseconds){
      break;
    }
  }
}
  				
// Called when incoming message from the Pebble is received
// We are currently only checking the "message" appKey defined in appinfo.json/Settings
Pebble.addEventListener("appmessage", function(e) {
  console.log("Received Message: " + e.payload.message);
});