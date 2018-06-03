#include <limits.h>
#include "timer.hpp"

int NUM_MOTORS = 4;
int PIN_ENABLE[] = { 2, 7, 10, 13 };
int PIN_DIR[] = { 3, 6, 9, 11 };
int PIN_STEP[] = { 4, 5, 8, 12 };

long TIMER_INTERVAL = 125L;
long STEPS_PER_REV = 2000L;

unsigned int* MOTOR_TICKS;
unsigned int* MOTOR_REMAINING;

void setup() {

 Serial.begin(9600);
  
  MOTOR_TICKS = new unsigned int[NUM_MOTORS];
  MOTOR_REMAINING = new unsigned int[NUM_MOTORS];
  
  for (int i = 0; i < NUM_MOTORS; i++) {
    MOTOR_TICKS[i] = UINT_MAX;
    MOTOR_REMAINING[i] = 0;
    
    pinMode(PIN_ENABLE[i], OUTPUT);
    pinMode(PIN_DIR[i], OUTPUT);
    pinMode(PIN_STEP[i], OUTPUT);
    
    digitalWrite(PIN_ENABLE[i], HIGH);
    digitalWrite(PIN_DIR[i], LOW);
    digitalWrite(PIN_STEP[i], LOW);
  }

  install_timer(125L);
}

void loop() {
  long t = millis();
  for (int i = 0; i < NUM_MOTORS; i++) {
    float r = t / (400.0 + 50 * i) + (PI / 4) * i;
    float s = (sin(r) + 1.0) / 2.0;
    int ticks = 5 + 100 * s;
    MOTOR_TICKS[i] = ticks;
  }

  delay(10);
}

bool led = false;

ISR(TIMER1_COMPA_vect) {

  for (int i = 0; i < NUM_MOTORS; i++) {
    MOTOR_REMAINING[i]--;
    if (MOTOR_REMAINING[i] == 0) {
      MOTOR_REMAINING[i] = MOTOR_TICKS[i];
      digitalWrite(PIN_STEP[i], HIGH);
      
      led = !led;
      //digitalWrite(13, led);
    }
  }
  
  //for (int i = 0; i < 10; i++) __asm__("nop\n\t");

  for (int i = 0; i < NUM_MOTORS; i++) {
    digitalWrite(PIN_STEP[i], LOW);
  }
}


