#include "includes.h"
#include "util.h"
#include "usb.h"
#include "sfc.h"

#define MAX_CONTROL_SIZE 8
#define MAX_CONTROL_CODE 0
#define MAX_INTERRUPT_SIZE 8
#define MAX_INTERRUPT_CODE 0

PACKED struct usb_request {
    uint8_t type;
    uint8_t request;
    uint16_t value;
    uint16_t index;
    uint16_t length;
};

PACKED struct device_descriptor {
    uint8_t length;
    uint8_t type;
    uint16_t usb;
    uint8_t device_class;
    uint8_t device_sub_class;
    uint8_t device_protocol;
    uint8_t max_packet_size;
    uint16_t vendor_id;
    uint16_t product_id;
    uint16_t device_version;
    uint8_t manufacturer_index;
    uint8_t product_index;
    uint8_t serial_number_index;
    uint8_t num_configurations;
};

CTASSERT(dev_desc_size, sizeof(struct device_descriptor) == 18);

struct config_descriptor {
    uint8_t length;
    uint8_t type;
    uint16_t total_length;
    uint8_t num_interfaces;
    uint8_t config_value;
    uint8_t config_index;
    uint8_t attributes;
    uint8_t max_power;
};

CTASSERT(config_desc_size, sizeof(struct config_descriptor) == 9);

PACKED struct string_descriptor {
    uint8_t length;
    uint8_t type;
    uint16_t string[];
};

PACKED struct interface_descriptor {
    uint8_t length;
    uint8_t type;
    uint8_t interface_number;
    uint8_t alternate_setting;
    uint8_t num_endpoints;
    uint8_t interface_class;
    uint8_t interface_subclass;
    uint8_t interface_protocol;
    uint8_t interface_index;
};

PACKED struct endpoint_descriptor {
    uint8_t length;
    uint8_t type;
    uint8_t address;
    uint8_t attributes;
    uint16_t max_packet_size;
    uint8_t interval;
};

PACKED struct hid_sub_descriptor {
    uint8_t type;
    uint16_t length;
};

PACKED struct hid_descriptor {
    uint8_t length;
    uint8_t type;
    uint16_t hid;
    uint8_t country;
    uint8_t num_descs;
    struct hid_sub_descriptor descs[0];
};

#define REQUEST_GET_REPORT 0x01

#define HRD_SIZE_0 0
#define HRD_SIZE_1 1
#define HRD_SIZE_2 2
#define HRD_SIZE_4 4

#define HRD_TYPE_MAIN 0x0
#define HRD_TYPE_GLOBAL 0x1
#define HRD_TYPE_LOCAL 0x2

#define HRD_TAG_INPUT 0x8
#define HRD_TAG_OUTPUT 0x9
#define HRD_TAG_FEATURE 0xB
#define HRD_TAG_COLLECTION 0xA
#define HRD_TAG_END_COLLECTION 0xC

#define HRD_TAG_USAGE_PAGE 0x0
#define HRD_TAG_LOGICAL_MIN 0x1
#define HRD_TAG_LOGICAL_MAX 0x2
#define HRD_TAG_PHYSICAL_MIN 0x3
#define HRD_TAG_PHYSICAL_MAX 0x4
#define HRD_TAG_UNIT_EXPONENT 0x5
#define HRD_TAG_UNIT 0x6
#define HRD_TAG_REPORT_SIZE 0x7
#define HRD_TAG_REPORT_ID 0x8
#define HRD_TAG_REPORT_COUNT 0x9
#define HRD_TAG_PUSH 0xA
#define HRD_TAG_POP 0xB

#define HRD_TAG_USAGE 0x0
#define HRD_TAG_USAGE_MIN 0x1
#define HRD_TAG_USAGE_MAX 0x2
#define HRD_TAG_DESIGNATOR_INDEX 0x3
#define HRD_TAG_DESIGNATOR_MIN 0x4
#define HRD_TAG_DESIGNATOR_MAX 0x5
#define HRD_TAG_STRING_INDEX 0x7
#define HRD_TAG_STRING_MIN 0x8
#define HRD_TAG_STRING_MAX 0x9
#define HRD_TAG_DELIMETER 0xA

