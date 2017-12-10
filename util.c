#include "includes.h"
#include "util.h"

#define BLINK_DELAY_MS 200

void
panic(uint8_t code)
{
    cli();
    LED1_CONF();
    LED2_CONF();

    for (;;)
    {
        for (int i = sizeof(code) * 8 - 1; i >= 0; --i)
        {
            if ((code >> i) & 1)
            {
                LED1_ON();
                _delay_ms(BLINK_DELAY_MS);
                LED1_OFF();
            }
            else
            {
                LED2_ON();
                _delay_ms(BLINK_DELAY_MS);
                LED2_OFF();
            }
            _delay_ms(BLINK_DELAY_MS);
        }
        _delay_ms(BLINK_DELAY_MS * 5);
    }
}
