//Libraries Needed
#include <AccelStepper.h>
#include <Servo.h>

//Positions for the lifitng servo
#define LIFT 150
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
  Serial.begin(1200);
  Serial.println("STARTING");
  //Sets speeds and accelerqation for stepper motors
  leftStepper.setMaxSpeed(1000.0);
	leftStepper.setAcceleration(50.0);
	leftStepper.setSpeed(1000);

  rightStepper.setMaxSpeed(1000.0);
  rightStepper.setAcceleration(500.0);
	rightStepper.setSpeed(1000);

  intakeStepper.setMaxSpeed(1000.0);
  intakeStepper.setAcceleration(50.0);
	intakeStepper.setSpeed(1000);

  pinMode(buzzer, OUTPUT);
  pinMode(button, INPUT);
  pinMode(photoresistor, INPUT);
  pinMode(laser, OUTPUT);
}

//Turns intake on
void intake(){
  intakeStepper.move(2038);
  intakeStepper.run();
  return;
}

//Turns at a specified angle in degrees (testing needed)
void turn(int angle){
  leftStepper.move(int((sin((3.1415/180)*angle))*-2038));
  rightStepper.move(int(sin((3.1415/180)*angle)*-2038));
  return;
}

//Drives a specified distance in centimeters (testing needed)
void drive(int distance){
  leftStepper.move(int((distance)/(2*3.1415*6.7))*-2038);
  rightStepper.move(int((distance)/(2*3.1415*6.7))*2038);
  return;
}

//Opens releases boxes
void open(){
  goSlow(openServo, 0, 90);
  goSlow(liftServo, BASE, TRIGGER);
  delay(100);
  return;
}

void goSlow(Servo servo, int curPos, int newPos){
  if(curPos < newPos){
    for(int i = curPos; i < newPos; i++){
      servo.write(i);
      delay(10);
    }
  }
  else{
    for(int i = curPos; i > newPos; i--){
      servo.write(i);
      delay(10);
    }
  }
}

//Lifts a box
void lift(){
  goSlow(liftServo, BASE, LIFT);
  delay(500);
  goSlow(liftServo, LIFT, BASE);
  return;
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
//Runs forever
void loop() {
  int buttonState = digitalRead(button);
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
    openServo.write(1);
    liftServo.attach(servo1);
    openServo.attach(servo2);
    digitalWrite(laser, HIGH);
    buttonPressed ++;
  }

  if(buttonPressed == 3){
    Serial.println("Begin");
    tone(buzzer, 1000);
    delay(500);
    noTone(buzzer);
    delay(500);
    lift();
    delay(1000);
    open();
    delay(1000);
    drive(50);
    while (leftStepper.distanceToGo() != 0 || rightStepper.distanceToGo() != 0){
      leftStepper.run();
      rightStepper.run();
    }
    buttonPressed++;
  }
}
