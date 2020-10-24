#ifndef COMMON_H
#define COMMON_H

#include <map>
#include <memory>

/// The directions on the pad
enum Panel
{
  Invalid,
  P1Up,
  P1Right,
  P1Down,
  P1Left,
  P2Up,
  P2Right,
  P2Down,
  P2Left,
};

/// Mapping between an IO pin (fsr) and panel
struct SensorState
{
  Panel panel = Panel::Invalid; // The panel/direction the pin is for
  int rawValue = 0; // Raw sensor value, read by main sketch
  int filteredValue = 0; // Sensor value after filtering, by FSRAlgorithm
  bool triggered = false;// Sensor is on/off, set by FSRAlgorithm
  // TODO: Other stuff like per-fsr sensitivity etc
};

class SerialProto;
struct State
{ 
  std::unique_ptr<SerialProto> serial;
  std::map<int, SensorState> sensors; // <pin index, state>
};

#endif
