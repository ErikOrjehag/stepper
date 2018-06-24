#include <CmdMessenger.h>
#include <limits.h>
#include "timer.hpp"


int NUM_MOTORS = 4;
int PIN_ENABLE[] = { 2, 7, 10, A0 };
int PIN_DIR[] = { 3, 6, 9, 11 };
int PIN_STEP[] = { 4, 5, 8, 12 };

long TIMER_INTERVAL = 125L;
long STEPS_PER_REV = 800L;

unsigned int* motor_ticks;
unsigned int* motor_remaining;

enum {
  steer
};

CmdMessenger cmd = CmdMessenger(Serial);

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

  cmd.attach(steer, on_steer);
}

void loop() {
  cmd.feedinSerialData();
  delay(10);
}

void on_steer() {
  for (int i = 0; i < NUM_MOTORS; i++) {
    int speed = cmd.readBinArg<int>();
    set_motor_speed(i, speed);
  }
}

void set_motor_speed(int motor, int speed) {
  unsigned int ticks = (unsigned int) ((1000L * 1000000L)/(TIMER_INTERVAL*STEPS_PER_REV*abs(speed)));
  motor_ticks[motor] = ticks;
  
  //digitalWrite(PIN_ENABLE[motor], ticks != UINT_MAX);
  digitalWrite(PIN_DIR[motor], speed < 0);
}

bool led = false;

ISR(TIMER1_COMPA_vect) {

  for (int i = 0; i < NUM_MOTORS; i++) {
    if (motor_ticks[i] != UINT_MAX) {
      motor_remaining[i]--;
      if (motor_remaining[i] == 0) {
        motor_remaining[i] = motor_ticks[i];
        digitalWrite(PIN_STEP[i], HIGH);
  
        led = !led;
        //digitalWrite(13, led);
      }
    }
  }
  
  for (int i = 0; i < 10; i++) __asm__("nop\n\t");

  for (int i = 0; i < NUM_MOTORS; i++) {
    digitalWrite(PIN_STEP[i], LOW);
  }
}


