
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
Encoder knobLeft(2,3);
Encoder knobRight(18,19);

volatile int encoder0Pos = 0;
int InA1 = 8;
int InB1 = 9;
unsigned int PWM1 = 10;

volatile int encoder1Pos = 0;
int InA2 = 11;
int InB2 = 12;
unsigned int PWM2 = 13;

void setup() { 
  Serial.begin(9600);
}

//encoder position
long positionLeft  = -999;
long positionRight = -999;

//meters
unsigned int wheelRadius = .045;
double pi = 3.141592653;
double rad2deg = 180.0/pi;

double distPerClick = 2 * pi * wheelRadius / 8400.0;

double distTraveled = 0.0;
double distTraveledRight = 0.0;
long newLeft = 0;
long newRight = 0;


double currLeft = 0;
double speedLeft = 0.0;
unsigned long pastTimeL = 0;
unsigned long currTimeL = 0;
unsigned long timeDeltaL = 0;

double currRight = 0;
double speedRight = 0.0;
unsigned long pastTimeR = 0;
unsigned long currTimeR = 0;
unsigned long timeDeltaR = 0;


int hasStarted=0;

//speed difference between left and right motors
double speedDisparity = 0.0;
int leftThrottleDelta = 0;
int rightThrottleDelta = 0;
double posDifference = 0.0;
double rotorPosition = 0.0;
double rotorAngleRad = 0.0;
double rotorAngleDeg = 0.0;

// Unique to https://www.pololu.com/product/4756
double countPerRev = 8400.0;

void loop() {

  if(!hasStarted)
  {
    hasStarted = 1;
    pastTimeL = millis();
    pastTimeR = millis();
  }
  
  newLeft = knobLeft.read();
  newRight = knobRight.read();
  
  distTraveled = newLeft * distPerClick;
  distTraveledRight = newRight * distPerClick;

  if (newLeft != positionLeft || newRight != positionRight) 
  {
    // uncomment to learn about encoder output values
    if ( newLeft != positionLeft )
    {
      currTimeL = millis();
      timeDeltaL = currTimeL - pastTimeL;
      pastTimeL = currTimeL;
      
      posDifference = abs(newLeft) - abs(positionLeft);
      rotorPosition = abs(newLeft)/8400.0;
      rotorAngleRad = rotorPosition * (2.0*pi);
      rotorAngleDeg = (int)(rotorAngleRad * rad2deg) % 360;
      Serial.print("Pos difference (clicks in encoder driven: ");
      Serial.print(posDifference);
      Serial.print(" RotorPosition: ");
      Serial.print(rotorPosition);
      Serial.print(" RotorAngle: ");
      Serial.print(rotorAngleRad);
      Serial.print(" RotorAngleDeg: ");
      Serial.println(rotorAngleDeg);
      /*
       * 1 rev gear == 70 rev of motor
       * say start at angle 0 of the gear
       * 1 rev of motor == 1/(2*pi) * 1/70 radians moved
       */      
      
      if ( timeDeltaL > 0)
      {
        double secondsPassed = timeDeltaL / 1000.0;       
        speedLeft = posDifference / secondsPassed;
      }      
      positionLeft = newLeft;
    }    
  }

  speedDisparity = abs(speedLeft) - abs(speedRight);
  
  // if a character is sent from the serial monitor,
  // reset both back to zero.
  if (Serial.available()) {
    Serial.read();
    Serial.println("Reset both knobs to zero");
    knobLeft.write(0);
    knobRight.write(0);
  }

  delay(10);
  if (rotorPosition < 70)
  {
    motorForward(32);  
  }
  else
  {
    motorStop();
  }
  
}

void motorForward(int throttle)  {
 Serial.print("Called MotorForward, ticks on left moved is: ");
 Serial.print( newLeft );
 //Serial.print(" SpeedLeft (m/s) = ");
 //Serial.println(speedLeft); 
 
 digitalWrite(InA1, HIGH);
 digitalWrite(InB1, LOW);

 analogWrite(PWM1, throttle);  
}

void motorStop() {
  analogWrite(PWM1, 0);  
}
