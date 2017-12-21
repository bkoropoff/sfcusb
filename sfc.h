/* We shift left into state, so positions are mirrored */
#define CTLR_B (15 - 0)
#define CTLR_Y (15 - 1)
#define CTLR_SELECT (15 - 2)
#define CTLR_START (15 - 3)
#define CTLR_UP (15 - 4)
#define CTLR_DOWN (15 - 5)
#define CTLR_LEFT (15 - 6)
#define CTLR_RIGHT (15 - 7)
#define CTLR_A (15 - 8)
#define CTLR_X (15 - 9)
#define CTLR_L (15 - 10)
#define CTLR_R (15 - 11)

/* Combination to jump back to bootloader at startup */
#define CTLR_BOOTLOADER ((1 << CTLR_L) | (1 << CTLR_R))

void controller_init(void);
uint16_t controller_read(void);
