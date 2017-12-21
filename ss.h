/* We shift left into state, so positions are mirrored */
#define CTLR_Z 0
#define CTLR_Y 1
#define CTLR_X 2
#define CTLR_R 3
#define CTLR_B 4
#define CTLR_C 5
#define CTLR_A 6
#define CTLR_START 7
#define CTLR_UP 8
#define CTLR_DOWN 9
#define CTLR_LEFT 10
#define CTLR_RIGHT 11
#define CTLR_L 15

/* Combination to jump back to bootloader at startup */
#define CTLR_BOOTLOADER ((1 << CTLR_L) | (1 << CTLR_R))

void controller_init(void);
uint16_t controller_read(void);
