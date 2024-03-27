/*
 * MIT License
 *
 * Copyright (c) 2024 Pontus Fuchs <pontus.fuchs@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#include <wchar.h>
#include <hidapi.h>

#define MAX_STR 255
#define HIDMSG_SIZE 64

/* Location of unit in messages*/
#define UNIT_OFFSET 4
/* Location of data in messages*/
#define DATA_OFFSET 5
/* Length of data */
#define DATA_LEN 8

#define UNITS_MAX 21
char *units[] = {
	"V AC",	// 0
	"mV AC",
	"V DC",
	"mV DC",
	"Hz",
	"%",
	"Ohm",
	"Ohm",
	"V",
	"u/nF",
	"C",	//10
	"F",
	"uA DC",
	"uA AC",
	"mA DC",
	"mA AC", //15
	"A DC",
	"A AC",
	"?18",
	"?19",
	"NZV",
	"lozV",
};

static void print_unit(char unit)
{
	if (unit > UNITS_MAX) {
		printf("??? (%d)\n", unit);
		return;
	}
	puts(units[unit]);
}

static void mm_main(hid_device *handle)
{
	int res;
	unsigned char buf[HIDMSG_SIZE];
	int i;

	while(1) {
		/* Magic values. Captured from UNI-T windows SW */
		buf[0] = 0x6;
		buf[1] = 0xab;
		buf[2] = 0xcd;
		buf[3] = 0x03;
		buf[4] = 0x5e;
		buf[5] = 0x01;
		buf[6] = 0xd9;

		res = hid_write(handle, buf, sizeof(buf));
		if (res != sizeof(buf)) {
				fprintf(stderr, "hid_write failed");
				exit(1);
		}

		res = hid_read(handle, buf, sizeof(buf));
		if (res != sizeof(buf)) {
				fprintf(stderr, "hid_write failed");
				exit(1);
		}

/*
		// Dump raw hid msg
		for (i = 0; i <31; i++)
			printf("%02x ", buf[i]);
*/

		/* Data comes as ascii*/
		for (i = 0; i < DATA_LEN;i++) {
			if (!isprint(buf[i+DATA_OFFSET])) {
				fprintf(stderr, "Bad data");
				exit(1);
			}
			putchar(buf[i+DATA_OFFSET]);
		}
		print_unit(buf[UNIT_OFFSET]);
		usleep(1000000);
	}
}

int main(int argc, char* argv[])
{
	int res;
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i;
	unsigned char unit;

	res = hid_init();

	handle = hid_open(0x1a86, 0xe429, NULL);
	if (!handle) {
		printf("Unable to open device\n");
		hid_exit();
		exit(1);
	}
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	printf("Manufacturer String: %ls\n", wstr);
	res = hid_get_product_string(handle, wstr, MAX_STR);
	printf("Product String: %ls\n", wstr);
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	printf("Serial Number String: (%d) %ls\n", wstr[0], wstr);
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	printf("Indexed String 1: %ls\n", wstr);

	mm_main(handle);

	hid_close(handle);
	res = hid_exit();
	return 0;
}
