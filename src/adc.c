
#include <avr/io.h>
#include <avr/interrupt.h>
#include "../inc/common.h"
#include "../inc/adc.h"

void InitADC()
{
	U8 dummy;	// used for dummy read
	static BOOL initDone = FALSE;

	if (!initDone)
	{
		ADMUX |= _BV(REFS1) | _BV(REFS0);	// use internal 2.56V reference
		ADMUX |= _BV(ADLAR);				// left adjust result for 8 bits usage
		ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);	// prescale 128
		ADCSRA |= _BV(ADEN);				// enable ADC

		ADCSRA |= _BV(ADSC);				// starts/init first conversion
		while (ADCSRA & _BV(ADSC));			// wait for conversion to complete (25 cycles)
		ADCSRA |= _BV(ADIF);				// clear flag, conversions now take 13 cycles
		dummy = ADCL;
		dummy = ADCH;

//		ADCSRA |= _BV(ADIE);				// enable ADC interrupt
//		ADCSRA &= ~_BV(ADIF); 				// clear ADC interrupt flag
		initDone = TRUE;
	}
}
