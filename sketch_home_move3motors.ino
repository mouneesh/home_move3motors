#include <AccelStepper.h> //accelstepper library

#define dirPin1 30    // Direction pin
#define stepPin1 32   // Pulse/Step pin
#define enPin1 28     // Enable pin

#define dirPin2 40   // Direction pin
#define stepPin2 36   // Pulse/Step pin
#define enPin2 38     // Enable pin

#define dirPin3 11    // Direction pin
#define stepPin3 12   // Pulse/Step pin
#define enPin3 10     // Enable pin

const int hallPin1 = 18;
const int hallPin2 = 21;
const int hallPin3 = 3;

int countHPM1 = 0;
int stepsPerLineM1 = 2123;
int homeValueM1 = 6;
int currentPositionM1 = homeValueM1;

int countHPM2 = 0;
int stepsPerLineM2 = 2123;
int homeValueM2 = 3;
int currentPositionM2 = homeValueM2;

int countHPM3 = 0;
int stepsPerLineM3 = 1651;
int homeValueM3 = 1;
int currentPositionM3 = homeValueM3;

bool homepositionM1 = false; 
bool homepositionM2 = false;
bool homepositionM3 = false;


AccelStepper stepper1(AccelStepper::DRIVER, stepPin1, dirPin1, enPin1);
AccelStepper stepper2(AccelStepper::DRIVER, stepPin2, dirPin2, enPin2);
AccelStepper stepper3(AccelStepper::DRIVER, stepPin3, dirPin3, enPin3);

// Variables to store serial input
String inputString = "";
boolean stringComplete = false;

bool messagePrinted = false;

void setup()
{

    // Initialize serial communication at 9600 bits per second
  Serial.begin(9600);
  Serial.println("");
  Serial.println("Enter commands in the format: <motor_number>,<command>,<value>");
  Serial.println("Commands available: CW, CCW");
  Serial.println("Examples:");
  Serial.println("1,CW,1000");
  Serial.println("2,CCW,500");
  //---------------------------------------------------------------------------

  pinMode(hallPin1, INPUT_PULLUP); // internal pullup resistor (debouncing)
  pinMode(hallPin2, INPUT_PULLUP); // internal pullup resistor (debouncing)
  pinMode(hallPin3, INPUT_PULLUP); // internal pullup resistor (debouncing)
  
    // Set up enable pins
  pinMode(enPin1, OUTPUT);
  pinMode(enPin2, OUTPUT);
  pinMode(enPin3, OUTPUT);

  // Enable stepper drivers (LOW to enable)
  digitalWrite(enPin1, LOW);
  digitalWrite(enPin2, LOW);
  digitalWrite(enPin3, LOW);

  
  attachInterrupt(digitalPinToInterrupt(hallPin1), hallValueChangedH1, FALLING);   //do not change it to 'CHANGE' 
  attachInterrupt(digitalPinToInterrupt(hallPin2), hallValueChangedH2, FALLING);   //do not change it to 'CHANGE' 
  attachInterrupt(digitalPinToInterrupt(hallPin3), hallValueChangedH3, FALLING);   //do not change it to 'CHANGE' 
  
  //---------------------------------------------------------------------------
  stepper1.setMaxSpeed(2000);   
  stepper1.setAcceleration(1000);
  
  

  stepper2.setMaxSpeed(2000);   
  stepper2.setAcceleration(1000);
  
  

  stepper3.setMaxSpeed(4000);   
  stepper3.setAcceleration(1000);
  
  //---------------------------------------------------------------------------

  // Reserve space for the inputString
  inputString.reserve(200);

}

