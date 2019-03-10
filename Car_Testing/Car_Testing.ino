//this program is written to test all the components of the circut
//also, you can see here how input/output is set

// include the library code for LCD:
#include <LiquidCrystal.h>

// DC motor pins
const int8_t g_enable_pin = 9;
const int8_t g_direction_a_pin = 8;
const int8_t g_direction_b_pin = 7;
#include <LiquidCrystal.h>

// LCD pins setup
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//stop signal and light
const int stop_signal_pin = 32;
const int light_pin = 33;


//buttons(order from white light to stop signal
int button1_pin = 28; int button1_state = 0;
int button2_pin = 27; int button2_state = 0;
int button3_pin = 26; int button3_state = 0;
int button4_pin = 25; int button4_state = 0;
int button5_pin = 24; int button5_state = 0;


void setup() 
{
  //set buttons
  pinMode(button1_pin, INPUT);
  pinMode(button2_pin, INPUT);
  pinMode(button3_pin, INPUT);
  pinMode(button4_pin, INPUT);
  pinMode(button5_pin, INPUT);

  
  //turn on light and stop signal
  pinMode(stop_signal_pin, OUTPUT);
  pinMode(light_pin, OUTPUT);
  digitalWrite(stop_signal_pin, HIGH);
  digitalWrite(light_pin, HIGH);

   //setting parameters for pins to control the motor
    pinMode(g_enable_pin, OUTPUT);
    pinMode(g_direction_a_pin, OUTPUT);
    pinMode(g_direction_b_pin, OUTPUT);
    digitalWrite(g_enable_pin, LOW);
    digitalWrite(g_direction_a_pin, LOW);
    digitalWrite(g_direction_b_pin, LOW); 


    //testing motor
        // rotate in one direction
       int8_t motor_speed = 170; //max 250 speed
       analogWrite(g_enable_pin, motor_speed);
        delay(10);
        digitalWrite(g_direction_a_pin, HIGH);
        delay(1);
        digitalWrite(g_direction_b_pin, LOW);
        delay(1000);
        // rotate in opposite direction
        analogWrite(g_enable_pin, motor_speed);
        delay(10);
        digitalWrite(g_direction_a_pin, LOW);
        delay(1);
        digitalWrite(g_direction_b_pin, HIGH);
        delay(1000);
       //stop motor
        digitalWrite(g_direction_a_pin, LOW);
        delay(10);
        digitalWrite(g_direction_b_pin, LOW);
    //motor tested

    //testing LCD    
      // set up the LCD's number of columns and rows:
      lcd.begin(16, 2);
      // Print a message to the LCD.
      lcd.print("Hello, world!");    
}

void loop() 
{
        //printing running time in second line
          // set the cursor to column 0, line 1
          // (note: line 1 is the second row, since counting begins with 0):
          lcd.setCursor(0, 1);
          // print the number of seconds since reset:
          lcd.print("Runtime: ");
          lcd.print(millis() / 1000);  
    //LCD tested    

    
    //buttons test (button state is indicated at LCD)
      button1_state = digitalRead(button1_pin);
      button2_state = digitalRead(button2_pin);
      button3_state = digitalRead(button3_pin);
      button4_state = digitalRead(button4_pin);
      button5_state = digitalRead(button5_pin);
          //if button is pressed, then write in on LCD
          if(button1_state == HIGH) {lcd.clear(); lcd.setCursor(0, 0); lcd.print("but1 pressed");}
          if(button2_state == HIGH) {lcd.clear(); lcd.setCursor(0, 0); lcd.print("but2 pressed");}
          if(button3_state == HIGH) {lcd.clear(); lcd.setCursor(0, 0); lcd.print("but3 pressed");}
          if(button4_state == HIGH) {lcd.clear(); lcd.setCursor(0, 0); lcd.print("but4 pressed");}
          if(button5_state == HIGH) {lcd.clear(); lcd.setCursor(0, 0); lcd.print("but5 pressed");}
    
}
