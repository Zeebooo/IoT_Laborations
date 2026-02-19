#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#define FAN 13
#define MCP9808_I2CADDR 0x18		  // 0x00011000 std slave address
#define MCP9808_REG_AMBIENT_TEMP 0x05 // 0b00000101 temp data register

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char *wifi_ssid = "NETGEAR61";
const char *wifi_pwd = "jaggedmountain461";
const char *mqtt_server = "192.168.1.8";
const int mqtt_port = 1883;

uint32_t next_time_1s = 1000;
float tempC = 0.0;

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	while (!mqttClient.connected())
	{
		Serial.print("Connecting to MQTT...");
		Serial.print(mqtt_server);
		if (mqttClient.connect("esp1-temp"))
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
}

void every1s()
{
	Wire.beginTransmission(MCP9808_I2CADDR);

	Wire.write(MCP9808_REG_AMBIENT_TEMP);
	Wire.endTransmission(false);
	uint8_t bytesReceived = Wire.requestFrom(MCP9808_I2CADDR, 2, true);

	if (bytesReceived == 2)
	{
		uint8_t byte1 = Wire.read();
		uint8_t byte2 = Wire.read();
		uint16_t t = (byte1 << 8) | byte2;
		t = t & 0x0fff;
		tempC = t / 16.0;

		String msg = String(tempC);

		mqttClient.publish("LEDBroker", msg.c_str());
	}
	else
	{
		Serial.println("error reading from temperature sensor");
	}
}

void setup()
{
	Serial.begin(115200);
	Wire.begin(23, 22, 100000);

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
	if (millis() > next_time_1s)
	{
		next_time_1s += 1000;
		every1s();
	}
}
