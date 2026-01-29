#include <Arduino.h>

#define LEDR 14
#define LEDY 32
#define LEDG 15

#define BTN_1 21
#define BTN_2 4

#define MSG_BUFLEN 256
char msg[MSG_BUFLEN];

uint32_t next_time100 = 100;
uint32_t next_time1s = 1000;

bool LEDR_state = false;
bool LEDY_state = false;
int LEDG_state = 0;

int buttonState1 = 0;
int buttonState2 = 0;
int button2Presses = 0;
int lastButton2State;

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

void every100ms()
{
	buttonState1 = digitalRead(BTN_1);
	buttonState2 = digitalRead(BTN_2);

	// detektera knapptryck (HIGH → LOW eftersom INPUT_PULLUP)
	if (lastButton2State == HIGH && buttonState2 == LOW)
	{
		LEDG_state = !LEDG_state; // toggle
		digitalWrite(LEDG, LEDG_state);

		button2Presses++;
		snprintf(msg, MSG_BUFLEN,"Knapp 2 har tryckts på %d gånger",button2Presses);
		Serial.println(msg);
	}

	lastButton2State = buttonState2; // VIKTIGT: uppdateras här
}

void every1s()
{
	if (LEDR_state == true)
	{
		LEDR_state = false;
		digitalWrite(LEDR, LOW);
	}
	else if (LEDR_state == false)
	{
		LEDR_state = true;
		digitalWrite(LEDR, HIGH);
	}

	if (buttonState1 == LOW && LEDY_state == false)
	{
		digitalWrite(LEDY, HIGH);
		LEDY_state = true;
	}

	else if (LEDY_state == true)
	{
		digitalWrite(LEDY, LOW);
		LEDY_state = false;
	}
}

void loop()
{
	// put your main code here, to run repeatedly:

	uint32_t time = millis();
	if (time >= next_time100)
	{
		next_time100 += 100;
		every100ms();
	}
	if (time >= next_time1s)
	{
		next_time1s += 1000;
		every1s();
	}
}