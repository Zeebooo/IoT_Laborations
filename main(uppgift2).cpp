#include <Arduino.h>

#define LEDR 14
#define LEDY 32
#define LEDG 15

#define BTN_1 21
#define BTN_2 4

#define MSG_BUFLEN 256
char msg[MSG_BUFLEN];

int buttonState1 = 0;
int buttonState2 = 0;
int button2Presses = 0;
int typeTime = 0;

void setup()
{
	// put your setup code here, to run once:
	Serial.begin(115200);
	pinMode(LEDR, OUTPUT);
	pinMode(LEDY, OUTPUT);
	pinMode(LEDG, OUTPUT);
	pinMode(BTN_1, INPUT_PULLUP);
	pinMode(BTN_2, INPUT_PULLUP);
}

void loop()
{
	// put your main code here, to run repeatedly:

	int temp = 0;

	digitalWrite(LEDR, HIGH);
	delay(500);
	digitalWrite(LEDR, LOW);
	delay(500);

	buttonState1 = digitalRead(BTN_1);

	if (buttonState1 == LOW)
	{
		digitalWrite(LEDY, HIGH);
		delay(500);
		digitalWrite(LEDY, LOW);
	}

	buttonState2 = digitalRead(BTN_2);

	if (buttonState2 == LOW)
	{
		button2Presses++;
		typeTime = 1;
	}

	if (button2Presses % 2 != 0)
	{
		digitalWrite(LEDG, HIGH);
		delay(500);
	}
	else
	{
		digitalWrite(LEDG, LOW);
		delay(500);
	}

	if (temp != typeTime)
	{
		snprintf(msg, MSG_BUFLEN, "Knapp 2 har tryckts på %d gånger", button2Presses);
		Serial.println(msg);
		typeTime = 0;
		delay(500);
	}
}