#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Airtel_My_Network";
const char* password = "9910357357";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Hardware pins for the ESP32-S3 (Change these to match your actual wiring)
const int ledPins[3] = {4, 5, 6}; 
int brightness[3] = {0, 0, 0};

// The condensed HTML and JavaScript UI
const char index_html[] PROGMEM = R"HTML(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body { background-color: #121212; color: #ffffff; font-family: sans-serif; text-align: center; }
.card { background: #1e1e1e; margin: 20px auto; padding: 20px; border-radius: 10px; max-width: 300px; }
input[type=range] { width: 100%; accent-color: #38bdf8; }
</style></head><body>
<h2>LED Controller</h2>
<p id="status" style="color: #888;">Connecting...</p>
<div id="dash"></div>
<script>
let ws = new WebSocket("ws://" + location.hostname + "/ws");

ws.onopen = () => { 
  document.getElementById("status").innerText = "Connected"; 
  ws.send("getValues"); 
};

ws.onmessage = (e) => {
  let data = JSON.parse(e.data);
  for(let i=1; i<=3; i++) {
    document.getElementById("s"+i).value = data["sliderValue"+i];
    document.getElementById("v"+i).innerText = data["sliderValue"+i] + "%";
  }
};

function update(el) { ws.send(el.id.replace("s","") + "s" + el.value); }

let html = "";
for(let i=1; i<=3; i++) html += `<div class='card'><h3>Channel ${i}</h3><h2 id='v${i}' style='color:#38bdf8'>0%</h2><input type='range' id='s${i}' min='0' max='100' value='0' oninput='update(this)'></div>`;
document.getElementById("dash").innerHTML = html;
</script></body></html>
)HTML";

void broadcastUpdate() {
  char json[100];
  snprintf(json, sizeof(json), "{\"sliderValue1\":%d,\"sliderValue2\":%d,\"sliderValue3\":%d}", brightness[0], brightness[1], brightness[2]);
  ws.textAll(json);
}

void onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
  if (type == WS_EVT_DATA) {
    String msg = "";
    for (size_t i = 0; i < len; i++) msg += (char)data[i];
    
    if (msg == "getValues") {
      broadcastUpdate();
    } else {
      int ch, val;
      if (sscanf(msg.c_str(), "%ds%d", &ch, &val) == 2) {
        ch--; 
        brightness[ch] = constrain(val, 0, 100);
        // Map 0-100% to 0-255 hardware PWM range (Core 2.x syntax uses channel number, not pin)
        ledcWrite(ch, map(val, 0, 100, 0, 255));
        broadcastUpdate();
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // ESP32 Core 2.x syntax for setting up PWM
  for (int i = 0; i < 3; i++) {
    ledcSetup(i, 5000, 8);        // Setup channel i, 5000 Hz, 8-bit resolution
    ledcAttachPin(ledPins[i], i); // Attach physical pin to channel i
    ledcWrite(i, 0);              // Set initial duty cycle to 0 on channel i
  }
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print("."); 
  }
  Serial.println("\nIP: " + WiFi.localIP().toString());
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    // Changed send_P to send to fix the deprecation warning
    request->send(200, "text/html", index_html);
  });
  
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);
  server.begin();
}

void loop() {
  ws.cleanupClients();
}