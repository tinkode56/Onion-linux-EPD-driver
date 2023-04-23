#include "epd_driver.h"
#include "image.h"
#include <sys/stat.h>

/**
 * @brief Sets DC pin level
 * @param level The level DC pin shall be set to
 * @return Returns the fd operation state
*/
int SetDC(level_type level)
{
    struct gpiohandle_data data;
    struct gpiohandle_request req;
    int fd, retval;

    fd = open(gpio_device, O_RDONLY);
    if (fd < 0)
        return -1;
    
    req.lineoffsets[0] = DC_OFFSET;
    req.flags = GPIOHANDLE_REQUEST_OUTPUT;
    req.lines = 1;
    retval = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
    close(fd);

    data.values[0] = level;
    retval = ioctl(req.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
    close(req.fd);

    return retval;
}

/**
 * @brief Sets RESET pin level
 * @param level The level RESET pin shall be set to
 * @return Returns the fd operation state
*/
int SetRESET(level_type level)
{
    struct gpiohandle_data data;
    struct gpiohandle_request req;
    int fd, retval;

    fd = open(gpio_device, O_RDONLY);
    if (fd < 0)
        return -1;
    
    req.lineoffsets[0] = RESET_OFFSET;
    req.flags = GPIOHANDLE_REQUEST_OUTPUT;
    req.lines = 1;
    retval = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
    close(fd);

    data.values[0] = level;
    retval = ioctl(req.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
    close(req.fd);

    return retval;
}

/**
 * @brief Waits for BUSY edge from LOW to HIGH
 * @return Returns EPD_BUSY_READY if success, -1 else
*/
int GetBUSY(void)
{
    int state;
    struct gpioevent_request req;
    struct pollfd pfd;
    int fd, ret;

    fd = open(gpio_device, O_RDONLY);
    if (fd < 0)
        return -1;
    
    req.lineoffset = BUSY_OFFSET;
    req.eventflags = GPIOEVENT_EVENT_RISING_EDGE;
    ret = ioctl(fd, GPIO_GET_LINEEVENT_IOCTL, &req);
    close(fd);

    pfd.fd = req.fd;
    pfd.events = POLLIN;
    ret = poll(&pfd, 1, -1);

    if(pfd.revents & POLLIN)
        state = EPD_BUSY_READY;
    else
        state = -1;
    close(req.fd);
    
    return state;
}

/**
 * @brief Sends data via SPI
 * @param tx Array containing data
 * @param len Length of data to be sent
*/
void SPITransfer(uint8_t const *tx, size_t len)
{
    int ret, fd;

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .len = len,
        .delay_usecs = spi_delay,
        .speed_hz = spi_speed,
        .bits_per_word = spi_bits,
    };

    fd = open(spi_device, O_RDWR);
    if (fd < 0)
        perror("Can`t open SPI device\n");

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        perror("Can`t send SPI message\n");
    
    close(fd);
}

/**
 * @brief Initialises the SPI module
*/
void SPIInit(void)
{
    int fd;
    int ret = 0;
    uint32_t spi_mode = SPI_MODE_0;

    fd = open(spi_device, O_RDWR);
    if (fd < 0)
        perror("Can`t open SPI device\n");

    /* SPI mode */
    ret = ioctl(fd, SPI_IOC_WR_MODE32, &spi_mode);
    if (ret == -1)
        perror("Can`t set SPI mode\n");
    
    /* SPI bits per word */
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bits);
    if (ret == -1)
        perror("Can`t set SPI bits per word\n");
    
    /* SPI max speed Hz */
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
    if (ret == -1)
        perror("Can`t set SPI max speed Hz\n");

    printf("SPI mode: 0x%x\n", spi_mode);
    printf("SPI bits per word: %d\n", spi_bits);
    printf("SPI max speed: %dHz (%dkHz)\n", spi_speed, spi_speed/1000);

    close(fd);
}

