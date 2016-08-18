#include "dmx.h"

void setBulbColor(const Command* cmd);
void setBulbCount(const Command* cmd);
uint8_t getCount(Level level);
uint8_t getPosition(Level level);
void setPosition(Level level, uint8_t position);
BulbsGroup* getUprBulbs(void);
BulbsGroup* getLwrBulbs(void);

void setUprBulbColor(const Command* cmd);
void setLwrBulbColor(const Command* cmd);