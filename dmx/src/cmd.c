#include "cmd.h"
#include "ticker.h"
#include "bulb.h"

extern UART_HandleTypeDef huart1;
extern Identificator IDDev;

void TransmitToPC(uint8_t *TXData, uint16_t size) {

	HAL_UART_Transmit_IT(&huart1, TXData, size);
}

uint8_t cmdRoutine(const Command* cmd, const uint8_t size) {

	if(cmd->type != PLAIN_ROW &&
	   cmd->type != PACKED_ROW &&
	   cmd->type != INIT_ALL_BULBS ) {
		if(cmd->length != size) return 0;
	}

	switch(cmd->type){
	case PLAIN_ROW:
	case PACKED_ROW:
		if(tkrState() == ENABLE_TKR) return upperTickerPrc() | lowerTickerPrc();
		break;
	case SET_BULBS_COUNT:
		setBulbCount(cmd);
		break;
	case SET_BULB_COLOR:
		setBulbColor(cmd);
		break;
	case SET_TICKER:
		setTicker(cmd);
		break;
	case START_PLAY:
		play();
		break;
	case STOP_PLAY:
		stop();
		break;
	case UPDATE:
		return 1;
		break;
	case GETID:
		TransmitToPC((uint8_t*) &IDDev, sizeof(IDDev));
		break;
	case INIT_ALL_BULBS:
		setBulbsColor(cmd);
		return 1;
	default:
		return 0;
		break;
	}
	return 0;
}
