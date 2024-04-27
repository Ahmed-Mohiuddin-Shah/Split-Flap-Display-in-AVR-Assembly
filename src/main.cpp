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
char homeCharacter = ' ';

void homeStepperMotor()
{
  Serial.println("Homing stepper motor");

  // Set the current position to 0:
  stepper.setCurrentPosition(0);

  // Run the motor forward at 500 steps/second until the motor reaches 4096 steps (1 revolution):
  while (stepper.currentPosition() != 200)
  {
    stepper.setSpeed(250);
    stepper.runSpeed();
  }

  while (digitalRead(homeSensor))
  {
    stepper.setSpeed(100);
    stepper.runSpeed();
  }

  stepper.setCurrentPosition(0);
  while (stepper.currentPosition() != 10)
  {
    stepper.setSpeed(250);
    stepper.runSpeed();
  }

  Serial.println("Stepper motor homed");

  // Prompt on serial to ask what character is on home position
  Serial.println("Please enter the character on the home position:");
  // while (Serial.available() == 0)
  // {
  //   // Wait for user input
  // }
  // homeCharacter = Serial.read();
  homeCharacter = '7';
  Serial.print("Character on home position: ");
  Serial.println(homeCharacter);
}

void homeSensorInterrupt()
{
  // Check if the home sensor is triggered
  if (digitalRead(homeSensor) == LOW)
  {
    // Home sensor is passed
    Serial.println("Home sensor passed");
    // Perform the necessary actions when the home sensor is passed
    // ...
  }
}

void setup()
{

  pinMode(homeSensor, INPUT_PULLUP);
  // Attach an interrupt to the home sensor pin
  attachInterrupt(digitalPinToInterrupt(homeSensor), homeSensorInterrupt, CHANGE);

  Serial.begin(9600);

  // Set the maximum steps per second:
  stepper.setMaxSpeed(5000);
  stepper.setSpeed(250);

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
      int currentIndex = 0;
      for (int i = 0; i < sizeof(letters) / sizeof(letters[0]); i++)
      {
        if (command == letters[i])
        {
          index = i;
          break;
        }
      }

      for (int i = 0; i < sizeof(letters) / sizeof(letters[0]); i++)
      {
        if (homeCharacter == letters[i].c_str()[0])
        {
          currentIndex = i;
          break;
        }
      }

      // If the character is found, move the stepper motor to the corresponding position
      if (index != -1 && homeCharacter != command.c_str()[0])
      {
        int distance = 0;

        homeCharacter = command.c_str()[0];
        for (int i = currentIndex;; i++)
        {
          distance++;
          if ((i % 45) == index)
          {
            break;
          }
        }
        // Calculate the number of steps to move
        int steps = distance * 43;
        Serial.println(steps);

        // Move the stepper motor to the desired position
        stepper.setCurrentPosition(0);
        while (stepper.currentPosition() != steps)
        {
          stepper.setSpeed(250);
          stepper.runSpeed();
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
