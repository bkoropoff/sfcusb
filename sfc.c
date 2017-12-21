#include "includes.h"
#include "util.h"
#include "sfc.h"

#define CTLR_DDR DDRB
#define CTLR_OUT_REG PORTB
#define CTLR_IN_REG PINB
#define CTLR_CLK PORTB1
#define CTLR_CLKDD DDB1
#define CTLR_LATCH PORTB3
#define CTLR_LATCHDD DDB3
#define CTLR_DATA PINB2
#define CTLR_DATADD DDB2

/* Timings in microseconds */
#define CTLR_LATCH_PULSE 12
#define CTLR_CLK_HALF_CYCLE 6

void
controller_init(void)
{
    /* Latch and clock are outputs */
    SET(CTLR_DDR, CTLR_LATCHDD);
    SET(CTLR_DDR, CTLR_CLKDD);
    /* Data is input */
    CLEAR(CTLR_DDR, CTLR_DATADD);
    CLEAR(CTLR_DDR, CTLR_DATADD);
    /* Clock is default high */
    SET(CTLR_OUT_REG, CTLR_CLK);
}

uint16_t
controller_read(void)
{
    uint8_t i;
    uint16_t raw = 0;

    /* Send latch pulse */
    SET(CTLR_OUT_REG, CTLR_LATCH);
    _delay_us(CTLR_LATCH_PULSE);
    CLEAR(CTLR_OUT_REG, CTLR_LATCH);

    /* Clock all button states */
    for (i = 0; i < 16; ++i)
    {
        _delay_us(CTLR_CLK_HALF_CYCLE);
        CLEAR(CTLR_OUT_REG, CTLR_CLK);
        raw = (raw << 1) | (TEST(CTLR_IN_REG, CTLR_DATA) ? 1 : 0);
        _delay_us(CTLR_CLK_HALF_CYCLE);
        SET(CTLR_OUT_REG, CTLR_CLK);
    }

    /* Data states are inverted */
    return ~raw;
}
