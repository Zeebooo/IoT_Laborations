#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

/* === Konfig === */
const char *wifi_ssid = "SSID";
const char *wifi_pwd = "PASS";

const char *mqtt_server = "192.168.1.10";
const int mqtt_port = 1883;

/* === Forward declarations === */
// void mqtt_callback(char* topic, byte* payload, unsigned int length);
// void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);

/* === Klienter === */
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup()
{
	Serial.begin(115200);

	mqttClient.setServer(mqtt_server, mqtt_port);
	mqttClient.setCallback(mqtt_callback);

	WiFi.onEvent(WiFiGotIP, ARDUINO_EVENT_WIFI_STA_GOT_IP);

	WiFi.begin(wifi_ssid, wifi_pwd);
	Serial.println("Connecting to WiFi...");
}

/* === WiFi event === */
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
	Serial.print("IP: ");
	Serial.println(WiFi.localIP());

	while (!mqttClient.connected())
	{
		if (mqttClient.connect("esp32-client"))
		{ 
			Serial.println("MQTT connected");
			mqttClient.subscribe("LEDBroker");
		}
		else
		{
			Serial.println(mqttClient.state());
			delay(2000);
		}
	}
}

/* === MQTT callback === */
void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
	Serial.print("Topic: ");
	Serial.println(topic);
}

/* === Loop === */
void loop()
{
	mqttClient.loop();
}
