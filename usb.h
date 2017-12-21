#ifndef USB_H_
#define USB_H_

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

#define HRD_SIZE_0 0
#define HRD_SIZE_1 1
#define HRD_SIZE_2 2
#define HRD_SIZE_4 3

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
#define HRD_USAGE_BUTTON_2 0x02
#define HRD_USAGE_BUTTON_3 0x03
#define HRD_USAGE_BUTTON_4 0x04
#define HRD_USAGE_BUTTON_5 0x05
#define HRD_USAGE_BUTTON_6 0x06
#define HRD_USAGE_BUTTON_7 0x07
#define HRD_USAGE_BUTTON_8 0x08
#define HRD_USAGE_BUTTON_9 0x09

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
#define REQ_GET_REPORT 0x01

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

void usb_init(void);

#endif
