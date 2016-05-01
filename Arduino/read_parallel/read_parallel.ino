/*
  read_parallel.ino - Reads pins in parallel.
*/

// Number of pins that will be read
const int PIN_COUNT = 8;

// Pin numbers for Arduino Yún
//int parallel_read_pins[PIN_COUNT] = {2, 3, 4, 5, 6, 7, 8, 9};

// Pin numbers for Arduino Mega
int parallel_read_pins[PIN_COUNT] = {31, 33, 35, 37, 39, 41, 43, 45};

// Initialize message strings for read loop
String msg = "";
String msg_next;

void setup() {
  // Set pin mode for each pin to INPUT
  for(int i = 0; i < PIN_COUNT; i++)
  {
    pinMode(parallel_read_pins[i], INPUT);
  }
  
  // Begin serial communication at 19.2k baud
  Serial.begin(19200);
  // Wait for serial connection to initialize
  while(!Serial);
}

void loop() {
  // Reset msg_next for each read
  msg_next = "";
  
  // Read all pins and append their value to msg_next
  for(int i = 0; i < PIN_COUNT; i++)
  {
    msg_next += digitalRead(parallel_read_pins[i]); 
  }

  // If data is different from last read, write to serial
  if(msg_next != msg)
  {
    msg = msg_next;
    //Serial.println(msg_next);
    Serial.println(msg);
  }
}