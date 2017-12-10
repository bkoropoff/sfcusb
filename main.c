#include "includes.h"
#include "util.h"
#include "usb.h"
#include "sfc.h"

/* Magic address and value to cause SparkFun bootloader to stay in serial mode */
#define SPARK_MAGIC_ADDR 0x0800
#define SPARK_MAGIC_VALUE 0x77

#define BOOTLOADER_RESET 0x3800


static void
setup(void)
{
    uint16_t state;

    sfc_init();

    state = sfc_read();
    if ((state & SFC_BOOTLOADER) == SFC_BOOTLOADER)
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

    for (;;)
    {
        SLEEP();
    }
}
