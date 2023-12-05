//Libraries Needed
#include <AccelStepper.h>
#include <Servo.h>

//Positions for the lifitng servo
#define LIFT 100
#define BASE 30
#define TRIGGER 0

//Sets up pins for Servos
Servo liftServo;
Servo openServo;
int servo1 = A4;
int servo2 = A5;

//All Other Pins
int buzzer = A3;
int button = 12;
int photoresistor = A0;
int laser = 13;

//Creates stepper motor objects
AccelStepper intakeStepper(4, 0, 2, 1, 3);
AccelStepper rightStepper(4, 4, 6, 5, 7);
AccelStepper leftStepper(4, 8, 10, 9, 11);

//Runs once
void setup() {  
  Serial.begin(9600); //Baud Rate changed
  Serial.println("STARTING");
  
  //Sets speeds and accelerqation for stepper motors
  leftStepper.setMaxSpeed(1000.0);
	leftStepper.setAcceleration(50.0);
	leftStepper.setSpeed(500);

  rightStepper.setMaxSpeed(1000.0);
  rightStepper.setAcceleration(500.0);
	rightStepper.setSpeed(500);

  intakeStepper.setMaxSpeed(1000.0);
  intakeStepper.setAcceleration(50.0);
	intakeStepper.setSpeed(500);

  pinMode(buzzer, OUTPUT);
  pinMode(button, INPUT);
  pinMode(photoresistor, INPUT);
  pinMode(laser, OUTPUT);

   // Attach servo pins
  liftServo.attach(servo1);
  openServo.attach(servo2);
}

//Turns intake on
void intake(){
  intakeStepper.move(203800);
  return;
}

//Turns at a specified angle in degrees (testing needed)
//void turn(int angle){
//  leftStepper.move(int((sin((3.1415/180)*angle))*-2038));
//  rightStepper.move(int(sin((3.1415/180)*angle)*-2038));
//  return;
//}

//Drives a specified distance in centimeters (testing needed)
void drive(int distance){
  int rotation = int((distance)/(2*3.1415*6.7))*-2038
  leftStepper.move(-1*rotation);
  rightStepper.move(rotation);
}

//Opens releases boxes
void open(){
  goSlow(openServo, 30, 90, 5);
  goSlow(liftServo, BASE, TRIGGER, 5);
  delay(100);
  
}

void goSlow(Servo servo, int curPos, int newPos, int delayTime){
 // if(curPos < newPos){
      // for(int i = curPos; i < newPos; i++){
   int step = (newPos > curPos) ? 1 : -1;
     for(int i = curPos; i != newPos; i += step){
      servo.write(i);
      delay(delayTime);
    }
  }
 // else{
 //   for(int i = curPos; i > newPos; i--){
  //    servo.write(i);
  //    delay(100);
  //  }
//  }
//}

//Lifts a box
void lift(){
  goSlow(liftServo, BASE, LIFT, 10);
  delay(500);
  goSlow(liftServo, LIFT, BASE, 10);
 
}

//Checks if the beambreak has been triggered, if so it lifts
void check(){
  int reading = analogRead(photoresistor);
  if(reading < 500){
    lift();
  }
}

//Loop that contains the necessary runnings of the robot
void run(){
  leftStepper.run();
  rightStepper.run();
  intake();
  check();
}

int buttonPressed = 0;

//loop that runs forever
void loop() {
  int buttonState = digitalRead(button); //tells us if button works through serial monitor
  Serial.print("Button State: ");
  Serial.println(buttonState);
  Serial.println(buttonPressed);
  //Counter for button pressed
  if(buttonState != LOW && buttonPressed < 3){
    buttonPressed++;
    delay(100);
  }

    //When the button is pressed for the first time the robot "resets" and plays a noise
  if(buttonPressed == 1){
    tone(buzzer, 500);
    delay(200);
    tone(buzzer, 1000);
    delay(300);
    noTone(buzzer);
    delay(500);
    liftServo.write(BASE);
    openServo.write(0);
    digitalWrite(laser, HIGH);
    buttonPressed ++;
  }

  if(buttonPressed == 3){
    Serial.println("Begin"); //Starts running all the steppers
    tone(buzzer, 1000);
    delay(500);
    noTone(buzzer);
    delay(500);
    drive(50);
    intake();
    while (leftStepper.distanceToGo() != 0 || rightStepper.distanceToGo() != 0){
      leftStepper.run();
      rightStepper.run();
      intakeStepper.run();	
    }
    check();
    delay(5000);
    open();
    delay(1000);
    buttonPressed++;
  }
}
