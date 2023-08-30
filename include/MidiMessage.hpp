#include <cstdint>

class MidiMessage {
  
 public:
  uint8_t getChannel();
  uint8_t getType();

  

 private:
  uint8_t status;
  uint8_t data1;
  uint8_t data2;
};