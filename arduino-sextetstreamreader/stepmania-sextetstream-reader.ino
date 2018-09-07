
// 13 bytes + '\n'
char data[14];

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  // Does this set to zero or infinity? Arduino docs suck
  Serial.setTimeout(1000);
}

// the loop function runs over and over again forever
void loop() {
  auto bytesRead = Serial.readBytes(data, 14);
  if( bytesRead == 0 )
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(10);
    digitalWrite(LED_BUILTIN, LOW);
    return;
  }
//Byte 3
//0x01 Player 1 #1 left
//0x02 Player 1 #2 right
//0x04 Player 1 #3 up
//0x08 Player 1 #4 down

  digitalWrite(2, (data[3] & 0x01) >> 0x00);
  digitalWrite(3, (data[3] & 0x02) >> 0x01);
  digitalWrite(4, (data[3] & 0x04) >> 0x02);
  digitalWrite(5, (data[3] & 0x08) >> 0x03);
}
