/*
 Name:		SmartFan.ino
 Created:	6/10/2022 6:12:34 PM
 Author:	Admin
*/
#define TIME_HOLD 500
#define TIME_OUT 1000
#define TOUCH_THRES 400
// the setup function runs once when you press reset or power the board
const int NIGHT = 400;
const int NIGHT_SAVING_TIME = 1800;
int LED_GREEN = 1;
int LED_YELLOW = 2;
int LED_RED = 3;
int WATER_PIN = 4;
int SENSOR_PIN = A1;
int TOUCH_1 = A2;
int TOUCH_2 = A3;
uint32_t sec=0,micro_sec = 0;
int holding = 0;

enum Mode {
	ON = 0, //GREEN
	OFF = -1, //RED LED
	TIMER_ON = 2, //off to on: RED -> GREEN
	TIMER_OFF = 3, // on to off: GREEN -> RED on timer
	WATER_SAVING = 4 //YELLOW

};
Mode mode = OFF;
/*
normal:
off: red pwm
on: green pwm 
HOLD:
1: GREEN to red pwm: on -> off
2: RED to green pwm: off -> on
3: YELLOW: water saving pwm at slow speed: release: choose speed red yellow green
*/

void setup() {
	pinMode(LED_GREEN, OUTPUT);
	pinMode(LED_YELLOW, OUTPUT);
	pinMode(LED_RED, OUTPUT);
	pinMode(WATER_PIN, OUTPUT);
	pinMode(SENSOR_PIN, INPUT);
	pinMode(TOUCH_1, INPUT);

	delay(showLED(4));
	digitalWrite(LED_RED, 1);
}
bool Touch1() {
	return analogRead(TOUCH_1) < TOUCH_THRES;
}
void Humid(int t) {
	int checkFreq = 1000;
	while (true)
	{
		digitalWrite(WATER_PIN, 0);
		delayMicroseconds(t);
		digitalWrite(WATER_PIN, 1);
		delayMicroseconds(t);
		checkFreq--;
		if(checkFreq == 0){
			checkFreq = 1000;
			micro_sec += checkFreq * t * 2;
			if (micro_sec >= 1000000)
			{
				sec--;
				micro_sec = 0;
				if (sec <= 0)return;
			}
			if (Touch1())return;

		}
	}
}

