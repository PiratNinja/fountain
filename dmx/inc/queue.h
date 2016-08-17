#include "dmx.h"

uint8_t getNumberCmd();
void addCommand(Command* cmd, uint16_t size);
queueItem* dequeuingCommand();