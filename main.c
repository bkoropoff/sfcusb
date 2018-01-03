#include "includes.h"
#include "util.h"
#include "usb.h"
#if defined(CONFIG_SFC)
#include "sfc.h"
#elif defined(CONFIG_SS)
#include "ss.h"
#elif defined(CONFIG_ARCADE)
#include "arcade.h"
#endif

/* Magic address and value to cause SparkFun bootloader to stay in serial mode */
#define SPARK_MAGIC_ADDR 0x0800
#define SPARK_MAGIC_VALUE 0x77

#define BOOTLOADER_RESET 0x3800

static void
setup(void)
{
    uint16_t state;

    controller_init();

    state = controller_read();
    if ((state & CTLR_BOOTLOADER) == CTLR_BOOTLOADER)
    {
        *((uint8_t*) SPARK_MAGIC_ADDR) = SPARK_MAGIC_VALUE;
        goto *(void*) BOOTLOADER_RESET;
    }

    cli();
    usb_init();
    sei();
}

int
main(void)
{
    setup();

#ifdef CONFIG_ARCADE
    controller_poll();
#else
    for (;;)
    {
        SLEEP();
    }
#endif
}
