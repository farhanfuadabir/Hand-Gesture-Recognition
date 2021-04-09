#include <Arduino.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

void __read_flex_data();
void __gen_data_string();
void __apply_mean_filter();
void __auto_calibration();
void __apply_median_filter();
void __auto_calibration_median();


#define FLEX_1 12
#define FLEX_2 27
#define FLEX_3 25
#define FLEX_4 32
#define FLEX_5 34

int FLEX_PINS[] = {12, 27, 25, 32, 34};
float flex_data[] = {0, 0, 0, 0, 0};
float calibrated_values[] = {0, 0, 0, 0, 0};

String data;
char buffer[100];

int count = 0;
float ___init_yaw__ = 0;

MPU6050 mpu;

#define INTERRUPT_PIN 23
bool blinkState = false;

bool dmpReady = false;
uint8_t mpuIntStatus;
uint8_t devStatus;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t fifoBuffer[64];

Quaternion q;
VectorInt16 aa;
VectorInt16 aaReal;
VectorInt16 aaWorld;
VectorFloat gravity;
float euler[3];
float ypr[3];

int current_time, future_time, interval = 150;

volatile bool mpuInterrupt = false;

void dmpDataReady() {
  mpuInterrupt = true;
}

void setup() {
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
    Wire.setClock(400000);
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(INTERRUPT_PIN, INPUT);

  Serial.begin(115200);
  while (!Serial);

  // digitalWrite(LED_BUILTIN, HIGH);
  // //__auto_calibration();
  // __auto_calibration_median();
  // digitalWrite(LED_BUILTIN, LOW);
  // delay(1400);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();
  
  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
  Serial.println(F("\nSend any character to begin DMP programming and demo: "));
  while (Serial.available() && Serial.read());
  //while (!Serial.available());
  while (Serial.available() && Serial.read());
  devStatus = mpu.dmpInitialize();

  mpu.setXGyroOffset(124);
  mpu.setYGyroOffset(35);
  mpu.setZGyroOffset(31);
  mpu.setXAccelOffset(-3562);
  mpu.setYAccelOffset(1821);
  mpu.setZAccelOffset(1781);

  if (devStatus == 0) {
    mpu.setDMPEnabled(true);
    Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
    Serial.println(F(")..."));
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();
    Serial.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    Serial.print(F("DMP Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
  }
}

void loop() {
  current_time = millis();
  future_time = current_time + interval;
  if (!dmpReady) return;
  while (!mpuInterrupt && fifoCount < packetSize) {
    if (mpuInterrupt && fifoCount < packetSize) {
      fifoCount = mpu.getFIFOCount();
    }
  }
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();
  fifoCount = mpu.getFIFOCount();
  if ((mpuIntStatus & _BV(MPU6050_INTERRUPT_FIFO_OFLOW_BIT)) || fifoCount >= 1024) {
    mpu.resetFIFO();
    fifoCount = mpu.getFIFOCount();
  } else if (mpuIntStatus & _BV(MPU6050_INTERRUPT_DMP_INT_BIT)) {
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    fifoCount -= packetSize;

    // mpu.dmpGetQuaternion(&q, fifoBuffer);
    // mpu.dmpGetGravity(&gravity, &q);
    // mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    // ypr[0] = map(ypr[0] * 180 / M_PI, -180, 180, 0, 100);
    // ypr[1] = ypr[1] * 180 / M_PI;
    // ypr[2] = ypr[2] * 180 / M_PI;
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetAccel(&aa, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
    mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
  }

  // if(count < 20) {
  //   ___init_yaw__ = ___init_yaw__ + ypr[0];
  //   count++;
  // } else {
  //   digitalWrite(LED_BUILTIN, LOW);
  //   ypr[0] = abs(ypr[0] - ___init_yaw__ / 20);
  // }
  //__apply_mean_filter();
  //__apply_median_filter();
  __gen_data_string();
  Serial.println(data);
  //Serial.println(millis()-current_time);

  while(future_time > millis());
  mpu.resetFIFO();
}

void __gen_data_string() {
  sprintf(buffer, "%d,%d,%d", aaWorld.x / 100, aaWorld.y / 100, aaWorld.z / 100);
  //sprintf(buffer, "%f\t%f\t%f", ypr[0], ypr[1], ypr[2]);
  //sprintf(buffer, "%f,%f,%f,%f,%f,%f,%f,%f", flex_data[0], flex_data[1], flex_data[2], flex_data[3], flex_data[4], ypr[0], ypr[1], ypr[2]);
  //sprintf(buffer, "%f,%f,%f,%f,%f,%d,%d,%d", flex_data[0], flex_data[1], flex_data[2], flex_data[3], flex_data[4], aaWorld.x / 100, aaWorld.y / 100, aaWorld.z / 100);
  data = buffer;
}

void __apply_mean_filter() {
  float sum[] = {0, 0, 0, 0, 0};
  for (int i = 0; i < 10; i++) {
    for(int i = 0; i < 5; i++) {
      sum[i] = sum[i] + analogRead(FLEX_PINS[i]);
    }
    delay(10);
  }
  for(int i = 0; i < 5; i++) {
    flex_data[i] = (sum[i] / 10 - calibrated_values[i]) * 2;
  }
}

void __auto_calibration() {
  float sum[] = {0, 0, 0, 0, 0};
  for (int i = 0; i < 700; i++) {
    for(int i = 0; i < 5; i++) {
      sum[i] = sum[i] + analogRead(FLEX_PINS[i]);
    }
    delay(10);
  }
  for(int i = 0; i < 5; i++) {
    calibrated_values[i] = sum[i] / 700;
  }
}

void __apply_median_filter() {
  int flex[5][7];
  for(int i = 0; i < 5; i++) {
    for(int j = 0; j < 7; j++) {
      flex[i][j] = 0;
    }
  }
  for(int i = 0; i < 7; i++) {
    for(int j = 0; j < 5; j++) {
      flex[j][i] = analogRead(FLEX_PINS[j]);
      for(int k = 0; k < i; k++) {
        for(int l = 1; l < 7; l++) {
          if(flex[j][l] > flex[j][l-1]) {
            int temp = flex[j][l];
            flex[j][l] = flex[j][l-1];
            flex[j][l-1] = temp;
          }
        }
      }
    }
    delay(10);
  }
  for(int i = 0; i < 5; i++) {
    flex_data[i] = (flex[i][4] - calibrated_values[i]) * 2;
  }
}

void __auto_calibration_median() {
  int flex[5][100];
  for(int i = 0; i < 5; i++) {
    for(int j = 0; j < 100; j++) {
      flex[i][j] = 0;
    }
  }
  for(int i = 0; i < 100; i++) {
    for(int j = 0; j < 5; j++) {
      flex[j][i] = analogRead(FLEX_PINS[j]);
      for(int k = 0; k < i; k++) {
        for(int l = 1; l < 100; l++) {
          if(flex[j][l] > flex[j][l-1]) {
            int temp = flex[j][l];
            flex[j][l] = flex[j][l-1];
            flex[j][l-1] = temp;
          }
        }
      }
    }
    delay(70);
  }
  for(int i = 0; i < 5; i++) {
    calibrated_values[i] = flex[i][50];
  }
}