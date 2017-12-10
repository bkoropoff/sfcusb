#define CTASSERT(N, P) typedef struct { int _x[(P) ? 1 : -1]; } N ## _assert_

#define SET(R, P) ((R) |= _BV(P))
#define CLEAR(R, P) ((R) &= ~_BV(P))
#define TEST(R, P) ((R) & _BV(P))
#define MASK(F, W) ((_BV((F) + (W)) - 1) & ~(_BV((F)) - 1))
#define WRITE(R, F, W, V) ((R) = ((R) & ~MASK((F), (W))) | (((V) << (F)) & MASK((F), (W))))
#define READ(R, F, W) (((R) & MASK(F, W)) >> (F))

#define SLEEP() __asm__ __volatile__ ("sleep" ::: "memory")

#define LED1_CONF() SET(DDRB, DDB0)
/* LEDs on this board are connected to VCC rather than GND, so
   we have to pull the pins low to turn them on */
#define LED1_ON() CLEAR(PORTB, PORTB0)
#define LED1_OFF() SET(PORTB, PORTB0)
#define LED2_CONF() SET(DDRD, DDD5)
#define LED2_ON() CLEAR(PORTD, PORTD5)
#define LED2_OFF() SET(PORTD, PORTD5)

#define PANIC_HALT 0x0
#define PANIC_USB_INIT 0x1
#define PANIC_USB_SETUP 0x2

#define PACKED __attribute__((packed))

void panic(uint8_t code);
