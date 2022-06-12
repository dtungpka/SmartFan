/*
 Name:		SmartFan.ino
 Created:	6/10/2022 6:12:34 PM
 Author:	Admin
*/
#define TIME_HOLD 1000
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
uint32_t sec=-1,micro_sec = 0;
int holding = 0;
int water_mode = 1;
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
int showLED(int led_mode);
int CheckHold(bool thres);
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
void LEDThres() {
	if (sec > NIGHT_SAVING_TIME)
	{
		digitalWrite(LED_RED, 1);
	}
	else {
		digitalWrite(LED_YELLOW, 0);
		digitalWrite(LED_GREEN, 0);
		digitalWrite(LED_RED, (sec < NIGHT_SAVING_TIME / 3) && sec % 3 ? 0 : 1);
		return;
	}
	if (sec > NIGHT_SAVING_TIME*2)
	{
		digitalWrite(LED_YELLOW, 1);
	}
	else {
		digitalWrite(LED_GREEN, 0);
		digitalWrite(LED_YELLOW, (sec- NIGHT_SAVING_TIME < NIGHT_SAVING_TIME / 3) && sec % 3 ? 0 : 1);
		return;
	}
		digitalWrite(LED_GREEN, (sec - NIGHT_SAVING_TIME*2 < NIGHT_SAVING_TIME / 3) && sec % 3 ? 0 : 1);
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
				if (sec > 0) {
					LEDThres();
					sec--;
				}
				micro_sec = 0;
				if (sec == 0)return;
			}
			if (Touch1())return;

		}
	}
}


void SetMode() {
	int holdLV = CheckHold(!(mode == ON || mode == OFF));
	if (holdLV == 0)
	{
		mode = (mode == ON || mode == OFF) ? mode : ON;
	}
	else if (holdLV == 1) {
		sec = CheckHold(true) * NIGHT_SAVING_TIME;
		mode = TIMER_OFF;
	}
	else if (holdLV == 2) {
		sec = CheckHold(true) * NIGHT_SAVING_TIME;
		mode = TIMER_ON;
	}
	else if (holdLV == 3) {
		water_mode = 10 + CheckHold(true) * 2;
		mode = WATER_SAVING;
	}
	

}
// the loop function runs over and over again until power down or reset
void loop() {
	switch (mode)
	{
	case ON:
		digitalWrite(LED_GREEN, 1);
		Humid(10);
		mode = OFF;
		digitalWrite(LED_GREEN, 0);
		digitalWrite(LED_YELLOW, 0);
		break;
	case OFF:
		for (int i = -255; i < 255; i++)
		{
			digitalWrite(LED_RED, abs(i));
			if (Touch1())SetMode();
			delay(10);
		}
		break;
	case TIMER_ON:
		showLED(1);
		Humid(10);
		break;
	case TIMER_OFF:
		showLED(2);
		Humid(10);
		break;
	case WATER_SAVING:
		showLED(3);
		Humid(water_mode);
		break;
	default:
		break;
	}
}
int showLED(int led_mode) {
  int value_red = 0, value_yellow = 0, value_green = 0;
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
	int time_out = TIME_OUT;
	int hd = 0;
	int level = 0;
	while (time_out > 0)
	{
		if (Touch1())
		{
			hd++;
			time_out = TIME_OUT;
		}
		else
		{
			if (hd > 0 && hd < TIME_HOLD)level = level == 4 ? 0 : level+ 1;
			if (hd >= TIME_HOLD) { showLED(4); return -1; }
			hd = 0;
		}
		time_out--;
		
		if (thres)
		{
			digitalWrite(LED_GREEN, level == 3);
			digitalWrite(LED_YELLOW, level >= 2 );
			digitalWrite(LED_RED, level >= 1);
		}
		else {
			if ((mode == ON || mode == OFF)) {
				digitalWrite(LED_GREEN, mode == ON);
				digitalWrite(LED_RED, mode == OFF);
				mode = mode == ON ? OFF : ON;
				return 0;
			}
			else {
				digitalWrite(LED_GREEN, level == 1);
				digitalWrite(LED_YELLOW, level == 3);
				digitalWrite(LED_RED, level == 2);
			}
		}
		delay(1);


	}
	if (thres) {
		showLED(level + 5);
	}
	else {
		showLED(level);
	}
	return level;
}
