#ifndef PAD_H
#define PAD_H

// Pad enum -> output pin ID
// Reassign these to what you need, these 2 are a fairly sensible layout
// Analog (PWM) io
enum LightPin {
  pad1Left = 3,
  pad1Right = 9,
  pad1Up = 10,
  pad1Down = 11,
};

static const uint32_t SextetSize = 14;

class Light {
  public:
      static const uint8_t MinVal = 0x00;
      static const uint8_t MaxVal = 0xff;
      Light()
          : mPin(pad1Left), mVal(MinVal), mByte(0), mBit(0), mShift(0) {}
      Light(LightPin p, uint32_t byte, uint32_t bit, uint32_t shift) 
          : mPin(p), mVal(MinVal)
          , mByte(byte), mBit(bit), mShift(shift) {}
      Light( const Light& ) = default;
      Light& operator=( const Light& ) = default;
      LightPin pin() const { return mPin; }
      void tick(uint32_t deltaMS) {
        if( mVal < mTargetVal ) {
            mVal += mFadeInDelta * deltaMS;
        } else if( mVal > mTargetVal ) {
            mVal -= mFadeOutDelta * deltaMS;
        }

        mVal = max( mVal, MinVal );
        mVal = min( mVal, MaxVal );
        set( mVal );
      }
      void target( uint8_t v ) {
          mTargetVal = v;
          if( mTargetVal > mVal ) {
              set( mTargetVal );
          }
      }
      void set( uint8_t v ) { 
          mVal = max( v, MinVal );
          mVal = min( v, MaxVal );
          analogWrite( mPin, v );
          mVal = v; }
      void add( uint8_t v ) { 
          if( MaxVal - mVal < v ) set(MaxVal);
          else set( static_cast<uint8_t>(mVal + v) ); }
      void sub( uint8_t v ) { 
          if( v > mVal ) set( MinVal );
          else set( static_cast<uint8_t>(mVal - v) ); }
      bool sextet( char data[SextetSize] ) {
        return (data[mByte] & mBit) >> mShift;
      }
  private:
      LightPin mPin;
      // In val per millisecond i.e 1 == fade over 255 milliseconds
      static const uint8_t mFadeInDelta = 4;
      static const uint8_t mFadeOutDelta = 2;
      int32_t mVal;
      uint8_t mTargetVal;
      uint32_t mByte;
      uint32_t mBit;
      uint32_t mShift;
};

class Pad {
    public:
        Pad() {
          // TODO: This is hardcoded, should it be?
          mLights[0] = Light(pad1Left, 0x03, 0x01, 0x00);
          mLights[1] = Light(pad1Right, 0x03, 0x02, 0x01);
          mLights[2] = Light(pad1Up, 0x03, 0x04, 0x02);
          mLights[3] = Light(pad1Down, 0x03, 0x08, 0x03);
        }
        ~Pad() {}
        // Perform a tick - Fade the lights out
        void tick(uint32_t deltaMS) {
            for( auto i = 0u; i < mNumLights; ++i ) {
                mLights[i].tick(deltaMS);
            }
        }

        void setFromSextet( char data[SextetSize] ) {
            for( auto i = 0u; i < mNumLights; ++i ) {
                mLights[i].target( mLights[i].sextet( data ) ? 0xff : 0x00 );
            }
        }

        void set( LightPin pad, bool v ) {
            for( auto i = 0u; i < mNumLights; ++i ) {
                if( mLights[i].pin() == pad )
                    mLights[i].set( v );
            }
        }

        void set( LightPin pad, uint8_t v ) {
            for( auto i = 0u; i < mNumLights; ++i ) {
                if( mLights[i].pin() == pad )
                    mLights[i].set( v );
            }
        }

    private:
        static const uint32_t mNumLights = 4;
        Light mLights[mNumLights];
};

#endif
