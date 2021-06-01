
#include "serialprotonone.h"

#include <Arduino.h>

#include "common.h"

SerialProtoNone::SerialProtoNone(int baud)
  : SerialProto(baud)
{}

SerialProtoNone::~SerialProtoNone()
{}

void SerialProtoNone::initialise() {
  Serial.begin(mBaud);
}

void SerialProtoNone::update(float t, float dT, State& state) {}
