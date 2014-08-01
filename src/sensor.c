#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "../inc/common.h"
#include "../inc/adc.h"
#include "../inc/sensor.h"
#include "../inc/cqueue.h"
#include "../inc/uart.h"

Sensor_t sensors[3];
volatile BOOL sensorsReady = FALSE;

void InitSensor(Sensor_t* s, U8 channel)
{
	InitADC();

	/* set sensor channel */
	s->channel = channel;

	// calibrate sensor distance
	ADMUX = (ADMUX & 0xF0) | (s->channel & 0x0F);
	while(ADCSRA & _BV(ADSC));	// wait for any active conversion
	ADCSRA |= _BV(ADSC);		// start conversion
	while(ADCSRA & _BV(ADSC));	// wait for conversion to finish
	s->base = ADCH;				// store base sensor distance
	s->history[0] = s->history[1] = s->base;	// initialize history
	s->history[2] = s->history[3] = s->base;	//  to base reading
}

void InitSensorISR()
{
	TCCR3B = _BV(CS30) | _BV(CS32);		// prescale 1024
	OCR3A = TCNT3 + 780;	// 50ms period
	ETIMSK = _BV(OCIE3A);	// enable OC3A interrupt
}

U8 averageADC(Sensor_t* s)
{
	U8 average;
	U8 oldSREG = SREG;

	cli();	// make atomic
	//average = (s->history[3] + s->history[2]) >> 1;
	average = s->history[3];	// return most recent value for faster response

	SREG = oldSREG;	// restore SREG, (therefore restoring previous interrupt state)

	return average;
}

/* read value from sensor and store it in the sensor's data array */
void ReadSensor(Sensor_t* s)
{
	ADMUX = (ADMUX & 0xF0) | (s->channel & 0x0F);	// select the channel

	while(ADCSRA & _BV(ADSC));	// wait for any active conversion
	ADCSRA |= _BV(ADSC);		// start conversion
	while(ADCSRA & _BV(ADSC));	// wait for conversion to finish

	if (!sensorsReady) {
		s->history[0] = s->history[1] = s->history[2] = s->history[3] = ADCH;
	} else {
		s->history[0] = s->history[1];	// store reading into history
		s->history[1] = s->history[2];
		s->history[2] = s->history[3];
		s->history[3] = ADCH;
	}

//	printf("Channel %d: %d\n", s->channel, s->history[3]);

	// turn on/off led
	switch(s->channel)
	{
	case SENSOR_LEFT:
		if (averageADC(s) > 70)
			PORTC &= ~0x01;
		else
			PORTC |= 0x01;
		break;
	case SENSOR_CENTER:
		if (averageADC(s) > 60)
			PORTC &= ~0x02;
		else
			PORTC |= 0x02;
		break;
	case SENSOR_RIGHT:
		if (averageADC(s) > 70)
			PORTC &= ~0x04;
		else
			PORTC |= 0x04;
		break;
	default: break;
	}
}

/* ISR that reads the sensors every 12.5ms */
ISR(TIMER3_COMPA_vect)
{
	static U8 sensorActive = SENSOR_LEFT;
	static U8 initCount = 0; // count to 2 , then set sensorReady to true
	sei();	// make ISR re-entrant

	if (initCount > 2)	// all sensors initialized
		sensorsReady = TRUE;

	/* read currently active sensor */
	ReadSensor(&sensors[sensorActive]);

	/* turn off sensors */
	PORTC |= 0xF0;

	/* set next sensor to be active */
	sensorActive = (++sensorActive > SENSOR_RIGHT) ? SENSOR_LEFT : sensorActive;

	/* turn on the next sensor to be read */
	switch(sensors[sensorActive].channel)
	{
	case SENSOR_LEFT:
		PORTC &= ~_BV(SENSOR_SW_LEFT);
		break;
	case SENSOR_CENTER:
		PORTC &= ~_BV(SENSOR_SW_CENTER);
		break;
	case SENSOR_RIGHT:
		PORTC &= ~_BV(SENSOR_SW_RIGHT);
		break;
	}

	initCount = (!sensorsReady) ? initCount + 1 : initCount;	// initializing counter

	OCR3A += 700;
}
