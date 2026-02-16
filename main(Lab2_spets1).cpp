#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#define UltraSonic 0x70  // Slave adress for the ultrasonic sensor
#define CMDRegister 0x00 // Register for sending commands to the ultrasonic sensor

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char *wifi_ssid = "NETGEAR61";
const char *wifi_pwd = "jaggedmountain461";
const char *mqtt_server = "192.168.1.8";
const int mqtt_port = 1883;
bool Larm = false;
bool led_state = false;
uint32_t next_250ms = 250;

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

    for (int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }
}

void every250ms()
{
    Wire.beginTransmission(UltraSonic);
    Wire.write(CMDRegister);
    Wire.write(0x51);
    Wire.endTransmission();

    delay(70);

    Wire.beginTransmission(UltraSonic);
    Wire.write(0x02);
    Wire.endTransmission();

    uint8_t bytesReceived = Wire.requestFrom(UltraSonic, 2);

    if (bytesReceived == 2)
    {
        uint8_t byte1 = Wire.read();
        uint8_t byte2 = Wire.read();
        uint16_t distance = (byte1 << 8) | byte2;
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" cm");

        if (distance >= 25 && distance < 30)
        {
            mqttClient.publish("MusicBroker", "1046");
        }
        else if (distance >= 30 && distance < 35)
        {
            mqttClient.publish("MusicBroker", "1172");
        }
        else if (distance >= 35 && distance < 40)
        {
            mqttClient.publish("MusicBroker", "1319");
        }
        else if (distance >= 40 && distance < 45)
        {
            mqttClient.publish("MusicBroker", "1396");
        }
        else if (distance >= 45 && distance < 50)
        {
            mqttClient.publish("MusicBroker", "1568");
        }
        else if (distance >= 50 && distance < 55)
        {
            mqttClient.publish("MusicBroker", "1760");
        }
        else if (distance >= 55 && distance < 60)
        {
            mqttClient.publish("MusicBroker", "1976");
        }
        else if (distance >= 60 && distance < 65)
        {
            mqttClient.publish("MusicBroker", "2093");
        }
    }
    else
    {
        Serial.println("Error reading from ultrasonic sensor");
    }
}

void setup()
{
    Wire.begin();
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
}

void loop()
{
    mqttClient.loop();
    if (millis() > next_250ms)
    {
        next_250ms += 250;
        every250ms();
    }
}
