#include "epd_driver.h"

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
 * @param arr Array with info to be sent to EPD
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
