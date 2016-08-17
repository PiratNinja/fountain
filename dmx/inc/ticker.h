#include "dmx.h"
#include "bulb.h"

void setTicker(const Command* cmd);
void play(void);
void stop(void);
TickerState tkrState(void);
uint8_t lowerTickerPrc();
uint8_t upperTickerPrc();