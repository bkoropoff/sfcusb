#include "includes.h"
#include "util.h"
#include "arcade.h"

void
controller_init(void)
{
    /* All inputs are pulled up when open and pulled to ground when
       closed */

    /* Configure joystick pins */
    CLEAR(DDRD, DDD1);
    SET(PORTD, PORTD1);
    CLEAR(DDRD, DDD0);
    SET(PORTD, PORTD0);
    CLEAR(DDRD, DDD4);
    SET(PORTD, PORTD4);
    CLEAR(DDRC, DDC6);
    SET(PORTC, PORTC6);

    /* Configure button pins */
    CLEAR(DDRF, DDF4);
    SET(PORTF, PORTF4);
    CLEAR(DDRF, DDF5);
    SET(PORTF, PORTF5);
    CLEAR(DDRF, DDF6);
    SET(PORTF, PORTF6);
    CLEAR(DDRF, DDF7);
    SET(PORTF, PORTF7);
    CLEAR(DDRB, DDB1);
    SET(PORTB, PORTB1);
    CLEAR(DDRB, DDB3);
    SET(PORTB, PORTB3);
    CLEAR(DDRB, DDB2);
    SET(PORTB, PORTB2);
    CLEAR(DDRB, DDB6);
    SET(PORTB, PORTB6);
}

uint16_t
controller_read(void)
{
    uint16_t raw = 0;
    uint8_t d = PIND;
    uint8_t c = PINC;
    uint8_t f = PINF;
    uint8_t b = PINB;

    raw |= TEST(d, PIND1) ? (1 << CTLR_LEFT) : 0;
    raw |= TEST(d, PIND0) ? (1 << CTLR_RIGHT) : 0;
    raw |= TEST(d, PIND4) ? (1 << CTLR_DOWN) : 0;
    raw |= TEST(c, PINC6) ? (1 << CTLR_UP) : 0;
    raw |= TEST(f, 4) ? (1 << CTLR_COIN) : 0;
    raw |= TEST(f, 5) ? (1 << CTLR_START) : 0;
    raw |= TEST(f, 6) ? (1 << CTLR_A) : 0;
    raw |= TEST(f, 7) ? (1 << CTLR_B) : 0;
    raw |= TEST(b, 1) ? (1 << CTLR_C) : 0;
    raw |= TEST(b, 3) ? (1 << CTLR_D) : 0;
    raw |= TEST(b, 2) ? (1 << CTLR_E) : 0;
    raw |= TEST(b, 6) ? (1 << CTLR_F) : 0;

    return ~raw;
}

#define HISTORY_SIZE 8
#define HISTORY_INTERVAL 125

static uint16_t last_debounced = 0;
static uint16_t history[HISTORY_SIZE];
static uint8_t history_index = 0;

uint16_t
controller_read_debounced(void)
{
    uint8_t i;
    uint16_t changed = 0xFFFF;

    for (i = 0; i < HISTORY_SIZE; ++i)
    {
        changed &= last_debounced ^ history[i];
    }

    last_debounced ^= changed;
    return last_debounced;
}

void
controller_poll(void)
{
    for (;;)
    {
        history[history_index++ % HISTORY_SIZE] = controller_read();
        _delay_us(HISTORY_INTERVAL);
    }
}
