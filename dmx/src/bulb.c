#include "bulb.h"

static BulbsGroup uprBulbs;
static BulbsGroup lwrBulbs;

static inline void setUprBulbColor(const Command* cmd) {
		uprBulbs.bulbs[cmd->data.bulbColor.bulbNUmber].blue = cmd->data.bulbColor.blue;
		uprBulbs.bulbs[cmd->data.bulbColor.bulbNUmber].green = cmd->data.bulbColor.green;
		uprBulbs.bulbs[cmd->data.bulbColor.bulbNUmber].red = cmd->data.bulbColor.red;
}

static inline void setLwrBulbColor(const Command* cmd) {
		lwrBulbs.bulbs[cmd->data.bulbColor.bulbNUmber].blue = cmd->data.bulbColor.blue;
		lwrBulbs.bulbs[cmd->data.bulbColor.bulbNUmber].green = cmd->data.bulbColor.green;
		lwrBulbs.bulbs[cmd->data.bulbColor.bulbNUmber].red = cmd->data.bulbColor.red;
}

void setBulbColor(const Command* cmd) {
	if(cmd->data.bulbColor.level) setLwrBulbColor(cmd);
	else setUprBulbColor(cmd);
}

void setBulbCount(const Command* cmd) {
	lwrBulbs.curPos = 0;
	uprBulbs.curPos = 0;
	lwrBulbs.countBulbs = cmd->data.bulbsCount.lowerBulbsCount;
	uprBulbs.countBulbs = cmd->data.bulbsCount.upperBulbsCount;
}

uint8_t getCount(Level level) {
	return (level) ? lwrBulbs.countBulbs : uprBulbs.countBulbs;
}

uint8_t getPosition(Level level) {
	return (level) ? lwrBulbs.curPos : uprBulbs.curPos;
}

void setPosition(Level level, uint8_t position) {
	if (level) lwrBulbs.curPos = position;
		else   uprBulbs.curPos = position;
}

