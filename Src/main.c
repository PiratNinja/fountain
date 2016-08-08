
/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "dmx.h"

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

extern SetTicker uprTickerConf;
extern SetTicker lwrTickerConf;

extern BulbsGroup uprBulbs;
extern BulbsGroup lwrBulbs;

// получаем только номер команды и состояние цветов
#define RECV_SIZE 128
uint8_t recBuf[RECV_SIZE];
uint8_t tmp;
uint8_t recState = 0, dmxState = 0;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);

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

	static uint8_t countRecByte = 0;

	if (recState == 0) {
		// запускаем ожидание данных
		recState = 1;
		// пофиг на переполенние по приему (выбираем только начало данных)
		__HAL_UART_CLEAR_OREFLAG(&huart1);

		HAL_UART_Receive_IT(&huart1, &tmp, 1);
	} else if (recState == 1) {
		//IDLE на линии и ненулевое количество принятых байт - конец команды.
		if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) && countRecByte > 0)
			recState = 2;

	} else if (recState == 2) {
		addCommand((Command*)recBuf, countRecByte);
		HAL_UART_Transmit_IT(&huart1, recBuf, countRecByte);
		countRecByte = 0;
		recState = 0;

	} else if (recState == 3){
		recBuf[countRecByte++] = tmp;
		recState = 0;
	}
}

static inline void DMXPort(uint32_t curTime, uint32_t* lastTime, uint8_t* updateBulbs, BulbsGroup* bulbsData) {

	uint8_t *firstPartData, *secondPartData;
	uint16_t firstPartLength, secondPartLength;
	uint8_t startByte = 0;

	firstPartData = (uint8_t*) &(bulbsData->bulbs[bulbsData->curPos]);
	firstPartLength = (bulbsData->countBulbs - bulbsData->curPos) * sizeof(Bulb);
	secondPartData = (uint8_t*) &(bulbsData->bulbs[0]);
	secondPartLength = bulbsData->curPos * sizeof(Bulb);

	if (dmxState == 0) {
		if (curTime - *lastTime > 1000) { // отправляем через в 10мс
			*lastTime = curTime;
			dmxState = 1;
		}
	} else if (dmxState == 1) {
		// отправляем break
		dmxState = 2;
		dmx_pin_break();
	} else if (dmxState == 2) {
		if (curTime > *lastTime) {
			dmxState = 3;
			*lastTime = curTime;
			dmx_pin_uart();
		}
	//стартовый байт
	} else if (dmxState == 3) {
			dmxState = 4;
			*lastTime = curTime;
			HAL_UART_Transmit_IT(&huart2, &startByte, 1);
	} else if (dmxState == 4) {
		if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC)) {
			*lastTime = curTime;
			dmxState = 5;
		}
	//прожекторы - первая часть узора
	} else if (dmxState == 5) {
			dmxState = 6;
			*lastTime = curTime;
			HAL_UART_Transmit_IT(&huart2, firstPartData, firstPartLength);
	} else if (dmxState == 6) {
		if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC)) {
			*lastTime = curTime;
			dmxState = 7;
		}
	//вторая
	} else if (dmxState == 7) {
			dmxState = 8;
			*lastTime = curTime;
			HAL_UART_Transmit_IT(&huart2, secondPartData, secondPartLength);
	} else if (dmxState == 8) {
		if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC)) {
			*lastTime = curTime;
			*updateBulbs = 0;
			dmxState = 0;
		}
	}
}

int main(void) {

	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();

	dmx_pin_uart();

	uint32_t lastTime = HAL_GetTick();
	uint32_t curTime = lastTime;
	uint8_t updateBulbs = 0;

	//uint8_t beginSecondPart = 0;
	BulbsGroup* bulbsData = &lwrBulbs;
	//DEBUG
	uint8_t i = 0;
	uint8_t count = 10;
	uint8_t stp = 1;
	//bulbsData->curPos = 10;

	while (1) {

		digitalInputs();

		generalPort();

		if(!updateBulbs) {
			queueItem* QE = dequeuingCommand();
			if(QE) updateBulbs = cmdRoutine(&QE->cmd, QE->size);
		}

		curTime = HAL_GetTick();

//		//debug
//		i = R_RING_SHIFT(i, stp, count);

		if(updateBulbs) DMXPort(curTime, &lastTime, &updateBulbs, bulbsData);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	//байт получен
	recState = 3;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
	//
	recState = 0;
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USART1 init function */
void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart1);

}

/* USART2 init function */
void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 250000;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_2;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart2);
}

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
     PA2   ------> USART2_TX
     PA9   ------> USART1_TX
     PA10   ------> USART1_RX
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOD_CLK_ENABLE();
  __GPIOA_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();

	/*
	// делается в HAL_UART_MspInit
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  */

	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */

/**
  * @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

