#include "includes.h"
#include "usb.h"

#if defined(CONFIG_SFC)
#include "sfc.h"
#elif defined(CONFIG_SS)
#include "ss.h"
#elif defined(CONFIG_ARCADE)
#include "arcade.h"
#endif

#define USB_LANG_INDEX 0
#define USB_MANU_INDEX 1
#define USB_PROD_INDEX 2

#if defined(CONFIG_SFC)
/* Vendor/ID of board I used for SFC controller */
#define USB_VENDOR_ID 0x1b4f
#define USB_PRODUCT_ID 0x9205
#define USB_MAX_POWER 20
#elif defined(CONFIG_SS)
/* Vendor/ID of board I used for SS controller */
#define USB_VENDOR_ID 0x2341
#define USB_PRODUCT_ID 0x0037
#define USB_MAX_POWER 15
#elif defined(CONFIG_ARCADE)
/* Vendor/ID of board I used for arcade controller */
#define USB_VENDOR_ID 0x2341
#define USB_PRODUCT_ID 0x0038
#define USB_MAX_POWER 15
#endif

static const struct string_descriptor lang_desc =
{
    .length = sizeof(lang_desc) + 1 * sizeof(uint16_t),
    .type = DESC_TYPE_STRING,
    .string = {LANG_ENGLISH}
};

static const struct string_descriptor manu_desc =
{
    .length = sizeof(manu_desc) + 8 * sizeof(uint16_t),
    .type = DESC_TYPE_STRING,
    .string = {'K','o','r','o','p','o','f','f'}
};

#if defined(CONFIG_SFC)
static const struct string_descriptor prod_desc =
{
    .length = sizeof(manu_desc) + 14 * sizeof(uint16_t),
    .type = DESC_TYPE_STRING,
    .string = {'S','F','C',' ','C','o','n','t','r','o','l','l','e','r'}
};
#elif defined(CONFIG_SS)
static const struct string_descriptor prod_desc =
{
    .length = sizeof(manu_desc) + 13 * sizeof(uint16_t),
    .type = DESC_TYPE_STRING,
    .string = {'S','S',' ','C','o','n','t','r','o','l','l','e','r'}
};
#elif defined(CONFIG_ARCADE)
static const struct string_descriptor prod_desc =
{
    .length = sizeof(manu_desc) + 6 * sizeof(uint16_t),
    .type = DESC_TYPE_STRING,
    .string = {'A','r','c','a','d','e'}
};
#endif

static const struct string_descriptor* usb_strings[] =
{
    [USB_LANG_INDEX] = &lang_desc,
    [USB_MANU_INDEX] = &manu_desc,
    [USB_PROD_INDEX] = &prod_desc,
};

#define PADDING(c) (8 - ((c) + 4) % 8)

