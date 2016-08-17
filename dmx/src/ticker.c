#include "ticker.h"

static SetTicker uprTickerConf;
static SetTicker lwrTickerConf;

static TickerState tkrSt = DISABLE_TKR;

void setTicker(const Command* cmd) {
	if(cmd->data.ticker.shift > getCount(cmd->data.ticker.level)) return;
	if(cmd->data.ticker.level > 0) {
			//нижний уровень
			lwrTickerConf.direction = cmd->data.ticker.direction;
			lwrTickerConf.countRow = cmd->data.ticker.rowCount;
			lwrTickerConf.stepShift = cmd->data.ticker.shift;
		}
		else
		{
			//верхний уровень
			uprTickerConf.direction = cmd->data.ticker.direction;
			uprTickerConf.countRow = cmd->data.ticker.rowCount;
			uprTickerConf.stepShift = cmd->data.ticker.shift;
		}
	return;
}

void play(void) {
	tkrSt = ENABLE_TKR;
}

void stop(void) {
	tkrSt = DISABLE_TKR;
}

TickerState tkrState(void) {
	return tkrSt;
}

uint8_t lowerTickerPrc() {

	static uint8_t countRow = 0;

	if(++countRow < lwrTickerConf.countRow || lwrTickerConf.countRow == 0) return 0;
	else {
		//сдвигаем на SetTickerCmd.shift
		if(lwrTickerConf.direction)
			setPosition(L_RING_SHIFT(getPosition(LOWER), lwrTickerConf.stepShift, getCount(LOWER)), LOWER);
		else
			setPosition(R_RING_SHIFT(getPosition(LOWER), lwrTickerConf.stepShift, getCount(LOWER)), LOWER);
		countRow = 0;
		return 1;
	}
}

uint8_t upperTickerPrc() {

	static uint8_t countRow = 0;

	if(++countRow < uprTickerConf.countRow 	|| uprTickerConf.countRow == 0) return 0;
	else {
		//сдвигаем на SetTickerCmd.shift
		if(uprTickerConf.direction)
			setPosition(L_RING_SHIFT(getPosition(UPPER), uprTickerConf.stepShift, getCount(UPPER)), UPPER);
		else
			setPosition(R_RING_SHIFT(getPosition(UPPER), uprTickerConf.stepShift, getCount(UPPER)), UPPER);
		countRow = 0;
		return 1;
	}
}