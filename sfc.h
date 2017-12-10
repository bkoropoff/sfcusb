/* We shift left into state, so positions are mirrored */
#define SFC_B (15 - 0)
#define SFC_Y (15 - 1)
#define SFC_SELECT (15 - 2)
#define SFC_START (15 - 3)
#define SFC_UP (15 - 4)
#define SFC_DOWN (15 - 5)
#define SFC_LEFT (15 - 6)
#define SFC_RIGHT (15 - 7)
#define SFC_A (15 - 8)
#define SFC_X (15 - 9)
#define SFC_L (15 - 10)
#define SFC_R (15 - 11)

/* Combination to jump back to bootloader at startup */
#define SFC_BOOTLOADER ((1 << SFC_L) | (1 << SFC_R))

void sfc_init(void);
uint16_t sfc_read(void);