#define HRD_BIT_DATA (0 << 0)
#define HRD_BIT_CONSTANT (1 << 0)
#define HRD_BIT_ARRAY (0 << 1)
#define HRD_BIT_VARIABLE (1 << 1)
#define HRD_BIT_ABSOLUTE (0 << 2)
#define HRD_BIT_RELATIVE (1 << 2)
#define HRD_BIT_NO_WRAP (0 << 3)
#define HRD_BIT_WRAP (1 << 3)
#define HRD_BIT_LINEAR (0 << 4)
#define HRD_BIT_NON_LINEAR (1 << 4)
#define HRD_BIT_PREFERRED_STATE (0 << 5)
#define HRD_BIT_NO_PREFERRED (1 << 5)
#define HRD_BIT_NO_NULL_POSITION (0 << 6)
#define HRD_BIT_NULL_STATE (1 << 6)
#define HRD_BIT_BIT_FIELD (0 << 8)
#define HRD_BIT_BUFFERED_BYTES (1 << 8)

#define HRD_COL_PHYSICAL 0x00
#define HRD_COL_APPLICATION 0x01
#define HRD_COL_REPORT 0x03
#define HRD_COL_NAMED_ARRAY 0x04
#define HRD_COL_USAGE_SWITCH 0x05
#define HRD_COL_USAGE_MODIFIER 0x06

#define HRD_USAGE_PAGE_GENERIC_DESKTOP 0x01
#define HRD_USAGE_POINTER 0x01
#define HRD_USAGE_JOYSTICK 0x04
#define HRD_USAGE_GAMEPAD 0x05
#define HRD_USAGE_X 0x30
#define HRD_USAGE_Y 0x31

#define HRD_USAGE_PAGE_BUTTON 0x09
#define HRD_USAGE_BUTTON_1 0x01
#define HRD_USAGE_BUTTON_8 0x08

#define HRD_ITEM_HEADER(Size, Type, Tag) ((Size) | ((Type) << 2) | ((Tag) << 4))
#define HRD_ITEM_0(Type, Tag) HRD_ITEM_HEADER(0, Type, Tag)
#define HRD_ITEM_1(Type, Tag, Data1) HRD_ITEM_HEADER(1, Type, Tag), (Data1)
#define HRD_ITEM_2(Type, Tag, Data1, Data2) HRD_ITEM_HEADER(2, Type, Tag), (Data1), (Data2)

#define HRD_INPUT_1(Data) HRD_ITEM_1(HRD_TYPE_MAIN, HRD_TAG_INPUT, Data)
#define HRD_COLLECTION(Kind) HRD_ITEM_1(HRD_TYPE_MAIN, HRD_TAG_COLLECTION, Kind)
#define HRD_END_COLLECTION HRD_ITEM_0(HRD_TYPE_MAIN, HRD_TAG_END_COLLECTION)

#define HRD_USAGE_PAGE_1(Page) HRD_ITEM_1(HRD_TYPE_GLOBAL, HRD_TAG_USAGE_PAGE, (Page) & 0xFF)
#define HRD_LOGICAL_MIN_1(Value) HRD_ITEM_1(HRD_TYPE_GLOBAL, HRD_TAG_LOGICAL_MIN, (Value) & 0xFF)
#define HRD_LOGICAL_MAX_1(Value) HRD_ITEM_1(HRD_TYPE_GLOBAL, HRD_TAG_LOGICAL_MAX, (Value) & 0xFF)
#define HRD_REPORT_SIZE_1(Value) HRD_ITEM_1(HRD_TYPE_GLOBAL, HRD_TAG_REPORT_SIZE, (Value) & 0xFF)
#define HRD_REPORT_COUNT_1(Value) HRD_ITEM_1(HRD_TYPE_GLOBAL, HRD_TAG_REPORT_COUNT, (Value) & 0xFF)

