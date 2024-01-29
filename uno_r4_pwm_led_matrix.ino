#include <Servo.h>
#include <Arduino_LED_Matrix.h>
ArduinoLEDMatrix matrix;

byte servoPin = 3; //UNO PWM out pin
Servo servo;
int percentPower;
int matrixDisplay;
int x;
int y;
int flag = 0;
int pwmMin = 800;   //Change to minimum PWM
int pwmMax = 2200;  //Change to maximum PWM
int pwmNeutral = (pwmMax + pwmMin) / 2;
int target = pwmNeutral;

byte frame[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

void setup() {
  servo.attach(servoPin);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  delay(2000);
  matrix.begin();
}

void loop() {

  percentPower = map(target, pwmMin, pwmMax, -100, 100);
  matrixDisplay = map(percentPower, -100, 100, 0, 95);

  while (target == pwmNeutral) {
    servo.writeMicroseconds(target);
    Serial.println("Enter PWM");
    memset(frame, 0, sizeof(frame[0][0]) * 8 * 12);
    matrix.renderBitmap(frame, 8, 12);
    int serialInput = Serial.parseInt();
    if (serialInput != 0 && serialInput <= pwmMax && serialInput >= pwmMin) {
      target = serialInput;
    } else if (serialInput > pwmMax || serialInput < pwmMin && serialInput != 0) {
      Serial.println("Invalid Input");
      delay(1000);
    }
  }

  if (target >= pwmNeutral) {
    memset(frame, 0, sizeof(frame[0][0]) * 8 * 12);
    for (y = 4; y < floor(((matrixDisplay) / 12)); y += 1) {
      for (int x = 0; x <= 11; x += 1) {
        frame[y][x] = 1;
        matrix.renderBitmap(frame, 8, 12);
      }
    }
    for (x = 0; x <= matrixDisplay % 12; x += 1) {
      frame[y][x] = 1;
      matrix.renderBitmap(frame, 8, 12);
    }
  }

  else if (target <= pwmNeutral) {
    memset(frame, 0, sizeof(frame[0][0]) * 8 * 12);
    memset(frame, 1, sizeof(frame[0][0]) * 4 * 12);
    int matrixDisplayInv = matrixDisplay - 1;
    for (y = 0; y < floor(((matrixDisplayInv) / 12)); y += 1) {
      for (int x = 0; x <= 11; x += 1) {
        frame[y][x] = 0;
        matrix.renderBitmap(frame, 8, 12);
      }
    }
    for (x = 0; x <= matrixDisplayInv % 12; x += 1) {
      frame[y][x] = 0;
      matrix.renderBitmap(frame, 8, 12);
    }
  }

  percentPower = map(target, pwmMin, pwmMax, -100, 100);
  matrix.renderBitmap(frame, 8, 12);
  servo.writeMicroseconds(target);
  Serial.print("% Power: ");
  Serial.print(percentPower);
  Serial.print(" | pwm: ");
  Serial.println(target);
  digitalWrite(LED_BUILTIN, 1);

  flag = Serial.parseInt();
  if (flag >= pwmMin && flag <= pwmMax) {
    target = flag;
  } else if (flag >= -pwmMin && flag <= pwmMin) {
    target = target + flag;
  } else {
    Serial.println("Invalid Input");
  }
  if (target < pwmMin || target > pwmMax) {
    Serial.println("Out of Range");
    target = 1500;
  }

  delay(10);
}
