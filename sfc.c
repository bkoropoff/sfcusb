#include "includes.h"
#include "util.h"
#include "sfc.h"

#define SFC_DDR DDRB
#define SFC_OUT_REG PORTB
#define SFC_IN_REG PINB
#define SFC_CLK PORTB1
#define SFC_CLKDD DDB1
#define SFC_LATCH PORTB3
#define SFC_LATCHDD DDB3
#define SFC_DATA PINB2
#define SFC_DATADD DDB2

/* Timings in microseconds */
#define SFC_LATCH_PULSE 12
#define SFC_CLK_HALF_CYCLE 6

void
sfc_init(void)
{
    /* Latch and clock are outputs */
    SET(SFC_DDR, SFC_LATCHDD);
    SET(SFC_DDR, SFC_CLKDD);
    /* Data is input */
    CLEAR(SFC_DDR, SFC_DATADD);
    CLEAR(SFC_DDR, SFC_DATADD);
    /* Clock is default high */
    SET(SFC_OUT_REG, SFC_CLK);
}

uint16_t
sfc_read(void)
{
    uint8_t i;
    uint16_t raw = 0;

    /* Send latch pulse */
    SET(SFC_OUT_REG, SFC_LATCH);
    _delay_us(SFC_LATCH_PULSE);
    CLEAR(SFC_OUT_REG, SFC_LATCH);

    /* Clock all button states */
    for (i = 0; i < 16; ++i)
    {
        _delay_us(SFC_CLK_HALF_CYCLE);
        CLEAR(SFC_OUT_REG, SFC_CLK);
        raw = (raw << 1) | (TEST(SFC_IN_REG, SFC_DATA) ? 1 : 0);
        _delay_us(SFC_CLK_HALF_CYCLE);
        SET(SFC_OUT_REG, SFC_CLK);
    }

    /* Data states are inverted */
    return ~raw;
}
