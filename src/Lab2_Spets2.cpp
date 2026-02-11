#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <iostream>
#include <string>

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

#define BUZ 14
#define BTN 32

const char *wifi_ssid = "NETGEAR61";
const char *wifi_pwd = "jaggedmountain461";
const char *mqtt_server = "192.168.1.8";
const int mqtt_port = 1883;

uint32_t next_time_100ms = 100;
uint32_t next_time_1s = 1000;
int timer = 0;
boolean activated = false;

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	while (!mqttClient.connected())
	{
		Serial.print("Connecting to MQTT...");
		Serial.print(mqtt_server);
		if (mqttClient.connect("esp2-client"))
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
	mqttClient.subscribe("MusicBroker");
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
	Serial.print("Message: ");
	char msg[4];
	for (unsigned int i = 0; i < length; i++)
	{
		msg[i] += (char)payload[i];
	}

	int freak = atoi(msg);

	if (timer > 0)
	{
		tone(BUZ, freak);
	}
}

void every100ms()
{
	int buttonState = digitalRead(BTN);

	if(buttonState == LOW && !activated)
	{
		activated = true;
		timer = 10;
		Serial.println("Button pressed, buzzer activated for 10s");
	}
}

void every1s()
{
	if(activated)
	{
		timer--;
		Serial.print("Buzzer active for ");
		Serial.print(timer);
		Serial.println(" more seconds");

		if(timer <= 0)
		{
			activated = false;
			noTone(BUZ);
			Serial.println("Buzzer deactivated");
		}
	}
}

void setup()
{
	Serial.begin(9600);

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

	pinMode(BTN, INPUT_PULLUP);
	pinMode(BUZ, OUTPUT);
}

void loop()
{
	uint32_t time = millis();
	if (time >= next_time_100ms)
	{
		next_time_100ms += 100;
		mqttClient.loop();
		every100ms();
	}
	if(time >= next_time_1s)
	{
		next_time_1s += 1000;
		every1s();
	}
}
