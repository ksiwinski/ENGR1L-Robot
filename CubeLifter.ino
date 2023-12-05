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

//Creates stepper motor objects, 4 sets the motors as HALFSTEP, the next 4 are the pins (order IN1, IN3, IN2, IN4)
AccelStepper intakeStepper(4, 0, 2, 1, 3);
AccelStepper rightStepper(4, 4, 6, 5, 7);
AccelStepper leftStepper(4, 8, 10, 9, 11);

//Runs once
void setup() {  
  Serial.begin(1200);
  Serial.println("STARTING");
  //Sets speeds and acceleration for stepper motors
  leftStepper.setMaxSpeed(1000.0);
  leftStepper.setAcceleration(50.0);
  leftStepper.setSpeed(200);

  rightStepper.setMaxSpeed(1000.0);
  rightStepper.setAcceleration(50.0);
  rightStepper.setSpeed(200);

  intakeStepper.setMaxSpeed(1000.0);
  intakeStepper.setAcceleration(50.0);
  intakeStepper.setSpeed(400);

  //Sets pins as inout/output
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

//Moves a servo slowly
void goSlow(Servo servo, int curPos, int newPos){
  if(curPos < newPos){
    for(int i = curPos; i < newPos; i++){
      servo.write(i);
      delay(5);
    }
  }
  else{
    for(int i = curPos; i > newPos; i--){
      servo.write(i);
      delay(5);
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
  if(buttonState == HIGH && buttonPressed < 3){
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

  //When the button is pressed for the second time the robot plays a noise then runs its main operations
  if(buttonPressed >= 3){
    Serial.println("Begin");
    tone(buzzer, 1000);
    delay(500);
    noTone(buzzer);
    delay(500);
    rightStepper.setAcceleration(50.0);
    rightStepper.setSpeed(200);
    rightStepper.setMaxSpeed(1000);
    rightStepper.move(2038);
    // drive(25);
    run();
    while(leftStepper.distanceToGo()!=0 || rightStepper.distanceToGo()!=0){
      Serial.println("Right:" + rightStepper.distanceToGo());
      delay(500);
    }
    run();
    delay(100000);
    turn(45);
    drive(10);
    while(leftStepper.distanceToGo()!=0 || rightStepper.distanceToGo()!=0){
      run();
    }
    turn(-45);
    drive(10);
    while(leftStepper.distanceToGo()!=0 || rightStepper.distanceToGo()!=0){
      run();
    }
    turn(-45);
    drive(10);
    while(leftStepper.distanceToGo()!=0 || rightStepper.distanceToGo()!=0){
      run();
    }
    turn(45);
    drive(10);
    while(leftStepper.distanceToGo()!=0 || rightStepper.distanceToGo()!=0){
      run();
    }
    open();
    drive(5);
    leftStepper.run();
    rightStepper.run();
  }
}
