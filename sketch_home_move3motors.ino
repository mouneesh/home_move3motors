#include <AccelStepper.h> // Include the AccelStepper library for controlling stepper motors

// Define pins for Motor 1
#define dirPin1 30    // Direction pin for Motor 1
#define stepPin1 32   // Step pin for Motor 1
#define enPin1 28     // Enable pin for Motor 1

// Define pins for Motor 2
#define dirPin2 40    // Direction pin for Motor 2
#define stepPin2 36   // Step pin for Motor 2
#define enPin2 38     // Enable pin for Motor 2

// Define pins for Motor 3
#define dirPin3 11    // Direction pin for Motor 3
#define stepPin3 12   // Step pin for Motor 3
#define enPin3 10     // Enable pin for Motor 3

// Define hall sensor pins
const int hallPin1 = 18;  // Hall sensor pin for Motor 1
const int hallPin2 = 21;  // Hall sensor pin for Motor 2
const int hallPin3 = 3;   // Hall sensor pin for Motor 3

// Variables for Motor 1
int countHPM1 = 0;        // Hall pulse count for Motor 1
int stepsPerLineM1 = 2123; // Steps per rotation for Motor 1
int homeValueM1 = 6;      // Home position value for Motor 1
int currentPositionM1 = homeValueM1; // Current position of Motor 1

// Variables for Motor 2
int countHPM2 = 0;        // Hall pulse count for Motor 2
int stepsPerLineM2 = 2123; // Steps per rotation for Motor 2
int homeValueM2 = 3;      // Home position value for Motor 2
int currentPositionM2 = homeValueM2; // Current position of Motor 2

// Variables for Motor 3
int countHPM3 = 0;        // Hall pulse count for Motor 3
int stepsPerLineM3 = 1651; // Steps per rotation for Motor 3
int homeValueM3 = 1;      // Home position value for Motor 3
int currentPositionM3 = homeValueM3; // Current position of Motor 3

// Flags to check if each motor is in its home position
bool homepositionM1 = false; 
bool homepositionM2 = false;
bool homepositionM3 = false;

// Initialize stepper motors using the AccelStepper library
AccelStepper stepper1(AccelStepper::DRIVER, stepPin1, dirPin1, enPin1);
AccelStepper stepper2(AccelStepper::DRIVER, stepPin2, dirPin2, enPin2);
AccelStepper stepper3(AccelStepper::DRIVER, stepPin3, dirPin3, enPin3);

// Variables to store serial input
String inputString = "";  // A string to hold the incoming command
boolean stringComplete = false;  // A flag to check if the serial input is complete

bool messagePrinted = false;  // A flag to check if the homing message has been printed

void setup()
{
  // Initialize serial communication at 9600 bits per second
  Serial.begin(9600);
  Serial.println("");
  Serial.println("Enter commands in the format: <motor_number>,<command>,<value>");
  Serial.println("Commands available: CW (Clockwise), CCW (Counter Clockwise)");
  Serial.println("Examples:");
  Serial.println("1,CW,1000  (Motor 1 rotates 1000 steps clockwise)");
  Serial.println("2,CCW,500  (Motor 2 rotates 500 steps counterclockwise)");
  
  // Initialize hall sensor pins with internal pullup resistors
  pinMode(hallPin1, INPUT_PULLUP); 
  pinMode(hallPin2, INPUT_PULLUP); 
  pinMode(hallPin3, INPUT_PULLUP); 
  
  // Initialize motor enable pins
  pinMode(enPin1, OUTPUT);
  pinMode(enPin2, OUTPUT);
  pinMode(enPin3, OUTPUT);

  // Enable stepper drivers (LOW to enable)
  digitalWrite(enPin1, LOW);
  digitalWrite(enPin2, LOW);
  digitalWrite(enPin3, LOW);

  // Attach interrupts for hall sensors to handle homing detection
  attachInterrupt(digitalPinToInterrupt(hallPin1), hallValueChangedH1, FALLING);
  attachInterrupt(digitalPinToInterrupt(hallPin2), hallValueChangedH2, FALLING);
  attachInterrupt(digitalPinToInterrupt(hallPin3), hallValueChangedH3, FALLING);
  
  // Set maximum speed and acceleration for each stepper motor
  stepper1.setMaxSpeed(2000);   
  stepper1.setAcceleration(1000);
  
  stepper2.setMaxSpeed(2000);   
  stepper2.setAcceleration(1000);
  
  stepper3.setMaxSpeed(4000);   
  stepper3.setAcceleration(1000);

  // Reserve memory space for the inputString
  inputString.reserve(200);
}

