#include "includes.h"
#include "util.h"
#include "usb.h"

#include "usbconf.h"

static const struct device_descriptor dev_desc =
{
    .length = sizeof(dev_desc),
    .type = DESC_TYPE_DEVICE,
    .usb = 0x0110,
    .device_class = DEV_CLASS_NONE,
    .device_sub_class = 0,
    .device_protocol = 0,
    .max_packet_size = MAX_CONTROL_SIZE,
    .vendor_id = USB_VENDOR_ID,
    .product_id = USB_PRODUCT_ID,
    .device_version = 0x0100,
    .manufacturer_index = USB_MANU_INDEX,
    .product_index = USB_PROD_INDEX,
    .serial_number_index = 0,
    .num_configurations = 1
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
        .max_power = USB_MAX_POWER
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
        .length = sizeof(usb_report_desc)
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

    /* Set PLL pre-scaler to 1/2 to get 8 MHz from 16 MHz clock */
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
        if (index >= sizeof(usb_strings) / sizeof(usb_strings[0]))
        {
            panic(index);
        }
        str = usb_strings[index];
        if (len > str->length)
            len = str->length;
        usb_write_control(str, len);
        break;
    case DESC_TYPE_REPORT:
        if (len > sizeof(usb_report_desc))
            len = sizeof(usb_report_desc);
        usb_write_control(usb_report_desc, len);
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
usb_handle_get_report(struct usb_request *req)
{
    uint8_t report[MAX_INTERRUPT_SIZE];
    uint8_t len;

    len = usb_report(report);

    usb_write_control(report, len);
    usb_wait_out();
    usb_ack_out();
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
    case REQ_GET_REPORT:
        usb_handle_get_report(&req);
        break;
    default:
        panic(req.request);
    }
}

static void
usb_handle_interrupt(void)
{
    uint8_t report[MAX_INTERRUPT_SIZE];
    uint8_t len;

    len = usb_report(report);
    usb_write_in(report, len);
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
