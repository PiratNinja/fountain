void dmx_pin_uart() {
	GPIO_InitTypeDef gpio;
	gpio.Pin = GPIO_PIN_2;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOA, &gpio);
}

void dmx_pin_break() {
	GPIO_InitTypeDef gpio;
	gpio.Pin = GPIO_PIN_2;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOA, &gpio);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
}

void usDelay(uint32_t us) {
	us *= (SystemCoreClock / 1000000) / 5;
    while (us--);
}

static inline void digitalInputs()	{
	static uint8_t inState = 0, inStateOld = 0;

	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)) inState |= 1 << 0;
	else inState &= ~(1 << 0);

	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)) inState |= 1 << 1;
	else inState &= ~(1 << 1);

	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)) inState |= 1 << 2;
	else inState &= ~(1 << 2);

	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)) inState |= 1 << 3;
	else inState &= ~(1 << 3);

	if (inState != inStateOld) {
		inStateOld = inState;

		HAL_UART_Transmit_IT(&huart1, &inState, 1);
	}
}

static inline uint8_t generalPort() {

	volatile static uint32_t bPause;
	volatile static uint8_t TRFlag = 0;
	//__IO uint32_t diff, diff2;

	switch (recState) {
		case 0:
			// запускаем ожидание данных
			HAL_UART_Receive_IT(&huart1, &tmp, 1);
			recState = 2;
			break;
		case 1:
			if(countRecByte < RECV_SIZE) {
				recBuf[countRecByte++] = tmp;
				TRFlag = 1;
			}
			recState = 0;
			break;
		case 2:
			//фиксация времени начала паузы на шине при условии наличия признака начала передачи
			if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) && TRFlag) {
				bPause = HAL_GetTick();
				recState = 3;
			}
			break;
		case 3:
			//проверка окончания паузы: если пауза больше или равна установленной
			//то фиксируем конец передачи + инициируем ожидание новой передачи, при условии все еще наличия паузы на шине.

			//debug
			//diff = HAL_GetTick() - bPause;
			if(HAL_GetTick() - bPause >= PAUSE_UART1 && __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)) {
				TRFlag = 0;
				//diff = HAL_GetTick();
				HAL_UART_Receive_IT(&huart1, &tmp, 1);
				//diff2 = HAL_GetTick() - diff;
				return 1; //end of frame
			}
	}
	return 0;
}

static inline void DMXPort(uint32_t curTime, uint32_t* lastTime, uint8_t* updateBulbs, BulbsGroup* bulbsData) {

	static uint8_t *firstPartData, *secondPartData;
	static uint16_t firstPartLength, secondPartLength;
	uint8_t startByte = 0;

	switch (dmxState) {
		case 0:
			if (curTime - *lastTime > DMX_TX_DELAY) { // отправляем через в 10мс
				*lastTime = curTime;
				dmxState = 1;
			}
			break;
		case 1:
		// отправляем break
			dmxState = 2;
			dmx_pin_break();

			firstPartData = (uint8_t*) &(bulbsData->bulbs[bulbsData->curPos]);
			firstPartLength = (bulbsData->countBulbs - bulbsData->curPos) * sizeof(Bulb);
			secondPartData = (uint8_t*) &(bulbsData->bulbs[0]);
			secondPartLength = bulbsData->curPos * sizeof(Bulb);
			break;
		case 2:
			if (curTime > *lastTime) {
				dmxState = 3;
				*lastTime = curTime;
				dmx_pin_uart();
			}
			break;
		//стартовый байт
		case 3:
			dmxState = 4;
			*lastTime = curTime;
			HAL_UART_Transmit_IT(&huart2, &startByte, 1);
			break;
		case 4:
			if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC)) {
				*lastTime = curTime;
				dmxState = 5;
			}
			break;
		//прожекторы - первая часть узора
		case 5:
			dmxState = 6;
			*lastTime = curTime;
			HAL_UART_Transmit_IT(&huart2, firstPartData, firstPartLength);
			break;
		case 6:
			if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC)) {
				*lastTime = curTime;
				dmxState = 7;
			}
			break;
		//вторая
		case 7:
			dmxState = 8;
			*lastTime = curTime;
			HAL_UART_Transmit_IT(&huart2, secondPartData, secondPartLength);
			break;
		case 8:
			if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC)) {
				*lastTime = curTime;
				*updateBulbs = 0;
				dmxState = 0;
			}
			break;
	}
}
