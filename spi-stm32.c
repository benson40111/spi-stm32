/**
 * @file spi-stm32.c
 *
 * Simple spi testing program communicate with STM32 used in linux user space.
 * @author Benson Huang <Benson.Huang@senao.com>
 * 
 *
 * Copyright (C) 2021 Senao Design
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>
#include <stdarg.h>

#include "spi.h"
#include "version.h"

#define SPI_DEVICE_0 "/dev/spidev0.0"
#define SPI_DEVICE_1 "/dev/spidev1.0"

void help(void);
void free_ptr(size_t n, ...);

uint8_t device[ARRAY_SIZE(SPI_DEVICE_0)];
uint8_t tx_buf[SPI_MAX_TX_MAX_LEN];
uint8_t rx_buf[SPI_MAX_RX_MAX_LEN];

/**
 * @brief help message
 *
 */
void help(void)
{
    fprintf(stdout,
			"Usage: ./spi-stm32 -d [spi device node] -t [transmit data]\n"
            "  -d, --device: device node name\n"
            "  -t, --transmit: hex value to be transmited in one byte\n"
            "  -v, --version: show programe version\n"
            "  -h, --help: show help string\n"
            "  ex: ./spi_test_tool -d 1 -t 0x01\n");

	exit(0);
}

/**
 * @brief free pointer
 * 
 * @param n number of pointer
 * @param ... pointers need to be free
 * 
 * usage: free_ptr(2, tx_buf, rx_buf)
 * state: deprecate
 */
void free_ptr(size_t n, ...)
{
	va_list va_ptr;
	uint8_t *arg = NULL;

	va_start(va_ptr, n);
	for (size_t i = 0; i < n; i++) {
		arg = va_arg(va_ptr, uint8_t *);
		free(arg);
		arg = NULL;
	}
}

static void pabort(const char *s)
{
	if (errno != 0) {
		perror(s);
	} else {
		fprintf(stderr, "%s\n", s);
	}

	exit(0);
}

static void parse_opts(int argc, char *argv[])
{
	int get_c = 0;
	const char *opstring = "d:t:vh";

	static const struct option opts[] = {
		{"device", required_argument, NULL, 'd'},
		{"transmit", required_argument, NULL, 't'},
		{"version", no_argument, NULL, 'v'},
		{"help", no_argument, NULL, 'h'},
		{0, 0, 0, 0}
	};

	while ((get_c = getopt_long(argc, argv, opstring, opts, NULL)) != -1) {
		switch (get_c) {
		case 'd':
			switch (optarg[0]) {
				case '0':
					strcpy(device, SPI_DEVICE_0);
					break;

				case '1':
					strcpy(device, SPI_DEVICE_1);
					break;

				default:
					pabort("SPI bus input error!");
			}
			break;

		case 't':
			tx_buf[0] = strtol(optarg, NULL, 16);
			break;
		
		case 'v':
			fprintf(stdout, "version: %s\n", VERSION);
			help();

		case 'h':
			help();

		default:
			fprintf(stderr, "Invalid argument! %d\n", -EINVAL);
			help();
		}
	}
}

int main(int argc, char *argv[])
{
	spi_t *spi = NULL;
	int ret = 0;

	parse_opts(argc, argv);

	ret = spi_init(&spi, device);
	if (ret < 0) {
		spi_close(spi);
		pabort("spi_init() error!");
	}

	fprintf(stdout, "Transmit data: 0x%x\n", tx_buf[0]);

	ret = spi_write_read(spi, tx_buf, rx_buf, 1);
	if (ret < 0) {
		spi_close(spi);
		pabort("spi_write_read() error!");
	}

	spi_close(spi);

	return 0;
}
