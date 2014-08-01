
#ifndef SENSORS_H_
#define SENSORS_H_

#include "../inc/common.h"

#define SENSOR_CENTER 1
#define SENSOR_LEFT 0
#define SENSOR_RIGHT 2

#define SENSOR_SW_CENTER 5
#define SENSOR_SW_LEFT 6
#define SENSOR_SW_RIGHT 4

/* minimum sensor value to detect a wall */
#define SENSOR_WALLDETECT_CENTER 60
#define SENSOR_WALLDETECT_LEFT 70
#define SENSOR_WALLDETECT_RIGHT 70

/* error correction thresholds */
#define SENSOR_ECC_FRONT 80
#define SENSOR_ECC_MEDIAN_LEFT 100
#define SENSOR_ECC_MEDIAN_RIGHT 100

typedef struct {
	U8 channel;
	volatile U8 history[4];
	U8 base;	// calibrated distance on startup
} Sensor_t;

void InitSensor(Sensor_t*, U8);
void InitSensorISR();
void ReadSensor(Sensor_t*);
void SensorsInterruptHandler();
U8 averageADC(Sensor_t* s);

#endif /* SENSOR_H_ */
