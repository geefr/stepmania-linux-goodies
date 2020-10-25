#ifndef COMMON_H
#define COMMON_H

#include <map>
#include <memory>
#include <limits>

// The directions on the pad
// Panel enum maps directly to joystick buttons
enum Panel
{
  Invalid = 0,
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
  int rawValueMin = std::numeric_limits<int>::max(); // Lowest value the sensor has seen, may be treated as zero by trigger algorithms
  int filteredValue = 0; // Sensor value after filtering, by FSRAlgorithm
  bool triggered = false;// Sensor is on/off, set by FSRAlgorithm
  int triggerValue = 100; // Sensor trigger value
};

class SerialProto;
struct State
{ 
  std::unique_ptr<SerialProto> serial;
  std::map<int, SensorState> sensors; // <pin index, state>
  std::map<Panel, bool> panels; // Whether panels are on or off, output of PanelAlgorithm
};

#endif
