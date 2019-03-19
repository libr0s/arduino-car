// include the library code for LCD:
#include <LiquidCrystal.h>

class Button {
  public:
    int pin;
    int button_state = LOW;
    bool released = false;

    Button(int button_pin) {
      pin = button_pin;
    };

    void updateButtonState() {
      int prev_state = button_state;
      button_state = digitalRead(pin);

      if (button_state == LOW && prev_state == HIGH) { // button release
        released = true;
      } else {
        released = false;
      }
    };
};

// LCD pins setup
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//stop signal and light
const int stop_signal_pin = 32;
const int light_pin = 33;

// DC motor pins
const int8_t g_enable_pin = 9;
const int8_t g_direction_a_pin = 8;
const int8_t g_direction_b_pin = 7;

// Car settings
enum Direction {
  BACKWARD = 0,
  FORWARD = 1
};

const int8_t DirectionMapping[] = {g_direction_a_pin, g_direction_b_pin};

enum LightState {
  OFF = 0,
  ON = 1
};

int car_speed = 0;
int car_direction = FORWARD;
const int car_acceleration_step = 10;
const int car_slow_down_step = 2;

// state machine

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

int state;

//buttons(order from white light to stop signal
Button accelerator_button(28);
Button brake_button(27);
Button direction_button(26);
Button startstop_button(25);

void setup() 
{
  //set buttons and lights
  pinMode(accelerator_button.pin, INPUT);
  pinMode(startstop_button.pin, INPUT);
  pinMode(brake_button.pin, INPUT);
  pinMode(direction_button.pin, INPUT);

  pinMode(stop_signal_pin, OUTPUT);
  pinMode(light_pin, OUTPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // begin with STOP state (car is not running)
  state = STOP;
}

void loop() 
{
  // actions which happen independently from state
  delay(50);
  printInfo(); // displays information about state and speed on LCD screen
  updateButtonStates(); // updates data about buttons
  
  // adjusts speed of motor
  analogWrite(g_enable_pin, car_speed);
  if (car_speed != 0)
    digitalWrite(DirectionMapping[car_direction], HIGH);
  else
    digitalWrite(DirectionMapping[car_direction], LOW);
  
  // gradually decreases car speed so to maintain speed
  // or increase it, driver has to keep pressing accelerator_button
  decreaseCarSpeed(car_slow_down_step);

  // checking if car stopped here instead of almost in every state
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
      if (car_direction == FORWARD)
        state = FORWARD_IDLE;
      else
        state = BACKWARD_IDLE;
      break;

    case FORWARD_IDLE:
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
        switchCarLights(stop_signal_pin, LOW);
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

int increaseCarSpeed(int step) {
  car_speed += step;
  if (car_speed > 255) car_speed = 255;
};

int decreaseCarSpeed(int step) {
  car_speed -= step;
  if (car_speed < 0) car_speed = 0;
};

void printInfo() {
  lcd.clear(); // clear screen

  lcd.setCursor(0, 0); // set cursor to top left
  String state_message = String("State: ") + String(StateMap[state]);
  lcd.print(state_message);

  if (state != STOP) {
    lcd.setCursor(0, 1); // set cursor to bottom left
    String speed_message = String("Speed: ") + String(car_speed);
    lcd.print(speed_message);
  } 
};

void switchCarLights(int light_pin, int light_state) {
  if(light_state == ON)
    digitalWrite(light_pin, HIGH);
  if(light_state == OFF)
    digitalWrite(light_pin, LOW);
  else
    Serial.print("Wrong light_state argument!");
};

void updateButtonStates() {
  accelerator_button.updateButtonState();
  brake_button.updateButtonState();
  direction_button.updateButtonState();
  startstop_button.updateButtonState();
};

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