#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>

#define FAN 13
#define PWM_FREQ 25000	 // 25 kHz
#define PWM_RESOLUTION 8 // 8-bit (0-255)

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char *wifi_ssid = "NETGEAR61";
const char *wifi_pwd = "jaggedmountain461";
const char *mqtt_server = "192.168.1.8";
const int mqtt_port = 1883;

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	while (!mqttClient.connected())
	{
		Serial.print("Connecting to MQTT...");
		Serial.print(mqtt_server);
		if (mqttClient.connect("esp1-fläkt"))
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

	for (int i = 0; i < length; i++)
	{
		message += (char)payload[i];
	}

	Serial.print("Message: ");
	Serial.println(message);

	if (message == "HIGH")
	{
		ledcWrite(FAN, 255);
	}
	else if (message == "LOW")
	{
		ledcWrite(FAN, 5);
	}
}

void setup()
{
	Serial.begin(115200);
	ledcAttach(FAN, PWM_FREQ, PWM_RESOLUTION);
	ledcWrite(FAN, 0);

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
}
