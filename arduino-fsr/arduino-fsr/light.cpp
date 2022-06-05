#include "light.h"

#include <algorithm>

#include "Arduino.h"

Light::Light(int pin)
  : mPin(pin)
{}

void Light::tick(float deltaSeconds) {
  uint32_t deltaMS = static_cast<uint32_t>(deltaSeconds * 1000.0f);
  if( mVal < mTargetVal ) {
      mVal += mFadeInDelta * deltaMS;
  } else if( mVal > mTargetVal ) {
      mVal -= mFadeOutDelta * deltaMS;
  }

  mVal = std::max( mVal, static_cast<int32_t>(MinVal) );
  mVal = std::min( mVal, static_cast<int32_t>(MaxVal) );
  set( mVal );
}

void Light::target( uint8_t v ) {
  mTargetVal = v;
  if( mTargetVal > mVal ) {
      set( mTargetVal );
  }
}

void Light::set( uint8_t v ) { 
  mVal = std::max( v, MinVal );
  mVal = std::min( v, MaxVal );
  analogWrite( mPin, v );
  mVal = v;
}

void Light::add( uint8_t v ) { 
  if( MaxVal - mVal < v ) set(MaxVal);
  else set( static_cast<uint8_t>(mVal + v) );
}

void Light::sub( uint8_t v ) { 
  if( v > mVal ) set( MinVal );
  else set( static_cast<uint8_t>(mVal - v) );
}
