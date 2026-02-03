#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char *wifi_ssid = "NETGEAR61";
const char *wifi_pwd = "jaggedmontain461";
const char *mqtt_server = "192.168.1.8";
const int mqtt_port = 1883;

void setup()
{
	Serial.begin(115200);

	mqttClient.setServer(mqtt_server, mqtt_port);
	mqttClient.setCallback(mqtt_callback);

	WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
	WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
	WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
	WiFi.begin(wifi_ssid, wifi_pwd);

	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.print("Connecting to WiFi SSID: ");
		Serial.println(wifi_ssid);
		delay(2000);
	}
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	while (!mqttClient.connected())
	{
		Serial.print("Connecting to MQTT...");
		Serial.print(mqtt_server);
		if (mqttClient.connect("esp32-client"))
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
	// ...
}

void loop()
{
	mqttClient.loop();
}
