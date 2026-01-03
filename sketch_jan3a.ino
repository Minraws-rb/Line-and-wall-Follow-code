// === IR Sensor Pins ==
const int trigFront = 4;
const int echoFront = 11;

const int trigLeft  = 3;
const int echoLeft  = 2;

const int trigRight = 12;
const int echoRight = 13;

const int irLeftPin  = A2; // Left IR
const int irMidPin   = A1; // Middle IR
const int irRightPin = A0; // Right IR

// === Motor Driver Pins ===

 

const int IN1 = 8;   // Right Motor Forward
const int IN2 = 9;   // Right Motor Backward
const int IN3 = 6;   // Left Motor Forward
const int IN4 = 7;   // Left Motor Backward
const int ENA = 5;   // PWM for Right Motor
const int ENB = 10;  // PWM for Left Motor

// === Speed Settings ===
const int baseSpeed = 200;   // Base speed
const int turnSpeed = 0;   // Speed while turning
const int baseSpeed2 = 255;
const int sharpTurnThreshold = 10;
void setup() {
  // Motor pins as output
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // IR sensors as input
  pinMode(irLeftPin, INPUT);
  pinMode(irMidPin, INPUT);
  pinMode(irRightPin, INPUT);

  pinMode(trigFront, OUTPUT); pinMode(echoFront, INPUT);
  pinMode(trigLeft, OUTPUT);  pinMode(echoLeft, INPUT);
  pinMode(trigRight, OUTPUT); pinMode(echoRight, INPUT);

  Serial.begin(9600);
}

void loop() {
  int leftIR  = analogRead(irLeftPin);
  int midIR   = analogRead(irMidPin);
  int rightIR = analogRead(irRightPin);

  // You might need to calibrate this threshold
  int threshold = 500;

  bool leftDetected  = (leftIR  < threshold);  // Black = low value
  bool midDetected   = (midIR   < threshold);
  bool rightDetected = (rightIR < threshold);

  // Logic
  if (midDetected && !leftDetected && !rightDetected) {
    // Perfectly on line
    moveForward(baseSpeed,baseSpeed); 
  } else if (leftDetected && !midDetected && !rightDetected) {
    // Line is to the left
  //  moveForward(baseSpeed, turnSpeed);
  sharp(1);
  } else if (rightDetected && !midDetected && !leftDetected) {
    // Line is to the right
  //  moveForward(turnSpeed, baseSpeed);
  sharp(2);
  } else if (leftDetected && midDetected && !rightDetected) {
    // Slight left
    sharp(1);
   //  moveForward(baseSpeed, turnSpeed);
  } else if (rightDetected && midDetected && !leftDetected) {
    // Slight right
    sharp(2);
     //moveForward(turnSpeed, baseSpeed);
  } else if (leftDetected && rightDetected && !midDetected) {
    // Line break or junction
    //moveForward(baseSpeed, baseSpeed);
     moveForward(turnSpeed, baseSpeed);
  } else if (!leftDetected && !midDetected && !rightDetected) {
    // Lost line
   ultra();
    //``stopMotors();
  } else {
    // Default: go forward
    moveForward(baseSpeed, baseSpeed);
  }

  delay(10);
}

// === Motor Control Functions ===
void moveForward(int leftSpeed, int rightSpeed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, rightSpeed);
  analogWrite(ENB, leftSpeed);
}
void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}
void sharp(int error){
  if(error==1){
    digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, baseSpeed-20);
  analogWrite(ENB, baseSpeed-20);
  }
  else if(error==2){
    digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, baseSpeed-20);
  analogWrite(ENB, baseSpeed-20);
  }
  
}
void ultra(){
int frontDist = getDistance(trigFront, echoFront);
  int leftDist = getDistance(trigLeft, echoLeft);
  int rightDist = getDistance(trigRight, echoRight);
  int total = leftDist + rightDist;
 if ((frontDist < sharpTurnThreshold) ) {
  if(total < 30){  
     if (leftDist > 15 ) {
      slightRight();
    }  
    else if (rightDist > 12 ) {
      slightLeft();
    } 
    else {
      moveForward2();
    }
  }
  else {
 if (leftDist > rightDist) {
      sharpLeft();
    } else {
      sharpRight();
    }
  }
   }
  else {
    // Wall-following behavior
   if (leftDist > 15 ) {
      slightRight();
    }  
    else if (rightDist > 12 ) {
      slightLeft();
    } 
    else {
      moveForward2();
    }
  }

}
int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000); // timeout after 30ms
  int distance = duration * 0.034 / 2;
  return constrain(distance, 0, 200); // safety cap
}
void moveForward2() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, baseSpeed2);
  analogWrite(ENB, baseSpeed2);
}

void slightLeft() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);   // Right forward
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);   // Left forward
  analogWrite(ENA, baseSpeed2);
  analogWrite(ENB, turnSpeed+50);  // Left slower
}

void slightRight() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);   // Right forward
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);   // Left forward
  analogWrite(ENA, turnSpeed+50);   // Right slower
  analogWrite(ENB, baseSpeed2);
}

void sharpRight() {
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);   // Right forward
  digitalWrite(IN3, HIGH);  digitalWrite(IN4, LOW); 
    // Left forward
  analogWrite(ENA, baseSpeed2);
  analogWrite(ENB, baseSpeed2);
}

void sharpLeft() {
  digitalWrite(IN1, HIGH);  digitalWrite(IN2, LOW);  // Right backward
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); 
 // Left backward
  analogWrite(ENA, baseSpeed2);
  analogWrite(ENB, baseSpeed2);
}


