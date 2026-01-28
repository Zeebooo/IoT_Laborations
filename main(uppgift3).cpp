#include <Arduino.h>

#define LEDR 14
#define LEDY 32
#define LEDG 15
#define PotPIN 36

int miliVolt = 0;

#define MSG_BUFLEN 256
char msg[MSG_BUFLEN];

void setup()
{
	// put your setup code here, to run once:
	Serial.begin(115200);
	pinMode(LEDR, OUTPUT);
	pinMode(LEDY, OUTPUT);
	pinMode(LEDG, OUTPUT);
	pinMode(PotPIN, INPUT);
}

void loop()
{
	// put your main code here, to run repeatedly:
	miliVolt = analogReadMilliVolts(PotPIN);

	if (miliVolt >= 1000 && miliVolt < 2000)
	{
		digitalWrite(LEDR, HIGH);
		digitalWrite(LEDY, LOW);
		digitalWrite(LEDG, LOW);
	}
	else if (miliVolt >= 2000 && miliVolt < 3000)
	{
		digitalWrite(LEDR, HIGH);
		digitalWrite(LEDY, HIGH);
		digitalWrite(LEDG, LOW);
	}
	else if (miliVolt >= 3000)
	{
		digitalWrite(LEDR, HIGH);
		digitalWrite(LEDY, HIGH);
		digitalWrite(LEDG, HIGH);
	}
	else
	{
		digitalWrite(LEDR, LOW);
		digitalWrite(LEDY, LOW);
		digitalWrite(LEDG, LOW);
	}

	snprintf(msg, MSG_BUFLEN, "Potentiometer voltage: %d mV\n", miliVolt);
	Serial.print(msg);
	delay(500);
}