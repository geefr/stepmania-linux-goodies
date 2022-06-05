#ifndef LIGHT_H
#define LIGHT_H

#include <cstdint>

class Light {
  private:
      static const uint8_t MinVal = 0x00;
      static const uint8_t MaxVal = 0xff;
      // In val per millisecond i.e 1 == fade over 255 milliseconds
      static const uint8_t mFadeInDelta = 4;
      static const uint8_t mFadeOutDelta = 4;

  public:
      Light(int pin);
      Light() = default;
      Light( const Light& ) = default;
      Light& operator=( const Light& ) = default;
      
      void tick(float deltaSeconds);
      void target( uint8_t v );
      void set( uint8_t v );
      void add( uint8_t v );
      void sub( uint8_t v );

  private:
      int mPin;
      int32_t mVal;
      uint8_t mTargetVal;
};

#endif
