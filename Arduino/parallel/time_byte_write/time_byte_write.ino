#include <SPI.h>
#include <SD.h>

File test_file;

void setup() {
  
  Serial.begin(115200);
  while(!Serial);
  
  if(!SD.begin()) {
    Serial.println("Setup Failed.");
    while(1) ;
  }
    
  test_file=SD.open("test.raw", FILE_WRITE);
  test_file.close();

  if(!SD.exists("test.raw")) {
    Serial.println("Error: test file does not exist.");
    while(1) ;
  }
  
  test_file=SD.open("test.raw", FILE_WRITE);
  Serial.println("Setup Complete");
  delay(1000);
}

void loop() {
  double t0, tf, av;
  //unsigned char data = 0x11;
  byte data = 0xFE;
  
  for(int i=1; i<=1000; i++) {
    t0 = micros();
    delayMicroseconds(5);
    test_file.write(data);
    delayMicroseconds(5);
    tf = micros();   
    Serial.print("Time to write byte ");
    Serial.print(i);
    Serial.print(" to file: ");
    Serial.print((tf-t0)-10);
    Serial.println(" micro seconds.");
    //av = av + (tf-t0);
  }
  test_file.flush();
  test_file.close();
  //av = av/(float)1000.0;
  
  //Serial.print("Average Time to write 1 byte to file: ");
  //Serial.print(av);
  //Serial.println(" micro seconds.");
  while(1) ;
  //delay(100);
}
