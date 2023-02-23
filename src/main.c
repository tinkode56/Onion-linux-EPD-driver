/*
    spidevlib.c - A user-space program to comunicate using spidev.
				Gustavo Zamboni
*/

/*
    mipsel-openwrt-linux-gcc -o helloworld helloworld.c
*/
#include <lib/epd_driver.h>
#include <main.h>


int main(void)
{
	uint8_t *tx;
	int size = 320;

	tx = malloc(size);

	for (int i = 0; i < size; i++)
		tx[i] = i;

	
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
	
	printf("%d\n", GetBUSY());

	EPDSendCmdData((uint8_t *)power_setting_cmd);
	free(tx);
    return 0;
}