#if defined(CONFIG_SFC)
#ifdef CONFIG_VIRTUAL_BUTTONS
#define BUTTON_COUNT 12
#else
#define BUTTON_COUNT 10
#endif
static const uint8_t usb_report_desc[] =
{
    HRD_USAGE_PAGE_1(HRD_USAGE_PAGE_GENERIC_DESKTOP),
    HRD_USAGE_1(HRD_USAGE_GAMEPAD),
    HRD_COLLECTION(HRD_COL_APPLICATION),
    HRD_USAGE_1(HRD_USAGE_POINTER),
    HRD_COLLECTION(HRD_COL_PHYSICAL),
    HRD_LOGICAL_MIN_1(-1),
    HRD_LOGICAL_MAX_1(1),
    HRD_REPORT_SIZE_1(2),
    HRD_REPORT_COUNT_1(2),
    HRD_USAGE_1(HRD_USAGE_X),
    HRD_USAGE_1(HRD_USAGE_Y),
    /* I tried adding string descriptors, but nothing seems to use
       them and Windows chokes on them. */
    HRD_INPUT_1(HRD_BIT_VARIABLE),
    HRD_END_COLLECTION,
    HRD_LOGICAL_MIN_1(0),
    HRD_LOGICAL_MAX_1(1),
    HRD_USAGE_PAGE_1(HRD_USAGE_PAGE_BUTTON),
    HRD_USAGE_MIN_1(1),
    HRD_USAGE_MAX_1(BUTTON_COUNT),
    HRD_REPORT_SIZE_1(1),
    HRD_REPORT_COUNT_1(BUTTON_COUNT),
    HRD_INPUT_1(HRD_BIT_VARIABLE),
#if PADDING(BUTTON_COUNT)
    /* Pad to a multiple of 8 bits, or Windows won't accept device */
    HRD_REPORT_SIZE_1(1),
    HRD_REPORT_COUNT_1(PADDING(BUTTON_COUNT)),
    HRD_INPUT_1(HRD_BIT_CONSTANT | HRD_BIT_VARIABLE),
#endif
    HRD_END_COLLECTION
};
#elif defined(CONFIG_SS)
#ifdef CONFIG_VIRTUAL_BUTTONS
#define BUTTON_COUNT 11
#else
#define BUTTON_COUNT 9
#endif
static const uint8_t usb_report_desc[] =
{
    HRD_USAGE_PAGE_1(HRD_USAGE_PAGE_GENERIC_DESKTOP),
    HRD_USAGE_1(HRD_USAGE_GAMEPAD),
    HRD_COLLECTION(HRD_COL_APPLICATION),
    HRD_USAGE_1(HRD_USAGE_POINTER),
    HRD_COLLECTION(HRD_COL_PHYSICAL),
    HRD_LOGICAL_MIN_1(-1),
    HRD_LOGICAL_MAX_1(1),
    HRD_REPORT_SIZE_1(2),
    HRD_REPORT_COUNT_1(2),
    HRD_USAGE_1(HRD_USAGE_X),
    HRD_USAGE_1(HRD_USAGE_Y),
    HRD_INPUT_1(HRD_BIT_VARIABLE),
    HRD_END_COLLECTION,
    HRD_LOGICAL_MIN_1(0),
    HRD_LOGICAL_MAX_1(1),
    HRD_USAGE_PAGE_1(HRD_USAGE_PAGE_BUTTON),
    HRD_USAGE_MIN_1(1),
    HRD_USAGE_MAX_1(BUTTON_COUNT),
    HRD_REPORT_SIZE_1(1),
    HRD_REPORT_COUNT_1(BUTTON_COUNT),
    HRD_INPUT_1(HRD_BIT_VARIABLE),
#if PADDING(BUTTON_COUNT)
    /* Pad to a multiple of 8 bits, or Windows won't accept device */
    HRD_REPORT_SIZE_1(1),
    HRD_REPORT_COUNT_1(PADDING(BUTTON_COUNT)),
    HRD_INPUT_1(HRD_BIT_CONSTANT | HRD_BIT_VARIABLE),
#endif
    HRD_END_COLLECTION
};
#elif defined(CONFIG_ARCADE)
#ifdef CONFIG_VIRTUAL_BUTTONS
#define BUTTON_COUNT 12
#else
#define BUTTON_COUNT 8
#endif
static const uint8_t usb_report_desc[] =
{
    HRD_USAGE_PAGE_1(HRD_USAGE_PAGE_GENERIC_DESKTOP),
    HRD_USAGE_1(HRD_USAGE_JOYSTICK),
    HRD_COLLECTION(HRD_COL_APPLICATION),
    HRD_USAGE_1(HRD_USAGE_POINTER),
    HRD_COLLECTION(HRD_COL_PHYSICAL),
    HRD_LOGICAL_MIN_1(-1),
    HRD_LOGICAL_MAX_1(1),
    HRD_REPORT_SIZE_1(2),
    HRD_REPORT_COUNT_1(2),
    HRD_USAGE_1(HRD_USAGE_X),
    HRD_USAGE_1(HRD_USAGE_Y),
    HRD_INPUT_1(HRD_BIT_VARIABLE),
    HRD_END_COLLECTION,
    HRD_LOGICAL_MIN_1(0),
    HRD_LOGICAL_MAX_1(1),
    HRD_USAGE_PAGE_1(HRD_USAGE_PAGE_BUTTON),
    HRD_USAGE_MIN_1(1),
    HRD_USAGE_MAX_1(BUTTON_COUNT),
    HRD_REPORT_SIZE_1(1),
    HRD_REPORT_COUNT_1(BUTTON_COUNT),
    HRD_INPUT_1(HRD_BIT_VARIABLE),
#if PADDING(BUTTON_COUNT)
    /* Pad to a multiple of 8 bits, or Windows won't accept device */
    HRD_REPORT_SIZE_1(1),
    HRD_REPORT_COUNT_1(PADDING(BUTTON_COUNT)),
    HRD_INPUT_1(HRD_BIT_CONSTANT | HRD_BIT_VARIABLE),
#endif
    HRD_END_COLLECTION
};
#endif

