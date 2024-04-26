#include <Arduino.h>

/* Example sketch to control a 28BYJ-48 stepper motor with
  ULN2003 driver board, AccelStepper and Arduino UNO
  More info: https://www.makerguides.com */

#include "AccelStepper.h"

// Motor pin definitions:
#define motorPin1 8  // IN1 on the ULN2003 driver
#define motorPin2 9  // IN2 on the ULN2003 driver
#define motorPin3 10 // IN3 on the ULN2003 driver
#define motorPin4 11 // IN4 on the ULN2003 driver

#define homeSensor 2 // Home sensor switch

// Define the AccelStepper interface type; 4 wire motor in half step mode:
#define MotorInterfaceType 8

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper library with 28BYJ-48 stepper motor:
AccelStepper stepper = AccelStepper(AccelStepper::FULL4WIRE, motorPin1, motorPin3, motorPin2, motorPin4, false);

const String letters[] = {" ", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "+", "-", "/", "*", "+-", ":", "{", "}"};

void homeStepperMotor()
{
  Serial.println("Homing stepper motor");
  while (digitalRead(homeSensor))
  {
    stepper.setSpeed(500);
    stepper.runSpeed();
  }
  Serial.println("Stepper motor homed");

  // Prompt on serial to ask what character is on home position
  Serial.println("Please enter the character on the home position:");
  while (Serial.available() == 0)
  {
    // Wait for user input
  }
  char homeCharacter = Serial.read();
  Serial.print("Character on home position: ");
  Serial.println(homeCharacter);
}

void setup()
{

  pinMode(homeSensor, INPUT_PULLUP);

  Serial.begin(9600);

  // Set the maximum steps per second:
  stepper.setMaxSpeed(5000);

  // Set the acceleration factor:
  stepper.setAcceleration(5000);

  // Home the stepper motor:
  homeStepperMotor();
}

void loop()
{
  if (Serial.available() > 0)
  {
    String command = Serial.readString();
    if (command == "Home")
    {
      Serial.println("Received home command");
      homeStepperMotor();
    }
    else if (command == "^")
    {
      Serial.println("Moving stepper motor 1 revolution forward");

      // Set the current position to 0:
      stepper.setCurrentPosition(0);

      // Run the motor forward at 500 steps/second until the motor reaches 4096 steps (1 revolution):
      while (stepper.currentPosition() != 2048)
      {
        stepper.setSpeed(250);
        stepper.runSpeed();
      }
      delay(1000);
    }
    else
    {
      Serial.print("Displaying character: ");
      Serial.println(command);
      // Convert the command to uppercase
      command.toUpperCase();

      // Find the index of the character in the letters array
      int index = -1;
      for (int i = 0; i < sizeof(letters) / sizeof(letters[0]); i++)
      {
        if (command == letters[i])
        {
          index = i;
          break;
        }
      }

      // If the character is found, move the stepper motor to the corresponding position
      if (index != -1)
      {
        // Calculate the number of steps to move
        int steps = index * 2;
        Serial.println(steps);

        // Move the stepper motor to the desired position
        stepper.setCurrentPosition(0);
        stepper.moveTo(steps);

        // Run the motor until it reaches the desired position
        while (stepper.distanceToGo() != 0)
        {
          stepper.run();
        }

        Serial.println("Character displayed");
      }
      else
      {
        Serial.println("Invalid character");
      }
    }
  }
}
