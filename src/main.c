
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/lookup.h"
#include "../inc/motion.h"
#include "../inc/sensor.h"
#include "../inc/eeprom.h"
#include "../inc/exploration.h"
#include "../inc/uart.h"

#ifdef __MM_COMP_AVR__
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <avr/sleep.h>
	//#include "../inc/uart.h"	/* for robot with uart communication */
#endif

extern DiffSteer_t DS;
extern Sensor_t sensors[3];
extern volatile BOOL sensorsReady;

volatile BOOL start = FALSE;

//static FILE mystdout = FDEV_SETUP_STREAM(uart_putc, uart_getc, _FDEV_SETUP_WRITE);

ISR(INT1_vect)	// button 1 ISR
{
	start = TRUE;
	PORTA |= 0x44;
}

void InitButtons()
{
	EICRA |= _BV(ISC11);	// falling edge interrupt
	EIMSK |= _BV(INT1);	// enable interrupt
}

void InitMotionControl()
{
	InitMotionISR();
	DSInit(&DS);
}

void InitSensors()
{
	/* Initialize sensors, LEDs ports */
	DDRC |= 0x77;	// PC0-PC2, PC4-PC6 outputs
	PORTC = 0x77;	// enable sensors, turn off LEDs

	InitSensor(&sensors[SENSOR_LEFT], SENSOR_LEFT);
	InitSensor(&sensors[SENSOR_CENTER], SENSOR_CENTER);
	InitSensor(&sensors[SENSOR_RIGHT], SENSOR_RIGHT);
	InitSensorISR();

	PORTC = 0x07;
}

/* Preloads a map into EEPROM (for testing) */
void WriteMapToEEPROM()
{
	EEPROM_write(0, 0xF3);
	EEPROM_write(1, 0x3B);
	EEPROM_write(2, 0xE8);
	EEPROM_write(3, 0x7F);
	EEPROM_write(4, 0x70);
	EEPROM_write(5, 0x0B);
	EEPROM_write(6, 0x28);
	EEPROM_write(7, 0x1B);
	EEPROM_write(8, 0x10);
	EEPROM_write(9, 0x1F);
	EEPROM_write(10, 0xFC);
	EEPROM_write(11, 0x0F);
	EEPROM_write(12, 0x80);
	EEPROM_write(13, 0x07);
	EEPROM_write(14, 0x03);
	EEPROM_write(15, 0x00);

	EEPROM_write(16, 0x88);
	EEPROM_write(17, 0x01);
	EEPROM_write(18, 0x10);
	EEPROM_write(19, 0x01);
	EEPROM_write(20, 0xE0);
	EEPROM_write(21, 0x4B);
	EEPROM_write(22, 0xC4);
	EEPROM_write(23, 0x08);
	EEPROM_write(24, 0x40);
	EEPROM_write(25, 0x1D);
	EEPROM_write(26, 0xA0);
	EEPROM_write(27, 0x3B);
	EEPROM_write(28, 0xDA);
	EEPROM_write(29, 0x37);

	EEPROM_write(30, 0x7C);
	EEPROM_write(31, 0x7E);
	EEPROM_write(32, 0xD6);
	EEPROM_write(33, 0x36);
	EEPROM_write(34, 0x6F);
	EEPROM_write(35, 0x5F);
	EEPROM_write(36, 0xD0);
	EEPROM_write(37, 0x7A);
	EEPROM_write(38, 0x40);
	EEPROM_write(39, 0x1D);
	EEPROM_write(40, 0x94);
	EEPROM_write(41, 0x23);
	EEPROM_write(42, 0xB8);
	EEPROM_write(43, 0x86);
	EEPROM_write(44, 0x00);
	EEPROM_write(45, 0x00);

	EEPROM_write(46, 0x80);
	EEPROM_write(47, 0x31);
	EEPROM_write(48, 0x02);
	EEPROM_write(49, 0x5B);
	EEPROM_write(50, 0x00);
	EEPROM_write(51, 0x03);
	EEPROM_write(52, 0xC0);
	EEPROM_write(53, 0x07);
	EEPROM_write(54, 0xF8);
	EEPROM_write(55, 0x1F);
	EEPROM_write(56, 0xF8);
	EEPROM_write(57, 0x3B);
	EEPROM_write(58, 0xFE);
	EEPROM_write(59, 0xAF);
}

int main(int argc, char *argv[])
{
//	WriteMapToEEPROM();

	InitButtons();
	InitMotionControl();
	InitSensors();

//	uart_init(UART_BAUD_SELECT(9600, 16000000UL));
//	stdout = &mystdout;

	sei();				// enable global interrupts

	/* wait until sensor initialization is done */
	while (!sensorsReady);

	/* wait for start button to be pressed */
	while (!start);

	/* start exploring */
	explore();

	return 0;
}
