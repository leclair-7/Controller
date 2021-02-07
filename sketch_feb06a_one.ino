
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
double distPerClick = 2 * 3.141592653 * .045 / 8400.0;
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
    /*
    Serial.print("Left = ");
    Serial.print(newLeft);
    Serial.print(" Right = ");
    Serial.print(newRight);
    Serial.println();
    */
    if ( newLeft != positionLeft )
    {
      currTimeL = millis();
      timeDeltaL = currTimeL - pastTimeL;
      pastTimeL = currTimeL;
      
      double posDifference = abs(newLeft) - abs(positionLeft);
      posDifference *= distPerClick;
      
      if ( timeDeltaL > 0)
      {
        double secondsPassed = timeDeltaL / 1000.0;       
        /*
        Serial.print("timeDeltaL = ");
        Serial.print(secondsPassed);
        Serial.println(" sec" );
        */
        speedLeft = posDifference / secondsPassed;
        //Serial.print("SpeedLeft (m/s) = ");
        //Serial.println(speedLeft);
      }      
      positionLeft = newLeft;
    }
    if ( newRight != positionRight )
    {
      currTimeR = millis();
      timeDeltaR = currTimeR - pastTimeR;
      pastTimeR = currTimeR;
      
      double posDifference = abs(newRight) - abs(positionRight);
      posDifference *= distPerClick;
      
      if ( timeDeltaR > 0)
      {
        double secondsPassed = timeDeltaR / 1000.0;       
        /*
        Serial.print("timeDeltaR = ");
        Serial.print(secondsPassed);
        Serial.println(" sec" );
        */
        speedRight = posDifference / secondsPassed;
        //Serial.print("SpeedRight (m/s) = ");
        //Serial.println(speedRight);
      }      
      positionRight = newRight;
    }
    
    //printDistance();
  }

  speedDisparity = abs(speedLeft) - abs(speedRight);
  
  // if a character is sent from the serial monitor,
  // reset both back to zero.
  if (Serial.available()) {
    Serial.read();
    Serial.println("Reset both knobs to zero");
    knobLeft.write(0);
    knobRight.write(0);
    printDistance();
  }

  delay(1000);
  motorForward(32);
}

void motorForward(int throttle)  {
 Serial.print("Called MotorForward, speed disp is: ");
 Serial.print(speedDisparity);
 Serial.print(" SpeedRight (m/s) = ");
 Serial.print(speedRight);
 Serial.print(" SpeedLeft (m/s) = ");
 Serial.println(speedLeft);
 
 //back right
 digitalWrite(InA1, HIGH);
 digitalWrite(InB1, LOW); 
 digitalWrite(InA2, HIGH);
 digitalWrite(InB2, LOW);

 int leftThrottle = throttle + leftThrottleDelta;
 int rightThrottle = throttle + rightThrottleDelta;
 
 if ( abs(speedDisparity) > .001)
 {
   if ( speedLeft > speedRight )
   {
     --leftThrottleDelta;
   }
   else if ( speedLeft < speedRight )
   {
     ++leftThrottleDelta;
   }
 }

 leftThrottle = max(leftThrottle,0);
 rightThrottle = max(rightThrottle,0);
 
 analogWrite(PWM1, leftThrottle); 
 analogWrite(PWM2, rightThrottle); 
}


void printDistance()
{
  Serial.print("Distance left: ");
  Serial.print(distTraveled);
  Serial.print(" Distance Right: ");
  Serial.println(distTraveledRight);
}