int showLED(int led_mode);
int CheckHold(bool thres);
void SetMode() {
	int holdLV;
	if (mode == WATER_SAVING)
	{
		int lv = 
	}
	else  holdLV = CheckHold(false);
	if ((mode == ON || mode == OFF) && holdLV < 1)
	{
		mode = mode == ON ? OFF : ON;
		return;
	}
	

}
// the loop function runs over and over again until power down or reset
void loop() {
	switch (mode)
	{
	case ON:
		break;
	case OFF:
		break;
	case TIMER_ON:
		break;
	case TIMER_OFF:
		break;
	case WATER_SAVING:
		break;
	default:
		break;
	}
}
int showLED(int led_mode) {
	switch (led_mode)
	{
	case -1:
		digitalWrite(LED_YELLOW, 0);
		digitalWrite(LED_GREEN, 0);
		digitalWrite(LED_RED, 0);
		delay(TIME_HOLD / 10);
		return TIME_HOLD / 2;
	case 0: // on off
		digitalWrite(LED_YELLOW, 0);
		digitalWrite(LED_GREEN, 1);
		digitalWrite(LED_RED, 0);
		delay(TIME_HOLD / 10);
		digitalWrite(LED_GREEN, 0);
		digitalWrite(LED_RED, 1);
		delay(TIME_HOLD / 10);
		return TIME_HOLD / 5;
	case 1: // green -> red
		int value_red = 0, value_yellow = 0, value_green = 0;
		for (int i = 0; i < 765; i += 5)
		{
			value_green = max(i, 255);
			value_yellow = i >= 255 ? min(i, 510) - 255 : 0;
			value_yellow = i >= 510 ? min(i, 765) - 510 : 0;
			analogWrite(LED_YELLOW, value_yellow);
			analogWrite(LED_GREEN, value_green);
			analogWrite(LED_RED, value_red);
			delay(1);



		}
		analogWrite(LED_YELLOW, 0);
		analogWrite(LED_GREEN, 0);
		analogWrite(LED_RED, 0);
		return 765 / 5;
	case 2: //red -> green
		int value_red = 0, value_yellow = 0, value_green = 0;
		for (int i = 765; i > 0; i -= 5)
		{
			value_green = max(i, 255);
			value_yellow = i >= 255 ? min(i, 510) - 255 : 0;
			value_yellow = i >= 510 ? min(i, 765) - 510 : 0;
			analogWrite(LED_YELLOW, value_yellow);
			analogWrite(LED_GREEN, value_green);
			analogWrite(LED_RED, value_red);
			delay(1);



		}
		analogWrite(LED_YELLOW, 0);
		analogWrite(LED_GREEN, 0);
		analogWrite(LED_RED, 0);
		return 765 / 5;
	case 3: //yellow pwm
		digitalWrite(LED_YELLOW, 0);
		digitalWrite(LED_GREEN, 0);
		digitalWrite(LED_RED, 0);
		for (int i = 0; i < 255; i += 2)
		{
			analogWrite(LED_YELLOW, i);
			delay(1);
		}
		for (int i = 255; i > 0; i -= 2)
		{
			analogWrite(LED_YELLOW, i);
			delay(1);
		}
		analogWrite(LED_YELLOW, 0);
		return 255;
	case 4: // off - on
		digitalWrite(LED_YELLOW, 0);
		digitalWrite(LED_GREEN, 0);
		digitalWrite(LED_RED, 0);
		delay(TIME_HOLD / 10);
		digitalWrite(LED_GREEN, 1);
		digitalWrite(LED_RED, 1);
		delay(TIME_HOLD / 10);
		digitalWrite(LED_YELLOW, 0);
		digitalWrite(LED_GREEN, 0);
		digitalWrite(LED_RED, 0);
		delay(TIME_HOLD / 10);
		return TIME_HOLD / 10 * 3;

	case 6:
		digitalWrite(LED_YELLOW, 0);
		digitalWrite(LED_GREEN, 0);
		digitalWrite(LED_RED, 0);
		for (int i = 0; i < 255; i += 2)
		{
			analogWrite(LED_GREEN, i);
			delay(1);
		}
		for (int i = 255; i > 0; i -= 2)
		{
			analogWrite(LED_GREEN, i);
			delay(1);
		}
		analogWrite(LED_GREEN, 0);
		return 255;
	case 7:
		digitalWrite(LED_YELLOW, 0);
		digitalWrite(LED_GREEN, 0);
		digitalWrite(LED_RED, 0);
		for (int i = 0; i < 255; i += 2)
		{
			analogWrite(LED_YELLOW, i);
			delay(1);
		}
		for (int i = 255; i > 0; i -= 2)
		{
			analogWrite(LED_YELLOW, i);
			delay(1);
		}
		analogWrite(LED_YELLOW, 0);
		return 255;
	case 8:
		digitalWrite(LED_YELLOW, 0);
		digitalWrite(LED_GREEN, 0);
		digitalWrite(LED_RED, 0);
		for (int i = 0; i < 255; i += 2)
		{
			analogWrite(LED_RED, i);
			delay(1);
		}
		for (int i = 255; i > 0; i -= 2)
		{
			analogWrite(LED_RED, i);
			delay(1);
		}
		analogWrite(LED_RED, 0);
		return 255;
	}
}

int CheckHold(bool thres) { //thres: 0: green
	int time_out = 200;
	int holdLv = -1;
	int curr_hold = 0;
	while (time_out > 0)
	{
		if (!thres) curr_hold = showLED(holdLv);
		else  curr_hold = showLED(holdLv + 6);
		if (Touch1())
		{
			holding+= curr_hold;
			time_out = TIME_OUT;
		}
		if (holding >= TIME_HOLD) {
			if(!thres)showLED(4);
			holdLv = holdLv == 5 ? -1 : holdLv + 1;
			holdLv = holdLv == 10 ? 6 : holdLv + 1;
			holding = 0;
		}
		time_out-= curr_hold;
		delay(1);
	}
	if (holdLv > -1)
	{
		digitalWrite(LED_YELLOW, 0);
		digitalWrite(LED_GREEN, 0);
		digitalWrite(LED_RED, 0);
		delay(100);
		delay(showLED(holdLv));
		digitalWrite(LED_YELLOW, 0);
		digitalWrite(LED_GREEN, 0);
		digitalWrite(LED_RED, 0);
	}
	else if (mode == ON || mode == OFF) {
		return  mode == ON ? -1 : 0;
	}
	return holdLv;
}

