#ifndef INC_DMX_H_
#define INC_DMX_H_

#include "stm32f1xx_hal.h"

#define MAX_COUNT_BULBS 	(uint8_t) 128
#define MAX_LENGTH_QUEUE 	(uint8_t) 10

typedef enum {
    PLAIN_ROW = 1,         		// Несжатая строка
    PACKED_ROW = 2,     		// Сжатая строка
    SET_BULBS_COUNT = 3, 		// Установить количество светильников
    SET_BULB_COLOR = 4,   		// Установить цвет конкретного светильника
    SET_TICKER = 5,         	// Задать эффект бегущей строки
    START_PLAY = 6,         	// Начать работу
    STOP_PLAY = 7,              // Остановить работу
} CommandsType;

typedef enum {
	ENABLE_TKR,
	DISABLE_TKR,
} TickerState;

typedef enum {
	UPPER = 0,
	LOWER = 1,
} Level;

typedef struct {
	uint8_t upperBulbsCount; 	// Количество верхних фонарей
 	uint8_t lowerBulbsCount; 	// Количество нижних фонарей
} SetBulbsCountCmd;

typedef struct {
	uint8_t level;             	// Верхний или нижний ряд фонарей (lower - 1, hi - 0)
 	uint8_t bulbNUmber; 		// Номер фонаря
 	uint8_t red;
 	uint8_t green;
 	uint8_t blue;
} SetBulbColorCmd;

typedef struct {
	uint8_t level;             	// Верхний или нижний ряд фонарей
	uint8_t direction;    		// Направление
	uint8_t shift;             	// Шаг сдвига
	uint8_t rowCount;  			// Сдвигать каждые rowCount строк
} SetTickerCmd;

typedef struct {
} StartPlayCommand;

typedef struct {
} StopPlayCommand;

typedef struct {
    uint8_t type;
    uint8_t length;
    union {
        SetBulbsCountCmd bulbsCount;
        SetBulbColorCmd  bulbColor;
        SetTickerCmd     ticker;
        StartPlayCommand start;
        StopPlayCommand  stop;
    } data;
} Command;

typedef struct {
 	uint8_t red;
 	uint8_t green;
 	uint8_t blue;
 	uint8_t strobo;
} Bulb;

typedef struct {
	uint8_t direction;    		// Направление
	uint8_t stepShift;          // Шаг сдвига
	uint8_t countRow;  			// Сдвигать каждые rowCount строк
} SetTicker;

typedef struct {
	uint8_t countBulbs;
	uint8_t curPos;
	Bulb bulbs[MAX_COUNT_BULBS];
} BulbsGroup;

typedef struct {
	Command cmd;
	uint16_t size;
} queueItem;

typedef struct {
	uint8_t lengthQueue;
	uint8_t firstCmd;
	uint8_t nextCmd;
	queueItem commandBuffer[MAX_LENGTH_QUEUE];
} ComandQueue;

#define RINGINC(i, MAX) ((uint8_t)(i + 1) < MAX) ? i + 1 : i + (~MAX + (uint8_t) 1)
#define L_RING_SHIFT(i, stp, MAX) ((uint8_t)(i - stp) < MAX) ? \
									(uint8_t) i - stp : \
									(uint8_t) i - stp - (~MAX + (uint8_t) 1)
#define R_RING_SHIFT(i, stp, MAX) ((uint8_t)(i + stp) < MAX) ? \
									(uint8_t)i + stp : \
									(uint8_t)i + stp + (~MAX + (uint8_t) 1)

#endif /* INC_DMX_H_ */
