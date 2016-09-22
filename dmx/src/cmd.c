#include "cmd.h"
#include "ticker.h"
#include "bulb.h"

extern UART_HandleTypeDef huart1;
extern Identificator IDDev;

uint8_t cmdRoutine(const Command* cmd, const uint8_t size) {

	if(cmd->type != PLAIN_ROW && cmd->type != PACKED_ROW) {
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
		HAL_UART_Transmit_IT(&huart1, (uint8_t*) &IDDev, sizeof(IDDev));
		break;
	default:
		return 0;
		break;
	}
	return 0;
}
