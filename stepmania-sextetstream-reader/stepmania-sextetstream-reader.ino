
// 13 bytes + '\n'
const uint32_t dataSize = 14;
char data[dataSize];

unsigned long lastInput = 0;

// Pad enum
// Also output pin ID
enum Pad {
  pad1Left = 2,
  pad1Right = 3,
  pad1Up = 4,
  pad1Down = 5,
};

void setPad( uint8_t pad, bool on ) {
  digitalWrite( pad, on );
}

void setAllPads( bool on ) {
  setPad( pad1Left, on );
  setPad( pad1Right, on );
  setPad( pad1Up, on );
  setPad( pad1Down, on );
}

// Input
// Byte 3
// 0x01 Player 1 #1 left
// 0x02 Player 1 #2 right
// 0x04 Player 1 #3 up
// 0x08 Player 1 #4 down
bool getSextet( char data[dataSize], uint8_t pad ) {
  switch( pad ) {
    case pad1Left:
      return (data[3] & 0x01) >> 0x00;
    case pad1Right:
      return (data[3] & 0x02) >> 0x01;
    case pad1Up:
      return (data[3] & 0x04) >> 0x02;
    case pad1Down:
      return (data[3] & 0x08) >> 0x03;
  }
}

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(pad1Left, OUTPUT);
  pinMode(pad1Right, OUTPUT);
  pinMode(pad1Up, OUTPUT);
  pinMode(pad1Down, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

  // Set a timeout on serial reads - Let us skip if there's no input for a while
  Serial.setTimeout(100);
}

// the loop function runs over and over again forever
void loop() {

  auto bytesRead = Serial.readBytes(data, dataSize);
  if( bytesRead == 0 )
  {
    // No data on serial, pulse the lights
    auto d = 50;
    setPad( pad1Up, true );
    delay( d );
    setAllPads( false );
    setPad( pad1Right, true );
    delay( d );
    setAllPads( false );
    setPad( pad1Down, true );
    delay( d );
    setAllPads( false );
    setPad( pad1Left, true );
    delay( d );
    setAllPads( false );

    return;
  }

  // Set the pad lights based on the data
  setPad( pad1Left, getSextet( data, pad1Left ) );
  setPad( pad1Right, getSextet( data, pad1Right ) );
  setPad( pad1Up, getSextet( data, pad1Up ) );
  setPad( pad1Down, getSextet( data, pad1Down ) );
}