#define HRD_USAGE_1(Usage) HRD_ITEM_1(HRD_TYPE_LOCAL, HRD_TAG_USAGE, (Usage) & 0xFF)
#define HRD_USAGE_MIN_1(Usage) HRD_ITEM_1(HRD_TYPE_LOCAL, HRD_TAG_USAGE_MIN, (Usage) & 0xFF)
#define HRD_USAGE_MAX_1(Usage) HRD_ITEM_1(HRD_TYPE_LOCAL, HRD_TAG_USAGE_MAX, (Usage) & 0xFF)
#define HRD_STRING_INDEX(Usage) HRD_ITEM_2(HRD_TYPE_LOCAL, HRD_TAG_STRING_INDEX, (Usage) & 0xFF, ((Usage) >> 8) & 0xFF)
#define HRD_STRING_INDEX_MIN(Usage) HRD_ITEM_2(HRD_TYPE_LOCAL, HRD_TAG_STRING_MIN, (Usage) & 0xFF, ((Usage) >> 8) & 0xFF)
#define HRD_STRING_INDEX_MAX(Usage) HRD_ITEM_2(HRD_TYPE_LOCAL, HRD_TAG_STRING_MAX, (Usage) & 0xFF, ((Usage) >> 8) & 0xFF)

#define REQ_GET_STATUS 0x0
#define REQ_SET_ADDRESS 0x5
#define REQ_GET_DESCRIPTOR 0x6
#define REQ_SET_CONFIGURATION 0x9
#define REQ_SET_IDLE 0xa

#define REQ_TYPE_HOST_TO_DEVICE (0 << 7)
#define REQ_TYPE_DEVICE_TO_HOST (1 << 7)
#define REQ_TYPE_STANDARD (0 << 5)
#define REQ_TYPE_CLASS (1 << 5)
#define REQ_TYPE_VENDOR (2 << 5)
#define REQ_TYPE_DEVICE (0 << 0)
#define REQ_TYPE_INTERFACE (1 << 0)
#define REQ_TYPE_ENDPOINT (2 << 0)

#define DESC_TYPE_DEVICE 0x1
#define DESC_TYPE_CONFIG 0x2
#define DESC_TYPE_STRING 0x3
#define DESC_TYPE_INTERFACE 0x4
#define DESC_TYPE_ENDPOINT 0x5
#define DESC_TYPE_HID 0x21
#define DESC_TYPE_REPORT 0x22
#define DESC_TYPE_PHYSICAL 0x23

#define DEV_CLASS_NONE 0x0
#define DEV_CLASS_HID 0x3

#define LANG_ENGLISH 0x0409

#define ENDPOINT_ADDR(Addr, In) ((Addr) | ((In) ? (1 << 7) : 0))
#define ENDPOINT_TYPE_INTERRUPT 0x3
#define ENDPOINT_SYNC_NONE 0x0
#define ENDPOING_USAGE_DATA 0x0
#define ENDPOINT_ATTR(Type, Sync, Usage) ((Type)|((Sync) << 2)|((Usage) << 4))

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

static const struct string_descriptor prod_desc =
{
    .length = sizeof(manu_desc) + 14 * sizeof(uint16_t),
    .type = DESC_TYPE_STRING,
    .string = {'S','F','C',' ','C','o','n','t','r','o','l','l','e','r'}
};

#define LANG_INDEX 0
#define MANU_INDEX 1
#define PROD_INDEX 2

static const struct string_descriptor* strings[] =
{
    [LANG_INDEX] = &lang_desc,
    [MANU_INDEX] = &manu_desc,
    [PROD_INDEX] = &prod_desc,
};

static const struct device_descriptor dev_desc =
{
    .length = sizeof(dev_desc),
    .type = DESC_TYPE_DEVICE,
    .usb = 0x0110,
    .device_class = DEV_CLASS_NONE,
    .device_sub_class = 0,
    .device_protocol = 0,
    .max_packet_size = MAX_CONTROL_SIZE,
    .vendor_id = 0x1b4f,
    .product_id = 0x9205,
    .device_version = 0x0100,
    .manufacturer_index = MANU_INDEX,
    .product_index = PROD_INDEX,
    .serial_number_index = 0,
    .num_configurations = 1
};

