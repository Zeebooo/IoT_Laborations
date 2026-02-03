#include <Arduino.h>
#include <Wire.h>

#define MSG_BUFLEN 256
char msg[MSG_BUFLEN];

#define MCP9808_I2CADDR 0x18		  // 0x00011000 std slave address
#define MCP9808_REG_AMBIENT_TEMP 0x05 // 0b00000101 temp data register
#define PotPIN 36
#define LEDR 14
#define BUZ 32

uint32_t next_time1s = 1000;
uint32_t next_time500ms = 500;

float tempLimit = 30;
int miliVolt = 0;
bool printFlag = false;
float tempC = 0.0;

void setup()
{
	// put your setup code here, to run once:
	Wire.begin(23, 22, 100000);
	Serial.begin(115200);
	pinMode(PotPIN, INPUT);
	pinMode(LEDR, OUTPUT);
	ledcAttach(BUZ, 500, 8);
}

void every1s()
{
	if (printFlag)
	{
		digitalWrite(LEDR, LOW);
		snprintf(msg, MSG_BUFLEN, "Temperature: %.2f C, Limit: %.2f °C, HOT HOT HOT!!!!!\n", tempC, tempLimit);
		printFlag = false;
	}
	else
	{
		snprintf(msg, MSG_BUFLEN, "Temperature: %.2f C, Limit: %.2f C\n", tempC, tempLimit);
	}

	Serial.print(msg);
}

void every100ms()
{
	// Nothing to do here for now
	Wire.beginTransmission(MCP9808_I2CADDR);

	Wire.write(MCP9808_REG_AMBIENT_TEMP);
	Wire.endTransmission(false);
	uint8_t bytesReceived = Wire.requestFrom(MCP9808_I2CADDR, 2, true);
	miliVolt = analogReadMilliVolts(PotPIN);

	if (bytesReceived == 2)
	{
		uint8_t byte1 = Wire.read();
		uint8_t byte2 = Wire.read();
		uint16_t t = (byte1 << 8) | byte2;
		t = t & 0x0fff;
		tempC = t / 16.0;

		tempLimit = map(miliVolt, 0, 3300, 20, 40);

		if (tempC > tempLimit)
		{
			ledcWrite(BUZ, 128);
			digitalWrite(LEDR, HIGH);
			printFlag = true;
		}
		else
		{
			digitalWrite(LEDR, LOW);
			ledcWrite(BUZ, 0);
		}
	}
	else
	{
		Serial.println("error reading from temperature sensor");
	}
}

void loop()
{
	// put your main code here, to run repeatedly:
	uint32_t time = millis();
	if (time >= next_time500ms)
	{
		next_time500ms += 500;
		every100ms();
	}
	if (time >= next_time1s)
	{
		next_time1s += 1000;
		every1s();
	}
}