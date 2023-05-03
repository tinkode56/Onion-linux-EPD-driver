#ifndef BMP_UTIL_H
#define BMP_UTIL_H

/**
 * Includes
*/
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

/**
 * Type definition
*/

typedef struct {
    uint32_t file_size;
    uint32_t bmp_data_offset;
    uint32_t header_size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bits;
    uint32_t compression;
    uint32_t bitmap_data_size;
    uint32_t hres;
    uint32_t vres;
    uint32_t colors;
    uint32_t important_colors;
} tBMP_header;

struct pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

/**
 * Functions
*/

void BMPParse(char* filename, uint8_t* data_buffer);

#endif