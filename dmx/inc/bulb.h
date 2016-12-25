#include "dmx.h"

void setBulbColor(const Command* cmd);
void setBulbsColor(const SCommand* cmd);
void setBulbCount(const Command* cmd);
void setBulbSCCount(const SCommand* cmd);
uint8_t getCount(Level level);
uint8_t getPosition(Level level);
void setPosition(Level level, uint8_t position);
BulbsGroup* getUprBulbs(void);
BulbsGroup* getLwrBulbs(void);

void setUprBulbColor(const Command* cmd);
void setLwrBulbColor(const Command* cmd);
void setUprBulbsColor(const SCommand* cmd);
void setLwrBulbsColor(const SCommand* cmd);
