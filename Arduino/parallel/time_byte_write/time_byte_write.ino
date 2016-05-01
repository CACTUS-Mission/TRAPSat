/*
  time_write_byte.ino - Program to test SD card operation and write speed.
*/

#include <SPI.h>
#include <SD.h>

// File to be written to. Opened in setup(), closed in loop()
File test_file;

void setup() {
  // Begin serial communication at 115.2k baud
  Serial.begin(115200);
  // Wait for serial connection to initialize before proceeding
  while(!Serial);
  
  // Attempt to open communication with the SD card
  if(!SD.begin()) {
    Serial.println("Setup Failed.");
    while(1);
  }
  
  // Create a file on the SD card to test for
  test_file = SD.open("test.raw", FILE_WRITE);
  test_file.close();

  // Test for created file to ensure SD card is writing and can read
  if(!SD.exists("test.raw")) {
    Serial.println("Error: test file does not exist.");
    while(1);
  }
  
  // Overwrite previous test.raw
  test_file = SD.open("test.raw", FILE_WRITE);
  Serial.println("Setup Complete");
  // Wait a second before progressing to loop()
  delay(1000);
}

void loop() {
  // Start and end time of each write, measured in microseconds
  double startTime, endTime;
  // Average write time in microseconds
  //double averageElapsed;
  
  //unsigned char data = 0x11;
  byte data = 0xFE;
  
  // Write data byte to file 1000 times, measure elapsed time
  for(int i = 1; i <= 1000; i++) {
    // Retrieve microseconds since program start
    startTime = micros();
    delayMicroseconds(5);
    test_file.write(data);
    delayMicroseconds(5);
    // Retrieve microseconds since program start to get difference from startTime
    endTime = micros();
    // Write results readout to Serial
    Serial.print("Time to write byte ");
    Serial.print(i);
    Serial.print(" to file: ");
    Serial.print((endTime - startTime) - 10);
    Serial.println(" micro seconds.");
    
    //averageElapsed = averageElapsed + (endTime-startTime);
  }
  // Flush and close the file
  test_file.flush();
  test_file.close();
  // Write average write time readout to Serial
  /*averageElapsed = averageElapsed/(float)1000.0;
  
  Serial.print("averageElapsederage Time to write 1 byte to file: ");
  Serial.print(averageElapsed);
  Serial.println(" micro seconds.");*/
  
  while(1);
  //delay(100);
}
