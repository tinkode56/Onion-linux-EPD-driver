#include <epd_driver.h>
#include <bmp_util.h>
#include <main.h>
#include "image.h"


int main(int argc, char* argv[])
{
	/* Colors: 
		0x00 - black (2 pixels)
		0x33 - white (2 pixels)
		0x44 - red (2 pixels)
	 */

	// unsigned char buffer[EPD_HEIGHT/2];
	// FILE *ptr;

	// uint8_t *tx;
	// int size = EPD_WIDTH*(EPD_HEIGHT/2);

	// tx = malloc(size);

	// for (int i = 0; i < size; i++)
	// 	tx[i] = 0x44;

	
	// SetDC(LEVEL_LOW);
	// SetRESET(LEVEL_HIGH);
	// SetDC(LEVEL_HIGH);
	// SetRESET(LEVEL_LOW);
	// SetDC(LEVEL_LOW);
	// SetRESET(LEVEL_HIGH);
	// SetDC(LEVEL_HIGH);
	// SetDC(LEVEL_LOW);
	// SetRESET(LEVEL_LOW);
	// SetDC(LEVEL_HIGH);

	SPIInit();
	
	// printf("%d\n", GetBUSY());

	// EPDSendCmdData((uint8_t *)power_setting_cmd);
	EPDInit();
	EPDSendCmdData((uint8_t *)start_tx_setting_cmd);

	// ptr = fopen("image.bin", "rb");
	// fread(buffer,EPD_WIDTH*EPD_HEIGHT/2, 1, ptr);
	// EPDSendPictureContent("image.bin");
	// EPDSendBMPData("miaumiau2.bmp");
	EPDSendBMPData(argv[1]);

	EPDPostTx();
	// free(tx);
    return 0;
}