/**
 * @file spi.c
 *
 * spi function
 * @author Benson Huang <Benson.Huang@senao.com>
 *
 * Copyright (C) 2021 Senao Design
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "spi.h"

/**
 * @brief init spi bus
 * 
 * @param self a pointer to spi struct
 * @param device bus number
 * 
 * @return init status
 */
int
spi_init(spi_t **self, const uint8_t *device)
{
    *self = malloc(sizeof(spi_t));
    if (self == NULL) {
        fprintf(stderr, "spi_init(): malloc: unable to allocate memory!"
            "%d\n", -ENOMEM);
        return -SPI_INIT_ERROR;
    }

    memset(&(*self)->xfer, 0, sizeof((*self)->xfer));

    (*self)->fd = open(device, O_RDWR);
    if ((*self)->fd < 0) {
        fprintf(stderr, "spi_init(): falied to open the bus\n");
        return -SPI_FILE_OPEN_ERROR;
    }

    if (ioctl((*self)->fd, SPI_IOC_RD_LSB_FIRST, &(*self)->lsb) < 0) {
        fprintf(stderr, "spi_init(): can't get 'LSB' first\n");
        spi_close(*self);
        return -SPI_LSB_FIRST_ERROR;
    }

    (*self)->mode = (uint8_t)0;
    (*self)->mode |= SPI_CPHA;
    if (ioctl((*self)->fd, SPI_IOC_WR_MODE, &(*self)->mode) < 0) {
        fprintf(stderr, "spi_init(): can't set mode first\n");
        spi_close(*self);
        return -SPI_SET_MODE_ERROR;
    }

    if (ioctl((*self)->fd, SPI_IOC_RD_MODE, &(*self)->mode) < 0) {
        fprintf(stderr, "spi_init(): can't get mode first\n");
        spi_close(*self);
        return -SPI_GET_MODE_ERROR;
    }

    (*self)->bits = (uint8_t)8;
    if (ioctl((*self)->fd, SPI_IOC_WR_BITS_PER_WORD, &(*self)->bits) < 0) {
        fprintf(stderr, "spi_init(): can't set bits per word\n");
        spi_close(*self);
        return -SPI_SET_BITS_PER_WORD_ERROR;
    }

    if (ioctl((*self)->fd, SPI_IOC_RD_BITS_PER_WORD, &(*self)->bits) < 0) {
        fprintf(stderr, "spi_init(): can't get bits per word\n");
        return -SPI_GET_BITS_PER_WORD_ERROR;
    }

    (*self)->speed = (uint32_t)10000000;
    if (ioctl((*self)->fd, SPI_IOC_WR_MAX_SPEED_HZ, &(*self)->speed) < 0) {
        fprintf(stderr, "spi_init(): can't set max speed\n");
        spi_close(*self);
        return -SPI_SET_SPEED_ERROR;
    }

    if (ioctl((*self)->fd, SPI_IOC_RD_MAX_SPEED_HZ, &(*self)->speed) < 0) {
        fprintf(stderr, "spi_init(): can't get max speed\n");
        spi_close(*self);
        return -SPI_GET_SPEED_ERROR;
    }

    return SPI_OK;
}

/**
 * @brief close spi bus
 * 
 * @param self a pointer to spi struct
 */
void
spi_close(spi_t *self)
{
    free(self);
    close(self->fd);
}

/**
 * @brief write and read byte(s) to device
 * 
 * @param self a pointer to spi struct
 * @param tx_buf transmit buf
 * @param rx_buf receive buf
 * @param len size of byte
 * 
 * @return write/read status
 */
int
spi_write_read(spi_t *self, const uint8_t *tx_buf, uint8_t *rx_buf,
    const size_t len)
{
    int ret = 0;
    
    self->xfer.tx_buf = (uint64_t)tx_buf;
    self->xfer.len    = (uint32_t)len;
    ret = ioctl(self->fd, SPI_IOC_MESSAGE(1), &self->xfer);
    if (ret < 0) {
        fprintf(stderr, "spi_write_read(): ioctl(SPI_IOC_MESSAGE(1)) return %d\n",
                ret);
        return -SPI_WRITE_ERROR;
    }

    self->xfer.rx_buf = (uint64_t)rx_buf;
    ret = ioctl(self->fd, SPI_IOC_MESSAGE(1), &self->xfer);
    if (ret < 0) {
        fprintf(stderr, "spi_write_read(): ioctl(SPI_IOC_MESSAGE(1)) return %d\n",
                ret);
        return -SPI_READ_ERROR;
    }

    return SPI_OK;
}

