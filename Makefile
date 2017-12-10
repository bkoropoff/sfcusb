CPU_FLAGS := -DF_CPU=16000000UL -mmcu=atmega32u4
CFLAGS := -Wall -Werror $(CPU_FLAGS) -Os

OBJECTS = main.o usb.o util.o sfc.o
HEADERS = includes.h sfc.h usb.h util.h

all: sfcusb.hex

upload: sfcusb.hex
	avrdude -v -patmega32u4 -cavr109 -P/dev/ttyACM0 -b57600 -D -Uflash:w:$<:i

clean:
	rm -f $(OBJECTS) sfcusb.hex sfcusb

sfcusb.hex: sfcusb
	avr-objcopy -O ihex -R .eeprom $< $@

sfcusb: $(OBJECTS)
	avr-gcc $(CFLAGS) -o $@ $(OBJECTS)

.c.o: $(HEADERS)
	avr-gcc $(CFLAGS) -c -o $@ $<

.PHONY: upload clean
