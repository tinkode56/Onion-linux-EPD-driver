#include <lib/epd_driver.h>
#include <main.h>
#include "image.h"


int main(void)
{
	/* Colors: 
		0x00 - black (2 pixels)
		0x11 - white (2 pixels)
		0x44 - red (2 pixels)
	 */
	
	// uint8_t *tx;
	// int size = WIDTH*(HEIGHT/2);

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
	EPDSendPictureContent((uint8_t *)epd_75_image);
	EPDPostTx();
	// free(tx);
    return 0;
}