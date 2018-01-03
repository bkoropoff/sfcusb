#define CTLR_UP 0
#define CTLR_DOWN 1
#define CTLR_LEFT 2
#define CTLR_RIGHT 3
#define CTLR_COIN 4
#define CTLR_START 5
#define CTLR_A 6
#define CTLR_B 7
#define CTLR_C 8
#define CTLR_D 9
#define CTLR_E 10
#define CTLR_F 11

/* Combination to jump back to bootloader at startup */
#define CTLR_BOOTLOADER ((1 << CTLR_COIN) | (1 << CTLR_START))

void controller_init(void);
uint16_t controller_read(void);
uint16_t controller_read_debounced(void);
void controller_poll(void);
