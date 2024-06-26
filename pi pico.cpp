/*

if you see "// insert comment here" this is a area you can change things for your liking/controller setup

*/

#include <PicoGamepad.h>
PicoGamepad gamepad;

const int buttoncount = 9; // amount of pins in use
const unsigned int buttonpins[] = {22, 13, 12, 11, 10, 15, 17, 2, 1}; // pins being used (first 5 MUST be GRYBO)

unsigned int buttonstate[] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; // if you added or removed the amount of pins add/remove the correct amount of 0s
unsigned int prevbuttonstate[] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; // if you added or removed the amount of pins add/remove the correct amount of 0s

const int hz = 1000; // setting for dequeue poll rate (hz)
const int debounce = 24; // setting for debounce (ms)

unsigned long pollrate = 1.0 / hz * 1000000.0;

int queueinput[200];
int queueactutate[200];
int front = -1; 
int rear = -1;
int queuecount = 0;

unsigned long time2;

void setup() {
  for (int i = 0; i < buttoncount; i++)
    pinMode(buttonpins[i], INPUT_PULLUP);

  time2 = micros();
}

void addqueue(int item1, int item2) {
  if (rear == 200 - 1) {
  }
  else {
    if (front == -1)
      front = 0;
    rear++;
    queueinput[rear] = item1;
    queueactutate[rear] = item2;
    queuecount++;
  }
}

void dequeue() {
  if (front == -1) {
  }
  else {
    front++;
    queuecount--;
    if (front > rear)
      front = rear = -1;
  }
}

void loop() {
  if (queuecount < 200) {
    for (int i = 0; i < 5; i++) {
      buttonstate[i] = digitalRead(buttonpins[i]);
      if (buttonstate[i] != prevbuttonstate[i]) {
        if (buttonstate[i] == LOW) {
          addqueue(buttonpins[i], 1);
        }
        else {
          addqueue(buttonpins[i], 0);
        }
        prevbuttonstate[i] = buttonstate[i];
      }
    }
  }
  
  unsigned long newTime = micros();
  if (newTime - time2 >= pollrate) {
    if (queuecount) {
      gamepad.SetButton(queueinput[front], queueactutate[front]);
      gamepad.send_update();
      dequeue();
    }

    for (int i = 0; i < 4; i++) {
      buttonstate[i + 5] = digitalRead(buttonpins[i + 5]);
      if (buttonstate[i + 5] != prevbuttonstate[i + 5]) {
        if (buttonstate[i + 5] == LOW) {
          gamepad.SetButton(buttonpins[i + 5], 1);
          gamepad.send_update();
        }
        else {
          gamepad.SetButton(buttonpins[i + 5], 0);
          gamepad.send_update();
        }
        prevbuttonstate[i + 5] = buttonstate[i + 5];
      }
    }
    
    int w = analogRead(28);
    gamepad.SetRz(w);
    gamepad.send_update();
    
    time2 = newTime;
  } else delayMicroseconds(debounce);

  if (queuecount >= 199) {
    rear = - 1;
    front = - 1;
    queuecount = 0;
  }
}
