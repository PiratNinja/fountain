/*
 * dmx.c
 *
 *  Created on: 31 июля 2016 г.
 *      Author: Андрей
 */

#include "dmx.h"
//конфиги прокрутки
SetTicker uprTickerConf;
SetTicker lwrTickerConf;

//конфиги + цветовая информация прожектороров
BulbsGroup uprBulbs;
BulbsGroup lwrBulbs;

TickerState tkrSt = DISABLE_TKR;

ComandQueue cmdQueue;

/**********************работа с очередью**********************/
uint8_t getNumberCmd() {
	return (cmdQueue.nextCmd > cmdQueue.firstCmd) 	? cmdQueue.nextCmd - cmdQueue.firstCmd
													: cmdQueue.nextCmd - cmdQueue.firstCmd - (~((uint8_t) MAX_LENGTH_QUEUE)+1);
}

void addCommand(Command* cmd, uint16_t size) {
	if(cmdQueue.lengthQueue < MAX_LENGTH_QUEUE) {
		cmdQueue.commandBuffer[cmdQueue.nextCmd].cmd = *cmd;
		cmdQueue.commandBuffer[cmdQueue.nextCmd].size = size;
		cmdQueue.nextCmd = RINGINC(cmdQueue.nextCmd, MAX_LENGTH_QUEUE);
		cmdQueue.lengthQueue = getNumberCmd();
	}
}

queueItem* dequeuingCommand() {
	uint8_t itemp;

	if(cmdQueue.lengthQueue > 0) {
		itemp = cmdQueue.firstCmd;
		cmdQueue.firstCmd = RINGINC(cmdQueue.firstCmd, MAX_LENGTH_QUEUE);
		cmdQueue.lengthQueue = (getNumberCmd() == MAX_LENGTH_QUEUE) ? 0 : getNumberCmd();
		return &cmdQueue.commandBuffer[itemp];
	}
	return 0;
}

/**********************прокрутка**********************/

uint8_t lowerTickerPrc() {

	static uint8_t countRow = 0;

	if(++countRow < lwrTickerConf.countRow || lwrTickerConf.countRow == 0) return 0;
	else {
		//сдвигаем на SetTickerCmd.shift
		if(lwrTickerConf.direction)
			lwrBulbs.curPos = L_RING_SHIFT(lwrBulbs.curPos, lwrTickerConf.stepShift, lwrBulbs.countBulbs);
		else
			lwrBulbs.curPos = R_RING_SHIFT(lwrBulbs.curPos, lwrTickerConf.stepShift, lwrBulbs.countBulbs);
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
			uprBulbs.curPos = L_RING_SHIFT(uprBulbs.curPos, uprTickerConf.stepShift, uprBulbs.countBulbs);
		else
			uprBulbs.curPos = R_RING_SHIFT(uprBulbs.curPos, uprTickerConf.stepShift, uprBulbs.countBulbs);
		countRow = 0;
		return 1;
	}
}

uint8_t cmdRoutine(const Command* cmd, const uint8_t size) {

	if(cmd->type != PLAIN_ROW && cmd->type != PACKED_ROW) {
		//проверка размера команды
		if(cmd->length != size) return 0;
	}

	switch(cmd->type){
	case PLAIN_ROW:
	case PACKED_ROW:
		if(tkrSt == DISABLE_TKR) return 0;
		else {
			return upperTickerPrc() | lowerTickerPrc();
		}

	break;

	case SET_BULBS_COUNT:
		lwrBulbs.countBulbs = cmd->data.bulbsCount.lowerBulbsCount;
		uprBulbs.countBulbs = cmd->data.bulbsCount.upperBulbsCount;
		return 0;
	break;

	case SET_BULB_COLOR:
		if(cmd->data.bulbColor.level > 0) {
			//нижний уровень
			lwrBulbs.bulbs[cmd->data.bulbColor.bulbNUmber].blue = cmd->data.bulbColor.blue;
			lwrBulbs.bulbs[cmd->data.bulbColor.bulbNUmber].green = cmd->data.bulbColor.green;
			lwrBulbs.bulbs[cmd->data.bulbColor.bulbNUmber].red = cmd->data.bulbColor.red;
		}
		else
		{
			//верхний уровень
			uprBulbs.bulbs[cmd->data.bulbColor.bulbNUmber].blue = cmd->data.bulbColor.blue;
			uprBulbs.bulbs[cmd->data.bulbColor.bulbNUmber].green = cmd->data.bulbColor.green;
			uprBulbs.bulbs[cmd->data.bulbColor.bulbNUmber].red = cmd->data.bulbColor.red;
		}
		return 0;
	break;

	case SET_TICKER:
		return setTicker(cmd);
	break;

	case START_PLAY:
		tkrSt = ENABLE_TKR;
		return 0;
	break;

	case STOP_PLAY:
		tkrSt = DISABLE_TKR;
		return 0;
	break;

	case UPDATE:
		return 1;
	break;

	default:
		return 0;
	break;
	}
}