void loop()
{
  // Check if all motors have reached their home positions
  if (homepositionM1 && homepositionM2 && homepositionM3) 
  {
    stepper1.setCurrentPosition(0);  // Set current position of Motor 1 to zero (home)
    stepper2.setCurrentPosition(0);  // Set current position of Motor 2 to zero (home)
    stepper3.setCurrentPosition(0);  // Set current position of Motor 3 to zero (home)

    if (!messagePrinted) 
    {
      Serial.println("Homing Completed");  // Print homing completion message
      messagePrinted = true;  // Prevent the message from printing repeatedly
    }

    // Check for serial input commands for motor control
    if (Serial.available() > 0) 
    {
      String data = Serial.readStringUntil('\n');  // Read the input string until newline character
      int firstCommaIndex = data.indexOf(',');     // Find the index of the first comma
      int secondCommaIndex = data.indexOf(',', firstCommaIndex + 1);  // Find the second comma

      // Parse motor number, direction, and step count from the input string
      String firstValue = data.substring(0, firstCommaIndex);
      String secondValue = data.substring(firstCommaIndex + 1, secondCommaIndex);
      String thirdValue = data.substring(secondCommaIndex + 1);

      int value1 = firstValue.toInt();  // Motor number
      int value2 = secondValue.toInt();  // Command (CW/CCW)
      int value3 = thirdValue.toInt();  // Number of steps

      Serial.println("Got the Values");  // Confirm that values were received

      Serial.print("Value 1: ");
      Serial.println(value1);
      Serial.print("Value 2: ");
      Serial.println(value2);
      Serial.print("Value 3: ");
      Serial.println(value3);

      // Move motors to the specified positions
      moveToPositionM1(value1);
      moveToPositionM2(value2);
      moveToPositionM3(value3);
    }
    delay(100);  // Delay to reduce loop frequency
  }
  else
  {
    // If motors are not yet in home position, run them towards home
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
  }
}


// Function to move Motor 1 to a specified position
void moveToPositionM1(int valueM1) 
{
  // Check if value is valid
  if (valueM1)
  {
    int testPositionM1 = valueM1;
    // Ensure the position is within bounds
    if (0 <= testPositionM1 && testPositionM1 <= 12)
    {
      if (currentPositionM1 < testPositionM1)  // Move clockwise if needed
      {
        Serial.println("Clockwise");
        int destinationM1 = (testPositionM1 - currentPositionM1) * stepsPerLineM1;
        moveMotorM1(destinationM1);
        currentPositionM1 = testPositionM1;
        delay(2000);
      }
      if (currentPositionM1 > testPositionM1)  // Move anticlockwise if needed
      {
        Serial.println("AntiClockwise");
        int destinationM1 = (currentPositionM1 - testPositionM1) * stepsPerLineM1;
        moveMotorM1(-destinationM1);
        currentPositionM1 = testPositionM1;
        delay(2000);
      }
    }
  }
}

// Similar moveToPosition functions exist for Motor 2 and Motor 3 (not shown for brevity)
// Each one handles the positioning of a different motor

// Move motor 1 by specified steps
void moveMotorM1(int steps_to_move)
{
  stepper1.setMaxSpeed(1500);   
  stepper1.setAcceleration(1000);
  stepper1.setSpeed(500);
  stepper1.moveTo(steps_to_move);
  stepper1.runToPosition();
}

// Similar moveMotor functions exist for Motor 2 and Motor 3

// Homing function for Motor 1
void homePositionM1()
{
  if(homepositionM1 == true && countHPM1 == 0 )
  {    
    stepper1.setSpeed(0);  // Stop motor after reaching home position
    countHPM1 = +1;
  }
}

// Interrupt service routine for Hall sensor of Motor 1
void hallValueChangedH1()
{
  homepositionM1 = true;  // Set Motor 1 homing flag to true
}

// Similar homing and interrupt functions exist for Motor 2 and Motor 3
