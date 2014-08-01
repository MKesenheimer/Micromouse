#include <avr/io.h>
#include "../inc/common.h"
#include "../inc/eeprom.h"

void EEPROM_write(U16 uiAddress, U8 ucData)
{
	/* Wait for completion of previous write */
	while(EECR & _BV(EEWE));
	while(SPMCSR & _BV(SPMEN));
	/* Set up address and data registers */
	EEAR = uiAddress;
	EEDR = ucData;
	/* Write logical one to EEMWE */
	EECR |= _BV(EEMWE);
	/* Start eeprom write by setting EEWE */
	EECR |= _BV(EEWE);
}

U8 EEPROM_read(U16 uiAddress)
{
	/* Wait for completion of previous write */
	while(EECR & _BV(EEWE));
	/* Set up address register */
	EEAR = uiAddress;
	/* Start eeprom read by writing EERE */
	EECR |= _BV(EERE);
	/* Return data from data register */
	return EEDR;
}
