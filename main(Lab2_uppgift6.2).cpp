#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define LEDR 14
#define LEDY 32
#define LEDG 15

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char *wifi_ssid = "NETGEAR61";
const char *wifi_pwd = "jaggedmountain461";
const char *mqtt_server = "192.168.1.8";
const int mqtt_port = 1883;
bool Larm = false;
bool led_state = false;
uint32_t next_500ms = 500;

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	while (!mqttClient.connected())
	{
		Serial.print("Connecting to MQTT...");
		Serial.print(mqtt_server);
		if (mqttClient.connect("esp1-client"))
		{
			Serial.println(". Connection established.");
		}
		else
		{
			Serial.print(". failed to connect with state ");
			Serial.println(mqttClient.state());
			delay(2000);
		}
	}
	mqttClient.subscribe("LEDBroker");
}

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
	Serial.println("Connected to WiFi.");
}

void WifiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
	Serial.println("Disconnected from WiFi.");
}

void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
	Serial.print("\nMessage arrived in topic: ");
	Serial.println(topic);
	String message;
	

	for(int i = 0; i < length; i++)
	{
		message += (char)payload[i];
	}

	if(message == "Larm"){
		Larm = true;
	}
	else if(message == "NoLarm"){
		Larm = false;
		digitalWrite(LEDR, LOW);
		digitalWrite(LEDY, LOW);
		digitalWrite(LEDG, LOW);
	}
	
	if (!Larm){
		if(message == "00"){
		digitalWrite(LEDR, LOW);
		digitalWrite(LEDY, LOW);
		digitalWrite(LEDG, LOW); 
		}
		else if(message == "01"){
			digitalWrite(LEDR, HIGH);
			digitalWrite(LEDY, LOW);
			digitalWrite(LEDG, LOW);	
		}
		else if(message == "10"){
			digitalWrite(LEDR, HIGH);
			digitalWrite(LEDY, HIGH);
			digitalWrite(LEDG, LOW);
		}
		else if(message == "11"){
			digitalWrite(LEDR, HIGH);
			digitalWrite(LEDY, HIGH);
			digitalWrite(LEDG, HIGH);
		}
	}
	
	
}

void every500ms(){
	if(Larm && !led_state){
		digitalWrite(LEDR, HIGH);
		digitalWrite(LEDY, HIGH);
		digitalWrite(LEDG, HIGH); 
		led_state = true;
	} else if(Larm && led_state){
		digitalWrite(LEDR, LOW);
		digitalWrite(LEDY, LOW);
		digitalWrite(LEDG, LOW);
		led_state = false;	
	}
}

void setup()
{
	Serial.begin(115200);
	pinMode(LEDR, OUTPUT);
	pinMode(LEDY, OUTPUT);
	pinMode(LEDG, OUTPUT);

	mqttClient.setServer(mqtt_server, mqtt_port);
	mqttClient.setCallback(mqtt_callback);

	WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
	WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
	WiFi.onEvent(WifiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
	WiFi.begin(wifi_ssid, wifi_pwd);

	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.print("Connecting to WiFi SSID: ");
		Serial.println(wifi_ssid);
		delay(2000);
	}
}

void loop()
{
	mqttClient.loop();
	if (millis() > next_500ms){
		next_500ms += 500;
		every500ms();
	}
}
