#include <limits.h>
#include "timer.hpp"

int NUM_MOTORS = 4;
int PIN_ENABLE[] = { 2, 7, 10, 13 };
int PIN_DIR[] = { 3, 6, 9, 11 };
int PIN_STEP[] = { 4, 5, 8, 12 };

long TIMER_INTERVAL = 125L;
long STEPS_PER_REV = 2000L;

unsigned int* motor_ticks;
unsigned int* motor_remaining;

void setup() {

  Serial.begin(9600);
  
  motor_ticks = new unsigned int[NUM_MOTORS];
  motor_remaining = new unsigned int[NUM_MOTORS];
  
  for (int i = 0; i < NUM_MOTORS; i++) {
    motor_ticks[i] = UINT_MAX;
    motor_remaining[i] = 0;
    
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
  set_motor_speed(0, 100);
  delay(10);
}

void set_motor_speed(unsigned char motor, int speed) {
  unsigned int ticks = (unsigned int) ((1000L * 1000000L)/(TIMER_INTERVAL*STEPS_PER_REVOLUTION*abs(speed)));
  motor_ticks[motor] = ticks;
  motor_remaining[motor] = ticks;
    
  #if ENABLE_TRACES
  Serial.println("motor %i = %i rps/1000\r\n", motor, speed);
  Serial.println("N time quanta = %u\r\n", ticks);
  Serial.println("dirmask = 0x%X\r\n", direction);
  #endif
}

ISR(TIMER1_COMPA_vect) {

  for (int i = 0; i < NUM_MOTORS; i++) {
    if (motor_ticks[i] = UINT_MAX) {
      digitalWrite(PIN_ENABLE[i], LOW);
    } else {
      motor_remaining[i]--;
      if (motor_remaining[i] == 0) {
        motor_remaining[i] = motor_ticks[i];
        digitalWrite(PIN_ENABLE[i], HIGH);
        digitalWrite(PIN_STEP[i], HIGH);
      }
    }
  }
  
  //for (int i = 0; i < 10; i++) __asm__("nop\n\t");

  for (int i = 0; i < NUM_MOTORS; i++) {
    digitalWrite(PIN_STEP[i], LOW);
  }
}


