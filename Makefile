CFLAGS := -std=c99 -Wall $(pkg-config --cflags hidapi-libusb) $(pkg-config --libs hidapi-libusb)

ut161d: ut161d.c
	gcc -o ut161d ut161d.c -I/usr/include/hidapi `pkg-config --libs hidapi-libusb`

clean:
	rm ut161d ut161d.o