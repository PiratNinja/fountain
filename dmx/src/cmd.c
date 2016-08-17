#include "cmd.h"
#include "ticker.h"
#include "bulb.h"

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
		return 1;
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

	default:
		return 0;
	break;
	}
	return 0;
}