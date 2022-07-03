#include <SAM32WiFiEsp.h>
#include <PubSubClient.h>
#include "DHT.h"
#include "LowPower.h"
#include "RealTimeClock.h"
#include "Secrets.h"

char ssid[] = SECRET_SSID ;            // your network SSID (name)
char pass[] = SECRET_PASS ;            // your network password
char mqttUser[] = SECRET_MQTT_USER;    // your MQTT User
char mqttPass[] = SECRET_MQTT_PASS ;   // your MQTT password

int status = WL_IDLE_STATUS;           // the Wifi radio's status

#define DHT11PIN 15 
#define RESETPIN 8
#define BATTERYPIN 22

String topic = "Sensor/Outside"; // MQTT Topic make unique for each sensor

int dht11_val[5]={0,0,0,0,0};  

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

// The address of our MQTT Server, todo use mDNS to resolve this by name
IPAddress server(192, 168, 4, 52);

// Initialize the Ethernet client object
WiFiEspClient netClient;

// Initialize the MQTT client
PubSubClient client(server, 1883, callback, netClient);

void setup()
{
  //Attach a wire from GPIO 8 to RUN so we can perform a hardware reset
  pinMode(RESETPIN,INPUT_PULLUP);

  pinMode(BATTERYPIN,INPUT_PULLUP);

  // initialize serial for ESP module
  Serial1.begin(115200);  //UART0 on pins 0 and 1
  // initialize ESP module
  WiFi.init(&Serial1);

    // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    //failed to detect wifi
    hardReset();
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  //Connected to the network

  //Connect MQTT
  if (client.connect("arduinoClient", SECRET_MQTT_USER, SECRET_MQTT_PASS)) {
    String connectMsg = "Connecting "  + topic;
    client.publish("Connect",connectMsg.c_str());
  }

  //Read Temp
  while (!dht11_read_val(DHT11PIN, &dht11_val)) {
    delay(500); // Wait and retry
  }

  //Read Battery
  byte batteryState = digitalRead(BATTERYPIN);

  sendMsg(&dht11_val,batteryState);

  //Can't do much once in low power mode so add any additional functionality above this line
  set_lowPower();

  //Wait for 1 minute
  setup_rtc();  
  waitfor_rtc(60);  

  hardReset();
}

void hardReset() 
{
  //Reset  
  pinMode(RESETPIN,OUTPUT);
  digitalWrite(RESETPIN, LOW);   
}

void loop()
{
  // Code never reaches here as the Pico is reset before
}

// Form a JSON string to send to MQTT
void sendMsg(int (*dht11_results)[5], byte batteryState)
{
  String msg = "{\"H\":" + String(*dht11_results[0]) + "." + String(*dht11_results[1]) + ",\"T\":" + String(*dht11_results[2]) +  "." + String(*dht11_results[3]) + ",\"B\":" + String(batteryState) + "}";
  client.publish(topic.c_str(),msg.c_str());
}
