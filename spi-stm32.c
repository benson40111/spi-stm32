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

/**
 * @brief help message
 *
 */
void help(void)
{
    fprintf(stdout,
			"Usage: ./spi-stm32 -d [spi device node] -t [transmit data]\n"
            "-d, --device: device node name\n"
            "-t, --transmit: hex value to be transmited in one byte\n"
            "-v, --version: show programe version\n"
            "-h, --help: show help string\n"
            "ex: ./spi_test_tool -d 1 -t 0x01\n");
}

/**
 * @brief free pointer
 * 
 * @param n number of pointer
 * @param ... pointers need to be free
 * 
 * usage: free_ptr(2, tx_buf, rx_buf)
 * state: throw-out
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

int main(int argc, char *argv[])
{
	spi_t *spi = NULL;
	int ret = 0;
	int get_c = 0;
	const char *opstring = "d:t:vh";

	autofree uint8_t *tx_buf = malloc(sizeof(uint8_t) * SPI_MAX_TX_MAX_LEN);
	if (tx_buf == NULL) {
		fprintf(stderr, "tx_buf memory allocate error: %s\n", strerror(errno));
	}

	autofree uint8_t *rx_buf = malloc(sizeof(uint8_t) * SPI_MAX_RX_MAX_LEN);
	if (rx_buf == NULL) {
		fprintf(stderr, "rx_buf memory allocate error: %s\n", strerror(errno));
	}
	
	autofree uint8_t *spi_bus = malloc(sizeof(uint8_t) * ARRAY_SIZE(SPI_DEVICE_0));
	if (spi_bus == NULL) {
		fprintf(stderr, "rx_buf memory allocate error: %s\n", strerror(errno));
	}

	static struct option opts[] = {
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
					strcpy(spi_bus, SPI_DEVICE_0);
					break;

				case '1':
					strcpy(spi_bus, SPI_DEVICE_1);
					break;

				default:
					fprintf(stdout, "SPI bus input error! %d\n", -EINVAL);
					goto exit;
			}
			break;

		case 't':
			tx_buf[0] = strtol(optarg, NULL, 16);
			break;
		
		case 'v':
			fprintf(stdout, "version: %s\n", VERSION);
			goto exit;

		case 'h':
			help();
			goto exit;

		case '?':
			fprintf(stderr, "Invalid argument! %d\n", -EINVAL);
			help();
			goto exit;

		default:
			fprintf(stderr, "Invalid argument! %d\n", -EINVAL);
			help();
			goto exit;
		}
	}

	ret = spi_init(&spi, spi_bus);
	if (ret < 0) {
		fprintf(stderr, "spi_init() error! %d\n", ret);
		spi_close(spi);
		goto exit;
	}

	fprintf(stdout, "Transmit data: 0x%x\n", tx_buf[0]);

	ret = spi_write_read(spi, tx_buf, rx_buf, 1);
	if (ret < 0) {
		fprintf(stderr, "spi_write_read() error! %d\n", ret);
		spi_close(spi);
		goto exit;
	}

	fprintf(stdout, "Receive data: 0x%x\n", rx_buf[0]);
	spi_close(spi);

	/* TODO: Need a good solution to end programe and free pointer */
	exit:
		return 0;

	return 0;
}
