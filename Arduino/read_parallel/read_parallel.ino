// Yun
//int parallel_read_pins[] = {2, 3, 4, 5, 6, 7, 8, 9};

// Mega
int parallel_read_pins[] = {31, 33, 35, 37, 39, 41, 43, 45};


void setup() {

  for(int i=0; i<8; i++)
  {
    pinMode(parallel_read_pins[i], INPUT);
  }

  Serial.begin(19200);
  while( !Serial ) ; // wait for serial terminal to be opened.
}

String msg = "";
String msg_next = "";

void loop() {
  
  msg_next = "";
  for(int i=0; i<8; i++)
  {
    msg_next += digitalRead(parallel_read_pins[i]); 
  }

  if(msg_next != msg)
  {
    msg = msg_next;
    //Serial.println(msg_next);
    Serial.println(msg);
  }

}

