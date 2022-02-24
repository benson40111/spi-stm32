/**
 * @file spi.h
 *
 * spi function define
 * @author Benson Huang <Benson.Huang@senao.com>
 *
 * Copyright (C) 2021 Senao Design
 *
 */

#include <errno.h>
#include <linux/spi/spidev.h>

struct _spi;

/**
 * @brief SPI object
 * 
 */
typedef struct _spi {
  int   fd; ///< file descripter
  uint8_t  lsb; ///< LSB first
  uint8_t mode; ///< SPI mode
  uint8_t bits; ///< bits per mode
  uint32_t speed; ///< SPI speed
  struct spi_ioc_transfer xfer;
} spi_t;

/* Macro */
#define SPI_MAX_TX_MAX_LEN  1
#define SPI_MAX_RX_MAX_LEN  1
/* TODO: Check is an array not a pointer */
#define ARRAY_SIZE(arr)  (sizeof(arr) / sizeof((arr)[0]))

#define autofree \
    __attribute__((cleanup(free_stack)))

__attribute__ ((always_inline))
inline void free_stack(void *ptr) { free(*(void **) ptr); }

/* error code */
#define SPI_OK                      0
#define SPI_INIT_ERROR              1
#define SPI_WRITE_ERROR             2
#define SPI_READ_ERROR              3
#define SPI_FILE_OPEN_ERROR         4
#define SPI_LSB_FIRST_ERROR         5
#define SPI_SET_MODE_ERROR          6
#define SPI_GET_MODE_ERROR          7
#define SPI_SET_BITS_PER_WORD_ERROR 8
#define SPI_GET_BITS_PER_WORD_ERROR 9
#define SPI_SET_SPEED_ERROR         10
#define SPI_GET_SPEED_ERROR         11

int spi_init(spi_t **self, const uint8_t *device);
void spi_close(spi_t *self);
int spi_write_read(spi_t *self, const uint8_t *tx_buf, uint8_t *rx_buf,
  const size_t len);