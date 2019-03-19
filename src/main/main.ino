// Include the library code for LCD:
#include <LiquidCrystal.h>

// 1. INITIAL SETUP AND CONSTANTS

// LCD pins setup
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Reusable button class with utility methods
class Button {
  public:
    int pin;
    int button_state = LOW;
    bool released = false;

    // Simple constructor - only button pin has to be provided
    Button(int button_pin) {
      pin = button_pin;
    };

    // Method updating state of pressing and releasing the button
    void updateButtonState() {
      int prev_state = button_state;
      button_state = digitalRead(pin);

      if (button_state == LOW && prev_state == HIGH) { // button release
        released = true;
        lcd.clear();
        lcd.print(pin);
        delay(500);
      } else {
        released = false;
      }
    };
};

// // LCD pins setup
// const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Stop signal and light
const int stop_signal_pin = 32;
const int light_pin = 33;

// Lights enum
enum LightState {
  OFF = 0,
  ON = 1
};

// DC motor pins
const int8_t motor_enable_pin = 9;
const int8_t motor_direction_forward_pin = 8;
const int8_t motor_direction_backward_pin = 7;

// Directions
enum Direction {
  BACKWARD = 0,
  FORWARD = 1
};

const int8_t DirectionMapping[] = {motor_direction_forward_pin, motor_direction_backward_pin};

// State machine
enum States {
  STOP = 0,
  START = 1,
  FORWARD_IDLE = 2,
  FORWARD_DRIVING = 3,
  BACKWARD_IDLE = 4,
  BACKWARD_DRIVING = 5,
  BRAKING = 6
};

char StateMap[][10] = {
  "STOP",
  "START",
  "FORWARD",
  "FORW. ->",
  "BACKWARD",
  "BACK. <-",
  "BRAKING"
};

// Buttons declaration and initialization
Button accelerator_button(28);
Button brake_button(27);
Button direction_button(26);
Button startstop_button(25);
Button changegear_button(22);

// Car settings
int state;
int car_speed = 0;
const int car_acceleration_step = 10;
const int car_slow_down_step = 3;
int car_direction = FORWARD;
int car_gear = 2;

// Car gears
const int gearLimits[] = {50, 150, 255};

// 2. MAIN APPLICATION

void setup() 
{
  // Set buttons and lights
  pinMode(accelerator_button.pin, INPUT);
  pinMode(startstop_button.pin, INPUT);
  pinMode(brake_button.pin, INPUT);
  pinMode(direction_button.pin, INPUT);

  pinMode(stop_signal_pin, OUTPUT);
  pinMode(light_pin, OUTPUT);

  // Set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // Begin with STOP state (car is not running)
  state = STOP;
}