/**
 * @brief Sends CMD+DAT to EPD and toggles the pins accordingly
 * @param arr Array of data to be sent to EPD
*/
void EPDSendCmdData(uint8_t *arr)
{
    uint8_t array_length = arr[0];

    /* Send command */
    SetDC(LEVEL_LOW);
    SPITransfer(&arr[1], 1);

    /* Send data, only if available */
    if (array_length > 2)
    {
        SetDC(LEVEL_HIGH);
        SPITransfer(&arr[2], array_length-2);
    }
}

/**
 * @brief Initializes the EPD driver
*/
void EPDInit(void)
{
    /* Perform hard reset */
    SetRESET(LEVEL_HIGH);
    usleep(10000);
    SetRESET(LEVEL_LOW);
    usleep(1000);
    SetRESET(LEVEL_HIGH);
    /* Wait for BUSY signal to clear */
    GetBUSY();

    /* Booster soft start */
    EPDSendCmdData((uint8_t *)boost_setting_cmd);
    /* Power setting */
    EPDSendCmdData((uint8_t *)power_setting_cmd);
    /* Power on */
    EPDSendCmdData((uint8_t *)pwron_setting_cmd);
    /* Wait for BUSY signal to clear */
    GetBUSY();
    /* Panel setting */
    EPDSendCmdData((uint8_t *)panel_setting_cmd);
    /* PLL setting */
    EPDSendCmdData((uint8_t *)pll_setting_cmd);
    /* Resolution setting */
    EPDSendCmdData((uint8_t *)tres_setting_cmd);
    /* VCM_DC setting*/
    EPDSendCmdData((uint8_t *)vdcs_setting_cmd);
    /* Vcom and data interval setting */
    EPDSendCmdData((uint8_t *)vcom_dat_setting_cmd);
    /* Define flash */
    EPDSendCmdData((uint8_t *)fls_mode_setting_cmd);
}

/**
 * @brief Post data transmission commands and power off + deep sleep
*/
void EPDPostTx(void)
{
    /* Display refresh */
    EPDSendCmdData((uint8_t *)refresh_setting_cmd);
    /* Wait for BUSY signal to clear */
    GetBUSY();

    /* Power off */
    EPDSendCmdData((uint8_t *)pwroff_setting_cmd);
    /* Deep sleep */
    EPDSendCmdData((uint8_t *)deepsleep_setting_cmd);
}

/**
 * @brief Send data containing image to be displayed
*/
void EPDSendPictureContent(char filename[])
{
    /* Prepare bin file */
    struct stat info;
    if (stat(filename, &info) != 0)
    {
        perror("Error handling file --> STAT\n");
    }
    printf("FILE SIZE: %lu\n", (unsigned long)info.st_size);
    char *content = malloc(info.st_size);
    if (content == NULL) {
        perror("Error handling file --> CONTENT\n");
    }

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Error handling file --> OPEN\n");
    }

    /* Try to read a single block of info.st_size bytes */
    size_t blocks_read = fread(content, info.st_size, 1, fp);
    if (blocks_read != 1) {
        perror("Error handling file --> BLOCKS READ\n");
    }
    fclose(fp);

    /*
    * If nothing went wrong, content now contains the
    * data read from the file.
    */

    printf("DATA LENGTH: %lu\n", (unsigned long)info.st_size);

    /* Send data */
    SetDC(LEVEL_HIGH);
    // for (int j = 0; j < WIDTH; j++)
    // {
    //     for (int i = 0; i < (HEIGHT/2); i++)
    //     {
    //         SPITransfer(&arr[i + (j * (HEIGHT/2))], 1);
    //     }
    // }
    // fread(buffer,WIDTH*HEIGHT/2, WIDTH*HEIGHT/2, ptr);
    for (int j = 0; j < WIDTH; j++)
    {
        for (int i = 0; i < (HEIGHT/2); i++)
        {
            SPITransfer(&content[i + (j * (HEIGHT/2))], 1);
        }
    }
    
}