static const uint8_t report_desc[] =
{
    HRD_USAGE_PAGE_1(HRD_USAGE_PAGE_GENERIC_DESKTOP),
    HRD_USAGE_1(HRD_USAGE_GAMEPAD),
    HRD_COLLECTION(HRD_COL_APPLICATION),
    HRD_USAGE_1(HRD_USAGE_POINTER),
    HRD_COLLECTION(HRD_COL_PHYSICAL),
    HRD_LOGICAL_MIN_1(-1),
    HRD_LOGICAL_MAX_1(1),
    /* This could be 2 bits, but Windows' HID driver didn't like it
       until I changed it to 4.  I'm not sure if this is because it
       just doesn't like 2-bit integers, or if it wants the report
       size to be an exact multiple of 8 bits. */
    HRD_REPORT_SIZE_1(4),
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
    HRD_USAGE_MIN_1(HRD_USAGE_BUTTON_1),
    HRD_USAGE_MAX_1(HRD_USAGE_BUTTON_8),
    HRD_REPORT_SIZE_1(1),
    HRD_REPORT_COUNT_1(8),
    HRD_INPUT_1(HRD_BIT_VARIABLE),
    HRD_END_COLLECTION
};

static const struct PACKED
{
    struct config_descriptor config;
    struct interface_descriptor interface;
    struct hid_descriptor hid;
    struct hid_sub_descriptor sub;
    struct endpoint_descriptor endpoint;
} conf_desc =
{
    .config =
    {
        .length = sizeof(conf_desc.config),
        .type = DESC_TYPE_CONFIG,
        .total_length = sizeof(conf_desc),
        .num_interfaces = 1,
        .config_value = 1,
        .config_index = 0,
        .attributes = 1 << 7,
        .max_power = 150 /* 300mA */
    },
    .interface =
    {
        .length = sizeof(conf_desc.interface),
        .type = DESC_TYPE_INTERFACE,
        .interface_number = 0,
        .alternate_setting = 0,
        .num_endpoints = 1,
        .interface_class = DEV_CLASS_HID,
        .interface_subclass = 0,
        .interface_protocol = 0,
        .interface_index = 0
    },
    .hid =
    {
        .length = sizeof(conf_desc.hid) + 1 * sizeof(conf_desc.sub),
        .type = DESC_TYPE_HID,
        .hid = 0x0111,
        .country = 0,
        .num_descs = 1
    },
    .sub =
    {
        .type = DESC_TYPE_REPORT,
        .length = sizeof(report_desc)
    },
    .endpoint =
    {
        .length = sizeof(conf_desc.endpoint),
        .type = DESC_TYPE_ENDPOINT,
        .address = ENDPOINT_ADDR(1, true),
        .attributes = ENDPOINT_ATTR(ENDPOINT_TYPE_INTERRUPT, 0, 0),
        .max_packet_size = 8,
        .interval = 8 /* ~120 times a second */
    }
};

static void
ep0_init(void)
{
    /* Select endpoint 0 */
    UENUM = 0;

    /* Enable it */
    SET(UECONX, EPEN);
    UECFG0X = 0;
    UECFG1X = _BV(ALLOC) | (MAX_CONTROL_CODE << EPSIZE0) | (0x0 << EPBK0);
    /* Check configuration was successful */
    if (!TEST(UESTA0X, CFGOK))
        panic(PANIC_USB_INIT);
    /* Enable interrupts */
    SET(UEIENX, RXSTPE);
}

static void
ep1_init(void)
{
    /* Select endpoint 1 */
    UENUM = 1;

    /* Enable it */
    SET(UECONX, EPEN);
    UECFG0X = (1 << EPDIR) | (3 << EPTYPE0);
    UECFG1X = _BV(ALLOC) | (MAX_INTERRUPT_CODE << EPSIZE0) | (0x0 << EPBK0);
    /* Check configuration was successful */
    if (!TEST(UESTA0X, CFGOK))
        panic(PANIC_USB_INIT);
    /* Enable interrupts */
    SET(UEIENX, TXINE);
}

void
usb_init(void)
{
    /* Enable USB voltage regulator */
    SET(UHWCON, UVREGE);

    /* PLL configuration */

    /* Set USB post-scaler to divide by 2 (24 MHz for low-speed USB) */
    SET(PLLCSR, PINDIV);
    /* Set output frequency to 48 MHz */
    PLLFRQ = (1 << PDIV2);
    /* Enable PLL */
    SET(PLLCSR, PLLE);
    /* Busy-wait for PLL to lock */
    while (!TEST(PLLCSR, PLOCK));

    /* USB configuration */

    /* Turn on USB */
    SET(USBCON, USBE);
    /* Set low-speed mode */
    SET(UDCON, LSM);
    /* Unfreeze clock */
    CLEAR(USBCON, FRZCLK);
    /* Configure endpoints */
    ep0_init();
    ep1_init();
    /* Enable interrupts */
    SET(UDIEN, EORSTE);

    /* Turn on VBUS pad */
    SET(USBCON, OTGPADE);
    /* Wait for VBUS to be available */
    while (!TEST(USBSTA, VBUS));
    /* Attach to host! */
    CLEAR(UDCON, DETACH);
}