#ifdef CONFIG_SFC
static uint8_t
usb_report(void* out)
{
    uint16_t state = controller_read();
    uint16_t horiz = state & ((1 << CTLR_LEFT) | (1 << CTLR_RIGHT));
    uint16_t vert = state & ((1 << CTLR_UP) | (1 << CTLR_DOWN));
    uint16_t report = 0;

    switch (horiz)
    {
    case (1 << CTLR_LEFT):
        report |= 0x3;
        break;
    case (1 << CTLR_RIGHT):
        report |= 0x1;
        break;
    }

    switch (vert)
    {
    case (1 << CTLR_UP):
        report |= 0x3 << 2;
        break;
    case (1 << CTLR_DOWN):
        report |= 0x1 << 2;
        break;
    }

    report |= (TEST(state, CTLR_B) ? 1 : 0) << 4;
    report |= (TEST(state, CTLR_Y) ? 1 : 0) << 5;
    report |= (TEST(state, CTLR_A) ? 1 : 0) << 8;
    report |= (TEST(state, CTLR_X) ? 1 : 0) << 9;
    report |= (TEST(state, CTLR_L) ? 1 : 0) << 10;
    report |= (TEST(state, CTLR_R) ? 1 : 0) << 11;

    if (TEST(state, CTLR_SELECT))
    {
#ifdef CONFIG_VIRTUAL_BUTTONS
        if (TEST(state, CTLR_UP))
        {
            /* Virtual button: up + select */
            report |= 1 << 12;
        }
        else if (TEST(state, CTLR_DOWN))
        {
            /* Virtual button: down + select */
            report |= 1 << 13;
        }
        else
#endif
        {
            /* Select */
            report |= 1 << 6;
        }
    }

    if (TEST(state, CTLR_START))
    {
#ifdef CONFIG_VIRTUAL_BUTTONS
        if (TEST(state, CTLR_UP))
        {
            /* Virtual button: up + start */
            report |= 1 << 14;
        }
        else if (TEST(state, CTLR_DOWN))
        {
            /* Virtual button: down + start */
            report |= 1 << 15;
        }
        else
#endif
        {
            /* Start */
            report |= 1 << 7;
        }
    }

    memcpy(out, &report, sizeof(report));
    return sizeof(report);
}
#elif defined(CONFIG_SS)
static uint8_t
usb_report(void* out)
{
    uint16_t state = controller_read();
    uint16_t horiz = state & ((1 << CTLR_LEFT) | (1 << CTLR_RIGHT));
    uint16_t vert = state & ((1 << CTLR_UP) | (1 << CTLR_DOWN));
    uint16_t report = 0;

    switch (horiz)
    {
    case (1 << CTLR_LEFT):
        report |= 0x3;
        break;
    case (1 << CTLR_RIGHT):
        report |= 0x1;
        break;
    }

    switch (vert)
    {
    case (1 << CTLR_UP):
        report |= 0x3 << 2;
        break;
    case (1 << CTLR_DOWN):
        report |= 0x1 << 2;
        break;
    }

    report |= (TEST(state, CTLR_Z) ? 1 : 0) << 4;
    report |= (TEST(state, CTLR_Y) ? 1 : 0) << 5;
    report |= (TEST(state, CTLR_X) ? 1 : 0) << 6;
    report |= (TEST(state, CTLR_R) ? 1 : 0) << 7;
    report |= (TEST(state, CTLR_B) ? 1 : 0) << 8;
    report |= (TEST(state, CTLR_C) ? 1 : 0) << 9;
    report |= (TEST(state, CTLR_A) ? 1 : 0) << 10;
    report |= (TEST(state, CTLR_L) ? 1 : 0) << 12;

    if (TEST(state, CTLR_START))
    {
#ifdef CONFIG_VIRTUAL_BUTTONS
        /* Virtual buttons: up/down + start */
        if (TEST(state, CTLR_UP))
        {
            report |= 1 << 13;
        }
        else if (TEST(state, CTLR_DOWN))
        {
            report |= 1 << 14;
        }
        else
#endif
        {
            report |= 1 << 11;
        }
    }

    memcpy(out, &report, sizeof(report));
    return sizeof(report);
}
#elif defined(CONFIG_ARCADE)
static uint8_t
usb_report(void* out)
{
    uint16_t state = controller_read_debounced();
    uint16_t horiz = state & ((1 << CTLR_LEFT) | (1 << CTLR_RIGHT));
    uint16_t vert = state & ((1 << CTLR_UP) | (1 << CTLR_DOWN));
    uint16_t report = 0;

    switch (horiz)
    {
    case (1 << CTLR_LEFT):
        report |= 0x3;
        break;
    case (1 << CTLR_RIGHT):
        report |= 0x1;
        break;
    }

    switch (vert)
    {
    case (1 << CTLR_UP):
        report |= 0x3 << 2;
        break;
    case (1 << CTLR_DOWN):
        report |= 0x1 << 2;
        break;
    }

    if (TEST(state, CTLR_COIN))
    {
#ifdef CONFIG_VIRTUAL_BUTTONS
        if (TEST(state, CTLR_UP))
        {
            /* Virtual button: up + coint */
            report |= 1 << 12;
        }
        else if (TEST(state, CTLR_DOWN))
        {
            /* Virtual button: down + coin */
            report |= 1 << 13;
        }
        else
#endif
        {
            /* Coin */
            report |= 1 << 4;
        }
    }

    if (TEST(state, CTLR_START))
    {
#ifdef CONFIG_VIRTUAL_BUTTONS
        if (TEST(state, CTLR_UP))
        {
            /* Virtual button: up + start */
            report |= 1 << 14;
        }
        else if (TEST(state, CTLR_DOWN))
        {
            /* Virtual button: down + start */
            report |= 1 << 15;
        }
        else
#endif
        {
            /* Start */
            report |= 1 << 5;
        }
    }

    report |= (TEST(state, CTLR_COIN) ? 1 : 0) << 4;
    report |= (TEST(state, CTLR_START) ? 1 : 0) << 5;
    report |= (TEST(state, CTLR_A) ? 1 : 0) << 6;
    report |= (TEST(state, CTLR_B) ? 1 : 0) << 7;
    report |= (TEST(state, CTLR_C) ? 1 : 0) << 8;
    report |= (TEST(state, CTLR_D) ? 1 : 0) << 9;
    report |= (TEST(state, CTLR_E) ? 1 : 0) << 10;
    report |= (TEST(state, CTLR_F) ? 1 : 0) << 11;

    memcpy(out, &report, sizeof(report));
    return sizeof(report);
}
#endif
