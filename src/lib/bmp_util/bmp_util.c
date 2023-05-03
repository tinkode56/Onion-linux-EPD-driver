#include "bmp_util.h"

/**
 * @brief Parse BMP file content
*/
void BMPParse(char* filename, uint8_t* data_buffer)
{
    tBMP_header image_header = { 0 };
    uint8_t header[0x36];

    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror(strerror(errno));
    }

    /* Read first 0x36 bytes data, last member is bitmap_data_size */
    fread(header, sizeof(uint8_t), 0x36, f);

    /* Populate header struct */
    image_header.file_size = *(uint32_t*)&header[0x02];
    image_header.bmp_data_offset = *(uint32_t*)&header[0x0a];
    image_header.header_size = *(uint32_t*)&header[0x0e];
    image_header.width = *(uint32_t*)&header[0x12];
    image_header.height = *(uint32_t*)&header[0x16];
    image_header.planes = *(uint16_t*)&header[0x1a];
    image_header.bits = *(uint16_t*)&header[0x1c];
    image_header.compression = *(uint32_t*)&header[0x1e];
    image_header.bitmap_data_size = *(uint32_t*)&header[0x22];
    image_header.hres = *(uint32_t*)&header[0x26];
    image_header.vres = *(uint32_t*)&header[0x2a];
    image_header.colors = *(uint32_t*)&header[0x2e];
    image_header.important_colors = *(uint32_t*)&header[0x32];

    /* Extract the color palette */
    uint8_t color_palette[4 * image_header.colors];
    struct pixel colors[image_header.colors];

    fread(color_palette, sizeof(uint8_t), 4 * image_header.colors, f);
    for (int i = 0, j = 0; i < sizeof(color_palette)/sizeof(uint8_t); i+=4, j++) {
        colors[j].blue = *(uint8_t*)&color_palette[i];
        colors[j].green = *(uint8_t*)&color_palette[i + 1];
        colors[j].red = *(uint8_t*)&color_palette[i + 2];
    }

    /* Check if image is compatible with EPD */
    if ((image_header.colors != 3) && (image_header.important_colors != 3) && (image_header.bits != 4)) {
        perror("Number of colors different of what EPD can handle");
        return;
    }

    /* Build LUT to replace colors with values used by EPD:
     * White = 0x3
     * Black = 0x0
     * Red = 0x4 */
    uint8_t* color_lut = (uint8_t*)malloc(image_header.colors * sizeof(uint8_t));
    for (int i = 0; i < image_header.colors; i++) {
        if (colors[i].red == 255)
            if (colors[i].green == 255)
                color_lut[i] = 0x3; /* White */
            else
                color_lut[i] = 0x4; /* Red */
        else
            color_lut[i] = 0x0; /* Black */
    }

    /* Read pixel data */
    // uint8_t* data_buffer = (uint8_t*)malloc(image_header.bitmap_data_size * sizeof(uint8_t));
    fread(data_buffer, sizeof(uint8_t), image_header.bitmap_data_size, f);

    /* Apply LUT on original pixel map */
    for (int i = 0; i < image_header.bitmap_data_size; i++) {
        uint8_t new_byte = 0x00;
        uint8_t old_byte = *(uint8_t*)&data_buffer[i];

        new_byte |= (color_lut[(old_byte & 0xF0) >> 4]) << 4;
        new_byte |= color_lut[old_byte & 0x0F];

        data_buffer[i] = new_byte;
    }

    printf("File size: %d\n", image_header.file_size);

    free(color_lut);
    fclose(f);
}
