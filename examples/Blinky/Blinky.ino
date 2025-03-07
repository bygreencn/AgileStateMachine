
/*
* States and transitions are statically allocated
*/

#include "AgileStateMachine.h"

#define PREV_BUTTON  	5
#define NEXT_BUTTON  	4
#define RESET_BUTTON  6
#define LED_BLINK 		20

// Let's use an array for change blink time
uint32_t blinkInterval = 1000;
uint32_t blinkSetTime[] = {0, 1000, 300, 150};


// A simple callback function for transition trigger (just button state)
bool xNextButton() {
	/*
	* Since we have defined a minimum run time for states, the button bounces
	* has no effects on state machine mechanism functioning
	* (FSM can switch to next state only after min time has passed)
	*/
	return (digitalRead(NEXT_BUTTON) == LOW);
}

bool xPrevButton() {
	/*
	* Since we have defined a minimum run time for states, the button bounces
	* has no effects on state machine mechanism functioning
	* (FSM can switch to next state only after min time has passed)
	*/
	return (digitalRead(PREV_BUTTON) == LOW);
}

bool xResetButton() {
	/*
	* Since we have defined a minimum run time for states, the button bounces
	* has no effects on state machine mechanism functioning
	* (FSM can switch to next state only after min time has passed)
	*/
	return (digitalRead(RESET_BUTTON) == LOW);
}

// Create new Finite State Machine
StateMachine myFSM;

// State label, min time, onEnter. onLeaving, onRun callbacks
State blinkOff("BlinkOFF", 200, onEntering, onLeaving, nullptr);
State blink1("Blink1", 200, onEntering, onLeaving, nullptr);
State blink2("Blink2", 200, onEntering, onLeaving, nullptr);
State blink3("Blink3", 200, onEntering, onLeaving, nullptr);

// Add transitions to target state and trigger condition (callback function or bool var)
Transition blinkOffToBlink1(blink1, xNextButton);
Transition blink1ToBlink2(blink2, xNextButton);
Transition blink2ToBlink3(blink3, xNextButton);
Transition blink3ToBlinkOff(blinkOff, xNextButton);     // xNextButton is a callback function
Transition blink3ToBlinkOff_time(blinkOff, 5000);		    // Go to blinkOff state if active time > 5000 ms

Transition blink1ToBlinkOff1(blinkOff, xPrevButton);
Transition blink2ToBlink1(blink1, xPrevButton);
Transition blink3ToBlink2(blink2, xPrevButton);
Transition blinkOffToBlink3(blink3, xPrevButton);     // xNextButton is a callback function

Transition blink1ToBlinkOff2(blinkOff, xResetButton);
Transition blink2ToBlinkOff(blinkOff, xResetButton);
Transition blink3ToBlinkOff2(blinkOff, xResetButton);



// Blink led. Frequency depends of selected state
void blink() {
	static bool level = LOW;
	static uint32_t bTime;
	if (millis() - bTime >= blinkInterval ) {
		bTime = millis();
		level = !level;
		digitalWrite(LED_BLINK, level);
	}
}

/////////// STATE MACHINE FUNCTIONS //////////////////

// Define "on leaving" callback functions
void onLeaving(){
 	Serial.print(F("Leaving state "));
	Serial.println(myFSM.getActiveStateName());
}

// Define "on entering" callback functions (just a message in this example)
void onEntering(){
	Serial.print(F("Entered state "));
	Serial.println(myFSM.getActiveStateName());

	blinkInterval = blinkSetTime[myFSM.getCurrentState()->getIndex()];
	Serial.print(F("Blink time: "));
	Serial.println(blinkInterval);
}


// Definition of the model of the finite state machine and start execution
void setupStateMachine(){
	// Create some states and assign name and callback functions
	myFSM.addState(blinkOff);
	myFSM.addState(blink1);
	myFSM.addState(blink2);
	myFSM.addState(blink3);

	// Add transitions to target state and trigger condition (callback function or bool var)
	blinkOff.addTransition(blinkOffToBlink1);
	blink1.addTransition(blink1ToBlink2);
	blink2.addTransition(blink2ToBlink3);
	blink3.addTransition(blink3ToBlinkOff);
	blink3.addTransition(blink3ToBlinkOff_time);	// This transition is on state timeout

	blink1.addTransition(blink1ToBlinkOff1);
	blink2.addTransition(blink2ToBlink1);
	blink3.addTransition(blink3ToBlink2);
	blinkOff.addTransition(blinkOffToBlink3);

	blink1.addTransition(blink1ToBlinkOff2);
	blink2.addTransition(blink2ToBlinkOff);
	blink3.addTransition(blink3ToBlinkOff2);

	// Start the Machine State
	myFSM.setInitialState(&blinkOff);
	myFSM.start();

	Serial.print(F("Active state: "));
	Serial.println(myFSM.getActiveStateName());
	Serial.println();
}


void setup() {
	pinMode(NEXT_BUTTON, INPUT_PULLUP);
	pinMode(PREV_BUTTON, INPUT_PULLUP);
	pinMode(RESET_BUTTON, INPUT_PULLUP);
	pinMode(LED_BLINK, OUTPUT);

	delay(1000);

	Serial.begin(115200);
	Serial.println(F("\n\nStarting State Machine...\n"));
	setupStateMachine();

	// Set initial blink time
	blinkInterval = blinkSetTime[myFSM.getCurrentState()->getIndex()];
}


void loop() {

	// Update State Machine	(true is state changed)
	if (myFSM.execute()) {
		Serial.print(F("Active state: "));
		Serial.println(myFSM.getActiveStateName());
		Serial.println();
	}

	// If blinkInterval greater than 0, let's blink the led
	if (blinkInterval > 0) {
		blink();
	}
	else {
		digitalWrite(LED_BLINK, LOW);
		delay(1);
	}

}