static inline uint16_t
usb_fifo_count(void)
{
    return (UEBCHX << 8) | UEBCLX;
}

static inline void
usb_wait_xmit(void)
{
    while (!TEST(UEINTX, TXINI));
}

static inline void
usb_xmit(void)
{
    CLEAR(UEINTX, FIFOCON);
    CLEAR(UEINTX, TXINI);
}

static inline void
usb_wait_out(void)
{
    while (!TEST(UEINTX, RXOUTI));
}

static inline void
usb_ack_out(void)
{
    CLEAR(UEINTX, FIFOCON);
    CLEAR(UEINTX, RXOUTI);
}

static inline void
usb_ack_setup(void)
{
    CLEAR(UEINTX, RXSTPI);
}

static void
usb_read(void* out, uint16_t len)
{
    uint8_t* dest = (uint8_t*) out;

    for (unsigned int i = 0; i < len; ++i)
    {
        dest[i] = UEDATX;
    }
}

static void
usb_write_control(void const* in, uint16_t len)
{
    uint8_t const* src = (uint8_t const*) in;
    uint16_t size = 0;

    while (len)
    {
        usb_wait_xmit();

        size = usb_fifo_count();

        while (len && size < MAX_CONTROL_SIZE)
        {
            UEDATX = *src;
            src++;
            len--;
            size++;
        }

        usb_xmit();
    }

    if (size == MAX_CONTROL_SIZE)
    {
        usb_wait_xmit();
        usb_xmit();
    }
}

static void
usb_write_in(void const* in, uint16_t len)
{
    bool zero = false;
    uint8_t const* src = (uint8_t const*) in;

    while (len)
    {
        usb_wait_xmit();

        CLEAR(UEINTX, TXINI);

        while (len && TEST(UEINTX, RWAL))
        {
            UEDATX = *src;
            src++;
            len--;
        }

        if (len == 0 && !TEST(UEINTX, RWAL))
            zero = true;

        CLEAR(UEINTX, FIFOCON);
    }

    if (zero)
    {
        usb_wait_xmit();
        CLEAR(UEINTX, TXINI);
        CLEAR(UEINTX, FIFOCON);
    }
}

static void
usb_handle_get_status(struct usb_request *req)
{
    uint16_t status = 0;

    switch (req->type)
    {
    case (REQ_TYPE_STANDARD | REQ_TYPE_DEVICE_TO_HOST | REQ_TYPE_DEVICE):
        status = 0;
        break;
    case (REQ_TYPE_STANDARD | REQ_TYPE_DEVICE_TO_HOST | REQ_TYPE_ENDPOINT):
        if ((req->index & 0xF) == 0)
            status = 0;
        else
            status = 1;
    default:
        panic(req->type);
    }

    usb_write_control(&status, sizeof(status));
    usb_wait_out();
    usb_ack_out();
}

static void
usb_handle_set_address(struct usb_request *req)
{
    /* Configure address */
    WRITE(UDADDR, 0, 7, req->value & 0xFF);
    /* Acknowledge request */
    usb_wait_xmit();
    usb_xmit();
    /* We have to wait for controller to finish sending before enabling address */
    usb_wait_xmit();
    /* Enable address */
    SET(UDADDR, ADDEN);
}