void loop() 
{
  // Actions which happen independently from state
  delay(50);
  printInfo(); // displays information about state and speed on LCD screen
  updateButtonStates(); // updates data about buttons
  
  // Check gear changes
  if (changegear_button.released)
    changeGear();

  // Adjusts speed of motor
  analogWrite(motor_enable_pin, car_speed);
  
  // Gradually decreases car speed so to maintain speed
  // or increase it, driver has to keep pressing accelerator_button
  decreaseCarSpeed(car_slow_down_step);

  // Checking if car stopped here (instead of almost in every state)
  if (startstop_button.released && state != STOP) {
    state = STOP;
    startstop_button.released = false;
  }

  switch(state) {
    case STOP:
      switchCarLights(stop_signal_pin, OFF);
      switchCarLights(light_pin, OFF);
      if (startstop_button.released)
        state = START;
      break;

    case START:
      printEngineStart();
      switchCarLights(light_pin, ON);
      car_gear = 2;
      if (car_direction == FORWARD)
        state = FORWARD_IDLE;
      else
        state = BACKWARD_IDLE;
      break;

    case FORWARD_IDLE:
        digitalWrite(DirectionMapping[FORWARD], HIGH);
        digitalWrite(DirectionMapping[BACKWARD], LOW);
      if (car_speed > 0)
        state = FORWARD_DRIVING;
      if (direction_button.released)
        state = BACKWARD_IDLE;
      else if (accelerator_button.button_state == HIGH)
        increaseCarSpeed(car_acceleration_step);
      if (brake_button.released)
        state = BRAKING;
        break;

    case BACKWARD_IDLE:
        digitalWrite(DirectionMapping[BACKWARD], HIGH);
        digitalWrite(DirectionMapping[FORWARD], LOW);
      if (car_speed > 0)
        state = BACKWARD_DRIVING;
      if (direction_button.released)
        state = FORWARD_IDLE;
      else if (accelerator_button.button_state == HIGH)
        increaseCarSpeed(car_acceleration_step);
      if (brake_button.released)
        state = BRAKING;
        break;

    case FORWARD_DRIVING:
      if (car_speed == 0)
        state = FORWARD_IDLE;
      else if (accelerator_button.button_state == HIGH)
        increaseCarSpeed(car_acceleration_step);
      if (brake_button.released)
        state = BRAKING;
        break;

    case BACKWARD_DRIVING:
      if (car_speed == 0)
        state = FORWARD_IDLE;
      else if (accelerator_button.button_state == HIGH)
        increaseCarSpeed(car_acceleration_step);
      if (brake_button.released)
        state = BRAKING;
        break;

    case BRAKING:
      switchCarLights(stop_signal_pin, HIGH);
      car_speed = 0;
      if (brake_button.released) {
        switchCarLights(stop_signal_pin, OFF);
        if (car_direction == FORWARD)
          state = FORWARD_IDLE;
        else
          state = BACKWARD_IDLE;
      }
      break;

    default:
      break;
  }
}

// 3. UTILITY FUNCTIONS

// Function for increasing car speed by declared step
int increaseCarSpeed(int step) {
  car_speed += step;
  if (car_speed > gearLimits[car_gear]) car_speed = gearLimits[car_gear];
};

// Function for slowing down the car by declared step
int decreaseCarSpeed(int step) {
  car_speed -= step;
  if (car_speed < 0) car_speed = 0;
};

// Function handling updating info on LCD screen
void printInfo() {
  lcd.clear(); // clear screen

  lcd.setCursor(0, 0); // set cursor to top left
  String state_message = String("State: ") + String(StateMap[state]);
  lcd.print(state_message);

  if (state != STOP) {
    lcd.setCursor(0, 1); // set cursor to bottom left
    String speed_message = String("Speed: ") + String(car_speed);
    lcd.print(speed_message);

    printGear();
  }
};

// Function switching on or off diode of specific pin
void switchCarLights(int light_pin, int light_state) {
  if(light_state == ON)
    digitalWrite(light_pin, HIGH);
  if(light_state == OFF)
    digitalWrite(light_pin, LOW);
  else
    Serial.print("Wrong light_state argument!");
};

// Function updating state of all used button
void updateButtonStates() {
  accelerator_button.updateButtonState();
  brake_button.updateButtonState();
  direction_button.updateButtonState();
  startstop_button.updateButtonState();
};

// Printing function for starting the engine
void printEngineStart() {
  lcd.clear(); // clear screen

  lcd.setCursor(0, 1); // set cursor to top left
  lcd.print("Engine start");
  delay(1000);
  lcd.print(".");
  delay(1000);
  lcd.print(".");
  delay(1000);
  lcd.print(".");
  delay(1000);
}

// Lock LCD sign
byte lock[8] = {
  B00000,
  B01110,
  B10001,
  B10001,
  B11111,
  B11111,
  B11111,
};

// Change gear
void changeGear() {
  lcd.clear();
  car_gear = (car_gear + 1) % 2;
}

// Printing function for displaying chosen gear
void printGear() {
  lcd.createChar(0, lock);
  lcd.setCursor(12, 1);
  lcd.write(byte(0));
  lcd.print(gearLimits[car_gear]);
}