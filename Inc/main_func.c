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

static inline void generalPort() {

//	static uint8_t countRecByte = 0;
//
//	switch (recState) {
//		case 1:
//			//IDLE на линии и ненулевое количество принятых байт - конец команды.
//			if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) && countRecByte > 0)
//				recState = 2;
//		break;
//
//		case 2:
//			addCommand((Command*)recBuf, countRecByte);
//			#ifdef DEBUG
//					HAL_UART_Transmit_IT(&huart1, recBuf, countRecByte);
//			#endif
//			countRecByte = 0;
//			recState = 0;
//		break;
//
//		case 3:
//			recBuf[countRecByte++] = tmp;
//			recState = 0;
//		break;
//	}
//
//	if (recState == 0) {
//		// запускаем ожидание данных
//		recState = 1;
//		// пофиг на переполенние по приему (выбираем только начало данных)
//		__HAL_UART_CLEAR_OREFLAG(&huart1);
//
//		HAL_UART_Receive_IT(&huart1, &tmp, 1);
//	}
	switch (recState) {
		case 1:
			//IDLE на линии и ненулевое количество принятых байт - конец команды.
			if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) && countRecByte > 0)
				recState = 2;
		break;

		case 2:
			addCommand((Command*)recBuf, countRecByte);
			#ifdef DEBUG
				HAL_UART_Transmit_IT(&huart1, recBuf, countRecByte);
			#endif
			countRecByte = 0;
			//commandReception = 0;
			recState = 0;
		break;

		case 0:
			// запускаем ожидание данных
			// пофиг на переполенние по приему (выбираем только начало данных)
			__HAL_UART_CLEAR_OREFLAG(&huart1);
			HAL_UART_Receive_IT(&huart1, &tmp, 1);
			recState = 1;
		break;

		case 3:
			recBuf[countRecByte++] = tmp;
			recState = 0;
			//commandReception = 1;
		break;
		}
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


//void dmx_pin_uart() {
//	GPIO_InitTypeDef gpio;
//	gpio.Pin = GPIO_PIN_2;
//    gpio.Mode = GPIO_MODE_AF_PP;
//    gpio.Speed = GPIO_SPEED_HIGH;
//    HAL_GPIO_Init(GPIOA, &gpio);
//}
//
//void dmx_pin_break() {
//	GPIO_InitTypeDef gpio;
//	gpio.Pin = GPIO_PIN_2;
//    gpio.Mode = GPIO_MODE_OUTPUT_PP;
//    gpio.Speed = GPIO_SPEED_HIGH;
//    HAL_GPIO_Init(GPIOA, &gpio);
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
//}
//
//void usDelay(uint32_t us) {
//	us *= (SystemCoreClock / 1000000) / 5;
//    while (us--);
//}
//
//static inline void digitalInputs()	{
//	static uint8_t inState = 0, inStateOld = 0;
//
//	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)) inState |= 1 << 0;
//	else inState &= ~(1 << 0);
//
//	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)) inState |= 1 << 1;
//	else inState &= ~(1 << 1);
//
//	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)) inState |= 1 << 2;
//	else inState &= ~(1 << 2);
//
//	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)) inState |= 1 << 3;
//	else inState &= ~(1 << 3);
//
//	if (inState != inStateOld) {
//		inStateOld = inState;
//
//		HAL_UART_Transmit_IT(&huart1, &inState, 1);
//	}
//}
//
//static inline void generalPort() {
//
////	static uint8_t countRecByte = 0;
////
////	switch (recState) {
////		case 1:
////			//IDLE на линии и ненулевое количество принятых байт - конец команды.
////			if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) && countRecByte > 0)
////				recState = 2;
////		break;
////
////		case 2:
////			addCommand((Command*)recBuf, countRecByte);
////			#ifdef DEBUG
////					HAL_UART_Transmit_IT(&huart1, recBuf, countRecByte);
////			#endif
////			countRecByte = 0;
////			recState = 0;
////		break;
////
////		case 3:
////			recBuf[countRecByte++] = tmp;
////			recState = 0;
////		break;
////	}
////
////	if (recState == 0) {
////		// запускаем ожидание данных
////		recState = 1;
////		// пофиг на переполенние по приему (выбираем только начало данных)
////		__HAL_UART_CLEAR_OREFLAG(&huart1);
////
////		HAL_UART_Receive_IT(&huart1, &tmp, 1);
////	}
//
//	switch (recState) {
//		case 1:
//			//IDLE на линии и ненулевое количество принятых байт - конец команды.
//			if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) && countRecByte > 0)
//				recState = 2;
//			else
//				break;
//
//		case 2:
//			addCommand((Command*)recBuf, countRecByte);
//			#ifdef DEBUG
//				HAL_UART_Transmit_IT(&huart1, recBuf, countRecByte);
//			#endif
//			countRecByte = 0;
//			//commandReception = 0;
//			recState = 0;
//
//		case 0:
//			// запускаем ожидание данных
//			// пофиг на переполенние по приему (выбираем только начало данных)
//			__HAL_UART_CLEAR_OREFLAG(&huart1);
//			HAL_UART_Receive_IT(&huart1, &tmp, 1);
//		break;
//
//		case 3:
//			recBuf[countRecByte++] = tmp;
//			// запускаем ожидание данных
//			// пофиг на переполенние по приему (выбираем только начало данных)
//			// __HAL_UART_CLEAR_OREFLAG(&huart1);
//			// HAL_UART_Receive_IT(&huart1, &tmp, 1);
//
//			recState = 1;
//		}
//
//
////	if (recState == 0) {
////		// çàïóñêàåì îæèäàíèå äàííûõ
////		recState = 1;
////		// ïîôèã íà ïåðåïîëåííèå ïî ïðèåìó (âûáèðàåì òîëüêî íà÷àëî äàííûõ)
////		__HAL_UART_CLEAR_OREFLAG(&huart1);
////
////		HAL_UART_Receive_IT(&huart1, &tmp, 1);
////	} else if (recState == 1) {
////		//IDLE íà ëèíèè è íåíóëåâîå êîëè÷åñòâî ïðèíÿòûõ áàéò - êîíåö êîìàíäû.
////		if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) && countRecByte > 0)
////			recState = 2;
////
////	} else if (recState == 2) {
////		addCommand((Command*)recBuf, countRecByte);
////	#ifdef DEBUG
////		HAL_UART_Transmit_IT(&huart1, recBuf, countRecByte);
////	#endif
////		countRecByte = 0;
////		recState = 0;
////
////	} else if (recState == 3){
////		recBuf[countRecByte++] = tmp;
////		recState = 0;
////	}
//}