static void
usb_handle_get_descriptor(struct usb_request *req)
{
    uint8_t type = req->value >> 8;
    uint8_t index = req->value & 0xF;
    uint16_t len = req->length;
    struct string_descriptor const* str;

    switch (type)
    {
    case DESC_TYPE_DEVICE:
        if (len > sizeof(dev_desc))
            len = sizeof(dev_desc);
        usb_write_control(&dev_desc, len);
        break;
    case DESC_TYPE_CONFIG:
        if (len > sizeof(conf_desc))
            len = sizeof(conf_desc);
        usb_write_control(&conf_desc, len);
        break;
    case DESC_TYPE_STRING:
        if (index >= sizeof(strings) / sizeof(strings[0]))
        {
            panic(index);
        }
        str = strings[index];
        if (len > str->length)
            len = str->length;
        usb_write_control(str, len);
        break;
    case DESC_TYPE_REPORT:
        if (len > sizeof(report_desc))
            len = sizeof(report_desc);
        usb_write_control(report_desc, len);
        break;
    default:
        panic(type);
    }
    usb_wait_out();
    usb_ack_out();
}

static void
usb_handle_set_configuration(struct usb_request* req)
{
    usb_wait_xmit();
    usb_xmit();
}

static void
usb_handle_set_idle(struct usb_request* req)
{
    usb_wait_xmit();
    usb_xmit();
}

static void
usb_handle_setup(void)
{
    static struct usb_request req;

    usb_read(&req, 8);
    usb_ack_setup();

    switch (req.request)
    {
    case REQ_GET_STATUS:
        usb_handle_get_status(&req);
        break;
    case REQ_SET_ADDRESS:
        usb_handle_set_address(&req);
        break;
    case REQ_GET_DESCRIPTOR:
        usb_handle_get_descriptor(&req);
        break;
    case REQ_SET_CONFIGURATION:
        usb_handle_set_configuration(&req);
        break;
    case REQ_SET_IDLE:
        usb_handle_set_idle(&req);
        break;
    default:
        panic(req.request);
    }
}

static void
usb_handle_interrupt(void)
{
    uint16_t state = sfc_read();
    uint16_t horiz = state & ((1 << SFC_LEFT) | (1 << SFC_RIGHT));
    uint16_t vert = state & ((1 << SFC_UP) | (1 << SFC_DOWN));
    uint16_t report = 0;

    switch (horiz)
    {
    case (1 << SFC_LEFT):
        report |= 0xF;
        break;
    case (1 << SFC_RIGHT):
        report |= 0x1;
        break;
    }

    switch (vert)
    {
    case (1 << SFC_UP):
        report |= 0xF << 4;
        break;
    case (1 << SFC_DOWN):
        report |= 0x1 << 4;
        break;
    }

    report |= (TEST(state, SFC_B) ? 1 : 0) << 8;
    report |= (TEST(state, SFC_Y) ? 1 : 0) << 9;
    report |= (TEST(state, SFC_SELECT) ? 1 : 0) << 10;
    report |= (TEST(state, SFC_START) ? 1 : 0) << 11;
    report |= (TEST(state, SFC_A) ? 1 : 0) << 12;
    report |= (TEST(state, SFC_X) ? 1 : 0) << 13;
    report |= (TEST(state, SFC_L) ? 1 : 0) << 14;
    report |= (TEST(state, SFC_R) ? 1 : 0) << 15;

    usb_write_in(&report, sizeof(report));
}

ISR(USB_GEN_vect)
{
    if (TEST(UDINT, EORSTI))
    {
        /* Reinitialize after reset */
        CLEAR(UDINT, EORSTI);
        ep0_init();
        ep1_init();
    }

    if (TEST(UDINT, WAKEUPI))
    {
        SET(PLLCSR, PLLE);
        while (!TEST(PLLCSR, PLOCK));
        CLEAR(USBCON, FRZCLK);

        CLEAR(UDINT, WAKEUPI);
        CLEAR(UDIEN, WAKEUPE);
        SET(UDIEN, SUSPE);
    }
    else if (TEST(UDINT, SUSPI))
    {
        CLEAR(UDINT, SUSPI);
        CLEAR(UDIEN, SUSPE);
        SET(UDIEN, WAKEUPE);
        SET(USBCON, FRZCLK);
        CLEAR(PLLCSR, PLLE);
    }
}

ISR(USB_COM_vect)
{
    UENUM = 0;
    /* Handle setup command */
    if (TEST(UEINTX, RXSTPI))
    {
        usb_handle_setup();
    }
    UENUM = 1;
    if (TEST(UEINTX, TXINI))
        usb_handle_interrupt();
}