void loop()
{


  if (homepositionM1 && homepositionM2 && homepositionM3) 
  {
    stepper1.setCurrentPosition(0);
    stepper2.setCurrentPosition(0);
    stepper3.setCurrentPosition(0);

    if (!messagePrinted) 
    {
      Serial.println("Homing Completed");
      messagePrinted = true; // Set the flag to true after printing
    }


    if (Serial.available() > 0) 
    {
      String data = Serial.readStringUntil('\n');

      int firstCommaIndex = data.indexOf(',');
      int secondCommaIndex = data.indexOf(',', firstCommaIndex + 1); // Find the second comma

      String firstValue = data.substring(0, firstCommaIndex);
      String secondValue = data.substring(firstCommaIndex + 1, secondCommaIndex);
      String thirdValue = data.substring(secondCommaIndex + 1);

      int value1 = firstValue.toInt();
      int value2 = secondValue.toInt();
      int value3 = thirdValue.toInt();

      Serial.println("Got the Values");

      Serial.print("Value 1: ");
      Serial.println(value1);
      Serial.print("Value 2: ");
      Serial.println(value2);
      Serial.print("Value 3: ");
      Serial.println(value3);

      moveToPositionM1(value1);
      moveToPositionM2(value2);
      moveToPositionM3(value3);
    }
    delay(100);
  }

  else
  {
    if(countHPM1 == 0)
    {
      stepper1.run();
      homePositionM1();
    }
    if(countHPM2 == 0)
    {
      stepper2.run();
      homePositionM2();
    }
    if(countHPM3 == 0)
    {
      stepper3.run();
      homePositionM3();
    }
    if (Serial.available() > 0) 
    {
    // Read the incoming data until newline character
      String data = Serial.readStringUntil('\n');
      data.trim();
      int firstComma = data.indexOf(',');
      int secondComma = data.indexOf(',', firstComma + 1);

      if (firstComma == -1) 
      {
        Serial.println("Invalid command format.");
        return;
      }

      String motorStr = data.substring(0, firstComma);
      int motorNumber = motorStr.toInt();

      if (motorNumber < 1 || motorNumber > 3) 
      {
        Serial.println("Motor number must be 1, 2, or 3.");
        return;
      }

      String commandStr;
      String valueStr;

      if (secondComma == -1) 
      {
      // No second comma found; invalid format
        Serial.println("Invalid command format. Expected: <motor_number>,<command>,<value>");
        return;
      } 
      else 
      {
        commandStr = data.substring(firstComma + 1, secondComma);
        valueStr = data.substring(secondComma + 1);
      }

      commandStr.trim();
      valueStr.trim();

      if (commandStr.equalsIgnoreCase("CW") || commandStr.equalsIgnoreCase("CCW")) 
      {
        int steps = valueStr.toInt();
        if (steps <= 0) 
        {
          Serial.println("Number of steps must be positive.");
          return;
        }

        int directionMultiplier = commandStr.equalsIgnoreCase("CW") ? 1 : -1;

        Serial.print("Moving Motor ");
        Serial.print(motorNumber);
        Serial.print(" ");
        Serial.print(commandStr);
        Serial.print(" by ");
        Serial.print(steps);
        Serial.println(" steps.");

        switch (motorNumber) 
        {
          case 1:
            stepper1.move(steps * directionMultiplier);
            break;
          case 2:
            stepper2.move(steps * directionMultiplier);
            break;
          case 3:
            stepper3.move(steps * directionMultiplier);
            break;
        }
      } 
      else 
      {
        Serial.println("Unknown command. Valid commands are CW and CCW.");
      }
    }
  }
}


void moveToPositionM1(int valueM1) 
{

  if (valueM1)
  {
    int testPositionM1 = valueM1;
    if (0 <= testPositionM1 && testPositionM1 <= 12)
    {
      if (currentPositionM1 < testPositionM1)
      {
        //direction = 0;
        Serial.println("Clockwise");
        Serial.println(currentPositionM1);
        int destinationM1 = testPositionM1 - currentPositionM1;
        destinationM1 = destinationM1 * stepsPerLineM1;
        Serial.println(destinationM1);
        moveMotorM1(destinationM1);
        currentPositionM1 = testPositionM1;
        delay(2000);
      }
      if (currentPositionM1 > testPositionM1)
      {
        //direction = 1;
        Serial.println("AntiClockwise");
        Serial.println(currentPositionM1);
        int destinationM1 = currentPositionM1 - testPositionM1;
        destinationM1 = destinationM1 * stepsPerLineM1;
        Serial.println(destinationM1);
        moveMotorM1(-destinationM1);
        currentPositionM1 = testPositionM1;
        delay(2000);
      }
    }
  }
}


