#include "AD524X.h"

#define LED_D3 10
#define LED_D5 11
#define LED_D6 12
#define INTERCEPT 16
#define RDAC_SHDN 20

#define AXIS_X_IN 26 // ADC0
#define AXIS_Y_IN 27 // ADC1
#define AXIS_C_IN 28 // ADC2
#define AXIS_X_OUT 1
#define AXIS_Y_OUT 0

// 3.3V = voltage reference
// 1.007f = +0.7% (measurements were reading too low)
// (10+33)/10 = resistive division ratio
#define AXIS_IN_LSB_TO_V (3.3f * 1.007f/4095.0f * (10.0f + 33.0f) / (10.0f))

AD524X AD01(0b0101100);

void setup()
{
  delay(1000);
  analogReadResolution(12);

  pinMode(LED_D3, OUTPUT); // LED D3
  pinMode(LED_D5, OUTPUT); // LED D5
  pinMode(LED_D6, OUTPUT); // LED D6
  pinMode(INTERCEPT, OUTPUT);

  pinMode(RDAC_SHDN, OUTPUT);
  digitalWrite(RDAC_SHDN, HIGH);

  Serial.begin(115200);
  Serial.println();
  Serial.println(__FILE__);
  Serial.print("AD524X_LIB_VERSION: ");
  Serial.println(AD524X_LIB_VERSION);

  Wire.begin();
  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.setClock(400000);

  bool b = AD01.begin();
  Serial.println(b ? "true" : "false");
  Serial.println(AD01.isConnected());
}

int ticks = 0;

void loop()
{
  uint16_t inX = analogRead(AXIS_X_IN);
  uint16_t inY = analogRead(AXIS_Y_IN);
  uint16_t inC = analogRead(AXIS_C_IN);

  ticks ++;
  if (ticks == 2) {
    digitalWrite(INTERCEPT, HIGH);
  } else if (ticks >= 4) {
    digitalWrite(INTERCEPT, LOW);
    ticks = 0;
  }

  float actualVM = inC * AXIS_IN_LSB_TO_V * 2;
  float desiredXout = inX * AXIS_IN_LSB_TO_V / (actualVM / 12.0f);
  float desiredYout = inY * AXIS_IN_LSB_TO_V / (actualVM / 12.0f);

  // https://www.wolframalpha.com/input?i=%2833+%2B+20*x+%2F255%29+%2F++%2833+%2B+20+%2B+33%29+*12+%3D+y%2C+solve+for+x
  // (copy and paste into browser, Arduino IDE doesn't open this link properly)
  int outX = constrain(17/8.0* (43 * desiredXout - 198) + 16, 0, 255);
  int outY = constrain(17/8.0* (43 * desiredYout - 198) + 16, 0, 255);
  
  AD01.write(AXIS_X_OUT, outX);
  AD01.write(AXIS_Y_OUT, outY);


  Serial.print("[In] X: ");
  Serial.print(inX * AXIS_IN_LSB_TO_V);x
  Serial.print("V ");
  Serial.print(inX);

  Serial.print(", Y: ");
  Serial.print(inY * AXIS_IN_LSB_TO_V);
  Serial.print("V ");
  Serial.print(inY);

  Serial.print(", C: ");
  Serial.print(inC * AXIS_IN_LSB_TO_V);
  Serial.print("V ");
  Serial.print(inC);

  Serial.print(" [Out] X: ");
  Serial.print(desiredXout);
  Serial.print("V ");
  Serial.print(outX);

  Serial.print(", Y: ");
  Serial.print(desiredYout);
  Serial.print("V ");
  Serial.print(outY);

  Serial.println();
  delay(50);

  // digitalWrite(INTERCEPT, HIGH);
  //   AD01.write(0, 128);
  //   AD01.write(1, 128);
  //   delay(5000);
    
  // digitalWrite(INTERCEPT, LOW);
  // for (int val = 0; val < 255; val+=16)
  // {
  //   AD01.write(0, val);
  //   AD01.write(1, val);
  //   Serial.println(val);
  //   delay(1000);
  // }
}

