#include "includes.h"
#include "util.h"
#include "ss.h"

#define CTLR_DATA_DDR DDRF
#define CTLR_DATA_DD0 DDF4
#define CTLR_DATA_REG PINF
#define CTLR_DATA_0 PINF4
#define CTLR_SEL_DDR DDRB
#define CTLR_SEL_DD0 DDB3
#define CTLR_SEL_DD1 DDB1
#define CTLR_SEL_REG PORTB
#define CTLR_SEL_0 PORTB3
#define CTLR_SEL_1 PORTB1

/* Timings in microseconds */
#define CTLR_DELAY 6

void
controller_init(void)
{
    /* Select is output */
    SET(CTLR_SEL_DDR, CTLR_SEL_DD0);
    SET(CTLR_SEL_DDR, CTLR_SEL_DD1);
    /* Data is input */
    WRITE(CTLR_DATA_DDR, CTLR_DATA_DD0, 4, 0);
}

static inline
uint8_t
controller_data(void)
{
    return (CTLR_DATA_REG >> CTLR_DATA_0) & 0xF;
}

uint16_t
controller_read(void)
{
    uint16_t raw = 0;

    CLEAR(CTLR_SEL_REG, CTLR_SEL_0);
    CLEAR(CTLR_SEL_REG, CTLR_SEL_1);
    _delay_us(CTLR_DELAY);
    raw |= controller_data();

    SET(CTLR_SEL_REG, CTLR_SEL_0);
    CLEAR(CTLR_SEL_REG, CTLR_SEL_1);
    _delay_us(CTLR_DELAY);
    raw |= controller_data() << 4;

    CLEAR(CTLR_SEL_REG, CTLR_SEL_0);
    SET(CTLR_SEL_REG, CTLR_SEL_1);
    _delay_us(CTLR_DELAY);
    raw |= controller_data() << 8;

    SET(CTLR_SEL_REG, CTLR_SEL_0);
    SET(CTLR_SEL_REG, CTLR_SEL_1);
    _delay_us(CTLR_DELAY);
    raw |= controller_data() << 12;

    return ~raw;
}
