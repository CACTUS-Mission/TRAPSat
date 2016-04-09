
int parallel_read_pins[] = {2, 3, 4, 5, 6, 7, 8, 9};

void setup() {
  
  for(int i=0; i<8; i++)
  {
    pinMode(parallel_read_pins[i], INPUT);
  }

  Serial.begin(9600);
  while( !Serial ) ; // wait for serial terminal to be opened.
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0; i<8; i++)
  {
    Serial.print(digitalRead(parallel_read_pins[i])); 
  }
  Serial.println("");
}
