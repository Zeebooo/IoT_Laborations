#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char *wifi_ssid = "NETGEAR61";
const char *wifi_pwd = "jaggedmountain461";
const char *mqtt_server = "192.168.1.8";
const int mqtt_port = 1883;

uint32_t next_time_100ms = 100;
boolean larm = false;
boolean lastButton1State = HIGH;
boolean lastButton2State = HIGH;

#define BTN_1 21
#define BTN_2 4

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
	Serial.print("Message: ");
	String msg;
	for (unsigned int i = 0; i < length; i++)
	{
		msg += (char)payload[i];
	}

	if (msg == "Larm")
	{
		larm = true;
	}
	else if(msg == "NoLarm")
	{
		larm = false;
	}

	Serial.println(larm);
}

void every100ms()
{
	if (!mqttClient.connected())
	{
		return;
	}

	if (!larm)
	{
		int button1state = digitalRead(BTN_1);
		int button2state = digitalRead(BTN_2);

		if (button1state == LOW && button2state == HIGH)
		{
			mqttClient.publish("LEDBroker", "01");
			lastButton1State = button1state;
		}
		if (button2state == LOW && button1state == HIGH)
		{
			mqttClient.publish("LEDBroker", "10");
			lastButton2State = button2state;
		}
		if(button1state == LOW && button2state == LOW)
		{
			mqttClient.publish("LEDBroker", "11");
			lastButton1State = button1state;
			lastButton2State = button2state;
		}

		if(button1state != lastButton1State || button2state != lastButton2State)
		{
			mqttClient.publish("LEDBroker", "00");
			lastButton1State = button1state;
			lastButton2State = button2state;
		}
	}
}

void setup()
{
	Serial.begin(115200);

	mqttClient.setServer(mqtt_server, mqtt_port);
	mqttClient.setCallback(mqtt_callback);

	WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
	WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
	WiFi.onEvent(WifiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
	WiFi.begin(wifi_ssid, wifi_pwd);

	pinMode(BTN_1, INPUT_PULLUP);
	pinMode(BTN_2, INPUT_PULLUP);

	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.print("Connecting to WiFi SSID: ");
		Serial.println(wifi_ssid);
		delay(2000);
	}
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
}
