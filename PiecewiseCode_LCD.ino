//BIG ASSUMPTIONS

//you can't move back and forth randomly because the arduino has no idea what zone you're in
//instead you have to move all the way from the start to the finish
//then the finish to the start
//then you have to reset the Arduino if you want to go again
//a pushbutton is better than resetting if we have the time (we don't)

//before the program starts we have to have the right offset voltage coming in right away
//I know this is poor coding but I don't have time to fix
//please fix if you have time

//might have to play with tolerance voltage for state changes

#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);        // select the pins used on the LCD panel
unsigned long tepTimer;

const int ANALOGPIN = 5;

const double VOLTAGESCALEFACTOR = 5.0/1023.0;

//the tolerance for what constitutes a state change
//emperically decided
//you might have to play with it
const double STATECHANGETOL = 0.0001;

//the input voltage when we switch from f1 (graph 1) to f2 (graph 2)
const double STATE1TO2 = 0.034081;
//same for f2 to f3
const double STATE2TO3 = 0.14444;
//the threshhold at which we can be know we're going back in
const double STATE3TO4 = 0.05;

//the six states
enum state
{
  goingOutF1,
  goingOutF2,
  goingOutF3,
  goingInF3,
  goingInF2,
  goingInF1
};

const state INITIAL = goingOutF1;

double offsetVoltage;

double currentScaledVoltage;

double currentDisplacement;

state currentState;

void setup() {
  currentState = INITIAL;

  currentDisplacement = 0;

  currentScaledVoltage = 0;
  
  //has to be connected at the start
  //read in and scale input voltage
  offsetVoltage = (double)analogRead(ANALOGPIN) * VOLTAGESCALEFACTOR;
  lcd.begin(16, 2);                       // start the library
}

void loop() {
  //read in and scale input voltage
  //take off the scale factor
  currentScaledVoltage = (double)analogRead(ANALOGPIN) * VOLTAGESCALEFACTOR - offsetVoltage;
  //state changes
  stateChange(currentState, currentScaledVoltage);
  //displacement calculation based on current state
  currentDisplacement = calculateDisplacement(currentState, currentScaledVoltage);
  
  //ADD DEBUG, THEN LATER LCD CODE
  if(millis() - tepTimer > 500){         // output a temperature value per 500ms
             tepTimer = millis();
             // print the results to the lcd
             lcd.print("Distance: ");               
             lcd.print(5);             
             lcd.print("CM");              
     }
  
  //GOOD LUCK

}

double calculateDisplacement(state currentState, double currentScaledVoltage)
{
   switch(currentState)
   {
   //syntax for a switch statement which runs with either condition
    case goingOutF1:
    case goingInF1:
    //F1: see Excel workbook
      return 1475.3*pow(currentScaledVoltage,2) + 8.8141*currentScaledVoltage + 0.0075;
      break;

    case goingOutF2:
    case goingInF2:
      //also see excel workbook for F2
      return -16512*pow(currentScaledVoltage,4) + -5038.9 * pow(currentScaledVoltage,3) - 462.53*pow(currentScaledVoltage,2) + 22.312*currentScaledVoltage + 3.9293;
      break;

    case goingOutF3:
    case goingInF3:
      //F3 is also in the workbook
      return -0.6644*pow(currentScaledVoltage,4) + 49.265 * pow(currentScaledVoltage,3) - 7.339*pow(currentScaledVoltage,2) + 8.8206*currentScaledVoltage + 7.1882;
      break;
    
   }
   return;
}

void stateChange(state &currentState, double currentScaledVoltage)
{
  //whether it should switch between f1 and f2, independent of direction it's being pulled
  bool switchF1F2 = ((currentScaledVoltage - STATE1TO2) > -1*STATECHANGETOL) || ((currentScaledVoltage - STATE1TO2) < STATECHANGETOL);
  //same for f2 and f3
  bool switchF2F3 = ((currentScaledVoltage + STATE2TO3) > -1*STATECHANGETOL) || ((currentScaledVoltage + STATE2TO3) < STATECHANGETOL);
  
  switch(currentState)
  {
    
    
    //state transitions for state 1
    case goingOutF1 :
      if(switchF1F2)
      {
        currentState = goingOutF2;
      }
      break;

    //state transitions for state 2
    case goingOutF2 :
      if(switchF2F3)
      {
        currentState = goingOutF3;

      }
      break;

     //" for state 3
    
    case goingOutF3 :
      if(currentScaledVoltage > STATE3TO4)
      {
        currentState = goingInF3;  
      }
      break;

    case goingInF3 :
      if(switchF2F3)
      {
        currentState = goingInF2;
      }
      break;

    case goingInF2 : 
      if(switchF1F2)
      {
        currentState = goingInF1;
      }
      break;

    //there is no getting out of goingInF1 programmatically
    //you have to reset the Arduino!
    //because there's no other indicator
    
  }
  return;
}