//static inline void DMXPort(uint32_t curTime, uint32_t* lastTime, uint8_t* updateBulbs, BulbsGroup* bulbsData) {
//
////	static uint8_t *firstPartData, *secondPartData;
////	static uint16_t firstPartLength, secondPartLength;
////	uint8_t startByte = 0;
////
////	switch (dmxState) {
////		case 0:
////			if (curTime - *lastTime > DMX_TX_DELAY) { // отправляем через в 10мс
////				*lastTime = curTime;
////				//dmxState = 1;
////			}
////			else
////				break;
////
////		case 1:
////			// отправляем break
////			dmxState = 2;
////			dmx_pin_break();
////
////			firstPartData = (uint8_t*) &(bulbsData->bulbs[bulbsData->curPos]);
////			firstPartLength = (bulbsData->countBulbs - bulbsData->curPos) * sizeof(Bulb);
////			secondPartData = (uint8_t*) &(bulbsData->bulbs[0]);
////			secondPartLength = bulbsData->curPos * sizeof(Bulb);
////		break;
////
////		case 2:
////			if (curTime > *lastTime) {
////				//dmxState = 3;
////				*lastTime = curTime;
////				dmx_pin_uart();
////			}
////			else
////				break;
////
////		//стартовый байт
////		case 3:
////			dmxState = 4;
////			*lastTime = curTime;
////			HAL_UART_Transmit_IT(&huart2, &startByte, 1);
////		break;
////
////		case 4:
////			if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC)) {
////				*lastTime = curTime;
////				//dmxState = 5;
////			}
////			else
////				break;
////
////		//прожекторы - первая часть узора
////		case 5:
////			dmxState = 6;
////			*lastTime = curTime;
////			HAL_UART_Transmit_IT(&huart2, firstPartData, firstPartLength);
////		break;
////
////		case 6:
////			if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC)) {
////				*lastTime = curTime;
////				//dmxState = 7;
////			}
////			else
////				break;
////
////		//вторая
////		case 7:
////			dmxState = 8;
////			*lastTime = curTime;
////			HAL_UART_Transmit_IT(&huart2, secondPartData, secondPartLength);
////		break;
////
////		case 8:
////			if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC)) {
////				*lastTime = curTime;
////				*updateBulbs = 0;
////				dmxState = 0;
////			}
////		//break;
////	}
//
//
//	static uint8_t *firstPartData, *secondPartData;
//		static uint16_t firstPartLength, secondPartLength;
//		uint8_t startByte = 0;
//
//		if (dmxState == 0) {
//			if (curTime - *lastTime > DMX_TX_DELAY) {
//				*lastTime = curTime;
//				dmxState = 1;
//			}
//		} else if (dmxState == 1) {
//			dmxState = 2;
//			dmx_pin_break();
//
//			firstPartData = (uint8_t*) &(bulbsData->bulbs[bulbsData->curPos]);
//			firstPartLength = (bulbsData->countBulbs - bulbsData->curPos) * sizeof(Bulb);
//			secondPartData = (uint8_t*) &(bulbsData->bulbs[0]);
//			secondPartLength = bulbsData->curPos * sizeof(Bulb);
//
//		} else if (dmxState == 2) {
//			if (curTime > *lastTime) {
//				dmxState = 3;
//				*lastTime = curTime;
//				dmx_pin_uart();
//			}
//		} else if (dmxState == 3) {
//				dmxState = 4;
//				*lastTime = curTime;
//				HAL_UART_Transmit_IT(&huart2, &startByte, 1);
//		} else if (dmxState == 4) {
//			if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC)) {
//				*lastTime = curTime;
//				dmxState = 5;
//			}
//		} else if (dmxState == 5) {
//				dmxState = 6;
//				*lastTime = curTime;
//				HAL_UART_Transmit_IT(&huart2, firstPartData, firstPartLength);
//		} else if (dmxState == 6) {
//			if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC)) {
//				*lastTime = curTime;
//				dmxState = 7;
//			}
//		} else if (dmxState == 7) {
//				dmxState = 8;
//				*lastTime = curTime;
//				HAL_UART_Transmit_IT(&huart2, secondPartData, secondPartLength);
//		} else if (dmxState == 8) {
//			if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC)) {
//				*lastTime = curTime;
//				*updateBulbs = 0;
//				dmxState = 0;
//			}
//		}
//}
