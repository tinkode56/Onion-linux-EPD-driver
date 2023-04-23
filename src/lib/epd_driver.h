#ifndef EPD_DRIVER_H
#define EPD_DRIVER_H

/**
 * General informations about hardware
 * 
 * DC           GPIO11      pin38   11
 * RESET        GPIO14      pin45   14
 * BUSY         GPIO15      pin46   15
 * SPI_CS       SPI_CS1     pin33   
 * SPI_MOSI     SPI_MOSI    pin36   
 * SPI_CLK      SPI_CLK     pin34   
 * 
 * NEOPIXEL     UTX2/PWM2   pin51  
 * 
 * /dev/gpiochip0 offset 0 means GPIO0
*/

/**
 * Includes
*/
#include <linux/gpio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdlib.h>
#include <poll.h>
#include <getopt.h>
#include <stdio.h>
#include <linux/spi/spidev.h>


/**
 * Defines
*/

#define DC_OFFSET 11
#define RESET_OFFSET 14
#define BUSY_OFFSET 15

#define EPD_BUSY_READY 1

#define WIDTH 384
#define HEIGHT 640


/**
 * Typedefs
*/

typedef enum
{
    LEVEL_HIGH = 1,
    LEVEL_LOW = 0,
} level_type;

/**
 * Variables
*/

static const char *spi_device = "/dev/spidev0.1";
static const char *gpio_device = "/dev/gpiochip0";
static uint32_t spi_mode;
static uint32_t spi_bits = 8;
static uint32_t spi_speed = 2000000;
static uint16_t spi_delay;

/**
 * EPD command table
 * format: { arr_size, cmd, [dat...] }
*/

static uint8_t power_setting_cmd[] = { 0x07, 0x01, 0x37, 0x00, 0x23, 0x23, 0x00 };
static uint8_t pwron_setting_cmd[] = { 0x02, 0x04 };
static uint8_t panel_setting_cmd[] = { 0x04, 0x00, 0x0f, 0x00 };
static uint8_t boost_setting_cmd[] = { 0x05, 0x06, 0xc7, 0xcc, 0x28 };
static uint8_t pll_setting_cmd[] = { 0x03, 0x30, 0x3c };
static uint8_t temp_setting_cmd[] = { 0x03, 0x41, 0x00 };
static uint8_t vcom_dat_setting_cmd[] = { 0x03, 0x50, 0x77 };
static uint8_t tcon_setting_cmd[] = { 0x03, 0x60, 0x22 };
static uint8_t tres_setting_cmd[] = { 0x06, 0x61, 0x02, 0x80, 0x01, 0x80 };
static uint8_t vdcs_setting_cmd[] = { 0x03, 0x82, 0x15 };
static uint8_t fls_mode_setting_cmd[] = { 0x03, 0xe5, 0x03 };
static uint8_t start_tx_setting_cmd[] = { 0x02, 0x10 };
static uint8_t stop_tx_setting_cmd[] = { 0x02, 0x11 };
static uint8_t refresh_setting_cmd[] = { 0x02, 0x12 };
static uint8_t pwroff_setting_cmd[] = { 0x02, 0x02 };
static uint8_t deepsleep_setting_cmd[] = { 0x03, 0x07, 0xa5 };
// static uint8_t _setting_cmd[] = { 0x00, };

/**
 * Functions
*/

/* GPIO functions */

int SetDC(level_type level);
int SetRESET(level_type level);
int GetBUSY(void);

/* SPI functions */

void SPIInit(void);
void SPITransfer(uint8_t const *tx, size_t len);

/* EPD functions */

void EPDSendCmdData(uint8_t *arr);
void EPDInit(void);
void EPDPostTx(void);
void EPDSendPictureContent(char filename[]);

#endif