void moveToPositionM2(int valueM2) 
{

  if (valueM2)
  {
    int testPositionM2 = valueM2;
    if (0 <= testPositionM2 && testPositionM2 <= 12)
    {
      if (currentPositionM2 < testPositionM2)
      {
        //direction = 0;
        Serial.println("Clockwise");
        Serial.println(currentPositionM2);
        int destinationM2 = testPositionM2 - currentPositionM2;
        destinationM2 = destinationM2 * stepsPerLineM2;
        Serial.println(destinationM2);
        moveMotorM2(-destinationM2);
        currentPositionM2 = testPositionM2;
        delay(2000);
      }
      if (currentPositionM2 > testPositionM2)
      {
        //direction = 1;
        Serial.println("AntiClockwise");
        Serial.println(currentPositionM2);
        int destinationM2 = currentPositionM2 - testPositionM2;
        destinationM2 = destinationM2 * stepsPerLineM2;
        Serial.println(destinationM2);
        moveMotorM2(destinationM2);
        currentPositionM2 = testPositionM2;
        delay(2000);
      }
    }
  }
}


void moveToPositionM3(int valueM3) 
{

  if (valueM3)
  {
    int testPositionM3 = valueM3;
    if (0 < testPositionM3 && testPositionM3 < 26)
    {
      if (currentPositionM3 < testPositionM3)
      {
        //direction = 0;
        Serial.println("Clockwise");
        Serial.println(currentPositionM3);
        int destinationM3 = testPositionM3 - currentPositionM3;
        destinationM3 = destinationM3 * stepsPerLineM3;
        Serial.println(-destinationM3);
        moveMotorM3(destinationM3);
        currentPositionM3 = testPositionM3;
        delay(2000);
      }
      if (currentPositionM3 > testPositionM3)
      {
        //direction = 1;
        Serial.println("AntiClockwise");
        Serial.println(currentPositionM3);
        int destinationM3 = currentPositionM3 - testPositionM3;
        destinationM3 = destinationM3 * stepsPerLineM3;
        Serial.println(destinationM3);
        moveMotorM3(destinationM3);
        currentPositionM3 = testPositionM3;
        delay(2000);
      }
    }
  }
}

void moveMotorM1(int steps_to_move)
{
  stepper1.setMaxSpeed(1500);   
  stepper1.setAcceleration(1000);
  stepper1.setSpeed(500);
  stepper1.moveTo(steps_to_move);
  stepper1.runToPosition();
}

void moveMotorM2(int steps_to_move)
{
  stepper2.setMaxSpeed(1500);   
  stepper2.setAcceleration(1000);
  stepper2.setSpeed(500);
  stepper2.moveTo(steps_to_move);
  stepper2.runToPosition();
}

void moveMotorM3(int steps_to_move)
{
  stepper3.setMaxSpeed(1500);   
  stepper3.setAcceleration(1000);
  stepper3.setSpeed(500);
  stepper3.moveTo(steps_to_move);
  stepper3.runToPosition();
}

void homePositionM1()
{
  if(homepositionM1 == true && countHPM1 == 0 )
  {    
    stepper1.setSpeed(0);
    countHPM1 = +1;
  }
}

void hallValueChangedH1()
{
  homepositionM1 = true;
}

void homePositionM2()
{
  if(homepositionM2 == true && countHPM2 == 0 )
  {    
    stepper2.setSpeed(0);
    countHPM2 = +1;
  }
}

void hallValueChangedH2()
{
  homepositionM2 = true;
}

void homePositionM3()
{
  if(homepositionM3 == true && countHPM3 == 0 )
  {    
    stepper3.setSpeed(0);
    countHPM3 = +1;
  }
}

void hallValueChangedH3()
{
  homepositionM3 = true;
}