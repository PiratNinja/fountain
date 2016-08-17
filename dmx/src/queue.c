#include "queue.h"

static ComandQueue cmdQueue;

uint8_t getNumberCmd() {
	return (cmdQueue.nextCmd > cmdQueue.firstCmd) ? cmdQueue.nextCmd - cmdQueue.firstCmd :
												  cmdQueue.nextCmd - cmdQueue.firstCmd - (~((uint8_t) MAX_LENGTH_QUEUE)+1);
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