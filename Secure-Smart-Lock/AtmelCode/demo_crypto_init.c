/**
 *
 * \file
 *
 * \brief ECC personalization implementation.
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include <string.h>
#include <stdio.h>
#include <atca_cmds.h>
#include <atca_comm.h>
#include <atca_lib_return_codes.h>
#include "demo_global_defines.h"
#include "demo_crypto_init.h"
#include "atca_config.h"

//#define	CHECK_RET_CODE

// Write Word Address 0x10 - 0x30
uint8_t ecc108_write_slot_config(void)
{
	uint8_t ret_code = 0x00;
	uint8_t command[WRITE_COUNT_SHORT];
	uint8_t response[WRITE_RSP_SIZE];

	// Configuration Zone Word Address
	uint8_t config_address[9] =		{0x10,0x14,0x18,0x1C,0x20,0x24,0x28,0x2C,0x30};

	// Data to be written to each Address
	uint8_t config_data[9][4] =		{{ATCA_I2C_CONFIGURED_ADDRESS,0xFF,0xAA,0x60},
									{0x83,0x20,0x8F,0x8F},
									{0x8F,0x8F,0x8F,0x8F},
									{0x9F,0x8F,0x9F,0x8F},
									{0x8F,0x8F,0xAF,0x3F},
									{0x00,0x00,0x00,0x00},
									{0x00,0x00,0x00,0x00},
									{0x00,0x00,0x00,0x00},
									{0x00,0x00,0xAF,0x8F}};

	// Address Encoding of Configuration Zone Word Address
	uint16_t address_encoding[9] =	{0x0004,0x0005,0x0006,0x0007,0x0008,0x0009,0x000A,0x000B,0x000C};

	// Loop Write Configuration Zone
	for (uint8_t i=0;i<9;i++)
	{
		// Write 4-Bytes to Config Zone.
		ret_code = atca_execute(ATCA_WRITE, ATCA_ZONE_CONFIG,
		address_encoding[i], ATCA_ZONE_ACCESS_4, &config_data[i][0], 0, NULL, 0, NULL, sizeof(command),
		command, WRITE_RSP_SIZE, response);
		if (ret_code == ATCA_SUCCESS)
		{
			printf("Write Config Zone Address 0x%.2X, Success!",config_address[i]);
		}
		else
		{
			printf("Write Config Zone Address 0x%.2X, Fail!\r\n",config_address[i]);
			printf("config zone ret_code = 0x%02X", ret_code);
			break;
		}
		printf("\r\n");
	}
	printf("\n");

	return ret_code;
}

// Write Word Address 0x60 - 0x7C
uint8_t ecc108_write_key_config(void)
{
	uint8_t ret_code = 0x00;
	uint8_t command[WRITE_COUNT_SHORT];
	uint8_t response[WRITE_RSP_SIZE];

	// Configuration Zone Word Address
	uint8_t config_address[8] =		{0x60,0x64,0x68,0x6C,0x70,0x74,0x78,0x7C};

	// Data to be written to each Address
	uint8_t config_data[8][4] =		{{0x33,0x00,0x1C,0x00},
									{0x1C,0x00,0x1C,0x00},
									{0x1C,0x00,0x1C,0x00},
									{0x1C,0x00,0x1C,0x00},
									{0x1C,0x00,0x1C,0x00},
									{0x1C,0x00,0x1C,0x00},
									{0x1C,0x00,0x1C,0x00},
									{0x1C,0x00,0x1C,0x00}};

	// Address Encoding of Configuration Zone Word Address
	uint16_t address_encoding[8] =	{0x0018,0x0019,0x001A,0x001B,0x001C,0x001D,0x001E,0x001F};

	// Loop Write Configuration Zone
	for (uint8_t i=0;i<8;i++)
	{
		// Write 4-Bytes to Config Zone.
		ret_code = atca_execute(ATCA_WRITE, ATCA_ZONE_CONFIG,
		address_encoding[i], ATCA_ZONE_ACCESS_4, &config_data[i][0], 0, NULL, 0, NULL, sizeof(command),
		command, WRITE_RSP_SIZE, response);
		if (ret_code == ATCA_SUCCESS)
		{
			printf("Write Config Zone Address 0x%.2X, Success!",config_address[i]);
		}
		else
		{
			printf("Write Config Zone Address 0x%.2X, Fail!\r\n",config_address[i]);
			printf("config zone ret_code = 0x%02X", ret_code);
			break;
		}
		printf("\r\n");
	}
	printf("\n");

	return ret_code;
}

// Write Data Zone 00 - 0F
uint8_t ecc108_write_data_zone(void)
{
	uint8_t ret_code = 0x00;
	uint8_t command[WRITE_COUNT_LONG];
	uint8_t response[WRITE_RSP_SIZE];
	uint8_t zone = 0;
	uint8_t j = 0;

	// Data of Zone 00-07: zone_data[0][] - zone_data[7][]
	// Data of Zone 08-0F: zone_data[8][] -> since the contents of each zone are the same
	uint8_t zone_data[9][32] =	{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
								0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
								0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
								0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
								
								{0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
								0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
								0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
								0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11},
								
								{0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,
								0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,
								0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,
								0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22},
								
								{0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,
								0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,
								0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,
								0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33},
								
								{0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
								0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
								0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
								0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44},
								
								{0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
								0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
								0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
								0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55},
								
								{0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,
								0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,
								0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,
								0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66},
								
								{0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,
								0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,
								0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,
								0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77},
								
								{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
								0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
								0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
								0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};

	// Address Encoding of Zone 00-0F
	// Each Zone consist of varying number of Blocks
	// Each Block needs to be written separately using 32-Bytes Write
	uint16_t address_encoding[44+1] =	{0x0000,0x0100,0x0008,0x0108,0x0010,0x0110,0x0018,0x0118,
										0x0020,0x0120,0x0028,0x0128,0x0030,0x0130,0x0038,0x0138,
										0x0040,0x0140,0x0240,0x0340,0x0440,0x0540,0x0640,0x0048,
										0x0148,0x0248,0x0050,0x0150,0x0250,0x0058,0x0158,0x0258,
										0x0060,0x0160,0x0260,0x0068,0x0168,0x0268,0x0070,0x0170,
										0x0270,0x0078,0x0178,0x0278,0xFFFF};

	// Loop Write Data Zone 01-0E
	// Note: this function needs to be adjusted according to the Zone Configuration
	for (uint8_t i=2;i<41;i++)
	{
		zone = (address_encoding[i] & 0x00FF) / 8;
		
		// Since Data of Zone 08-0F points to the same Array
		if(zone < 8)
		{
			j = zone;
		}
		else
		{
			// Only write the first seven zones
			break;
			j = 8;
		}

		// Write 32-Bytes
		ret_code = atca_execute(ATCA_WRITE, ATCA_ZONE_DATA | ATCA_ZONE_COUNT_FLAG,
		address_encoding[i], ATCA_KEY_SIZE, &zone_data[j][0], 0, NULL, 0, NULL, sizeof(command),
		command, WRITE_RSP_SIZE, response);
		if (ret_code != ATCA_SUCCESS)
		{
			printf("Write Data Zone %.2X, Fail!\r\n",zone);
			break;
		}

		// If Zone will change in the next loop, print current Zone status
		if ((address_encoding[i] & 0x00FF) != (address_encoding[i+1] & 0x00FF))
		{
			printf("Write Data Zone %.2X, Success!\r\n",zone);
		}
	}	// end for (uint8_t i=2;i<41;i++)
	printf("\n");

	return ret_code;
}

// Lock Config Zone
uint8_t ecc108_lock_config_zone(void)
{
	uint8_t ret_code = 0xFF;
	uint8_t command[WRITE_COUNT_SHORT];
	uint8_t response[WRITE_RSP_SIZE];
	
	ret_code = atca_execute(ATCA_LOCK, LOCK_ZONE_NO_CRC, 0x0000,
	0, NULL, 0, NULL, 0, NULL, sizeof(command),
	command, WRITE_RSP_SIZE, response);
	if (response[1] == 0x11)
	{
		ret_code = 0x0F; /* It is not Lock Command response but WakeUp response */
	}
	//puthex_array(response, WRITE_RSP_SIZE);
	return ret_code;
}

// Lock Data Zone
uint8_t ecc108_lock_data_zone(void)
{
	uint8_t ret_code = 0xFF;
	uint8_t command[WRITE_COUNT_SHORT];
	uint8_t response[WRITE_RSP_SIZE];
	
	ret_code = atca_execute(ATCA_LOCK, LOCK_ZONE_NO_CRC | LOCK_ZONE_DATA, 0x0000,
	0, NULL, 0, NULL, 0, NULL, sizeof(command),
	command, WRITE_RSP_SIZE, response);
	//puthex_array(command, sizeof(command));
	if (response[1] == 0x11)
	{
		ret_code = 0x0F; /* It is not Lock Command response but WakeUp response */
	}
	return ret_code;
}

uint8_t restart_wdt(void)
{
	uint8_t ret_code = 0x00;
	uint8_t wakeup_response[ATCA_RSP_SIZE_MIN];
	
	/* IDLE */
	ret_code = atca_idle();
	if (ret_code == ATCA_SUCCESS)
	{
		printf("Idle success!\r\n\n");
	}
	else
	{
		printf("Idle failed!\r\n");
		printf("Idle ret_code = 0x%02X\r\n\n",ret_code);
	}
	
	/* WAKE UP DEVICE */
	ret_code = atca_wakeup(wakeup_response);
	#ifdef	CHECK_RET_CODE
	if (ret_code == ATCA_SUCCESS)
	#else
	if ((wakeup_response[0] == 0x04) && (wakeup_response[1] == 0x11))
	#endif
	{
		printf("Wakeup success!\r\n\n");
	}
	else
	{
		printf("Wakeup failed!\r\n\n");
	}
	
	return ret_code;
}

uint8_t ecc508_personalization_config_zone(void)
{
	uint8_t ret_code = 0x00;
	uint8_t wakeup_response[ATCA_RSP_SIZE_MIN];

	do 
	{
		/* WAKE UP DEVICE */
		ret_code = atca_wakeup(wakeup_response);
#ifdef	CHECK_RET_CODE
		if (ret_code == ATCA_SUCCESS)
#else
		if ((wakeup_response[0] == 0x04) && (wakeup_response[1] == 0x11))
#endif
		{
			printf("Wakeup success!\r\n\n");
		}
		else
		{
			printf("Wakeup failed!\r\n\n");
			break;
		}

		/* WRITE SLOT CONFIG */
		ret_code = ecc108_write_slot_config();
		if (ret_code != ATCA_SUCCESS)
		{
			printf("\r\nWrite SlotConfig failed, \r\nDEVICE MAY BE LOCKED!\r\n\n");
			printf("ATCA_CMD_FAIL ret_code = 0x%02X\r\n\n",ret_code);
			break;
		}

		/* IDLE */
		ret_code = restart_wdt();
		if (ret_code == ATCA_SUCCESS)
		{
			printf("Idle success!\r\n\n");
		}
		else
		{
			printf("Idle failed! ret_code = 0x%02X\r\n\n",ret_code);
			break;
		}

		/* WRITE KEY CONFIG */
		ret_code = ecc108_write_key_config();
		if (ret_code != ATCA_SUCCESS)
		{
			printf("Write KeyConfig failed! ret_code = 0x%02X\r\n\n",ret_code);
			break;
		}

		/* IDLE */
		ret_code = restart_wdt();
		if (ret_code == ATCA_SUCCESS)
		{
			printf("Idle success!\r\n\n");
		}
		else
		{
			printf("Idle failed!\r\n");
			printf("Idle ret_code = 0x%02X\r\n\n",ret_code);
			break;
		}

		/* LOCK CONFIG ZONE */
		ret_code = ecc108_lock_config_zone();
		if (ret_code == ATCA_SUCCESS)
		{
			printf("lock config zone success!\r\n\n");
		}
		else
		{
			printf("lock config zone failed!\r\n\n");
			break;
		}

		/* IDLE */
		ret_code = restart_wdt();
		if (ret_code == ATCA_SUCCESS)
		{
			printf("Idle success!\r\n\n");
			puts(CONF_CONFIG_ZONE_DONE);
		}
		else
		{
			printf("Idle failed!\r\n");
			printf("Idle ret_code = 0x%02X\r\n\n",ret_code);
			break;
		}
	} while (0);

	atca_sleep();
	
	return ret_code;
}


uint8_t ecc508_personalization_user_data_zone(void)
{
	
	uint8_t ret_code = 0x00;
	uint8_t wakeup_response[ATCA_RSP_SIZE_MIN];

	do
	{
		/* WAKE UP DEVICE */
		ret_code = atca_wakeup(wakeup_response);
		#ifdef	CHECK_RET_CODE
		if (ret_code == ATCA_SUCCESS)
		#else
		if ((wakeup_response[0] == 0x04) && (wakeup_response[1] == 0x11))
		#endif
		{
			printf("Wakeup success!\r\n\n");
		}
		else
		{
			printf("Wakeup failed!\r\n\n");
			break;
		}
		
		/* WRITE DATA ZONE */
		ret_code = ecc108_write_data_zone();
		if (ret_code != ATCA_SUCCESS)
		{
			printf("\rDEVICE MAY BE LOCKED!\r\n");
			printf("ATCA_CMD_FAIL ret_code = 0x%02X\r\n\n",ret_code);
			break;
		}

		/* IDLE */
		ret_code = restart_wdt();
		if (ret_code == ATCA_SUCCESS)
		{
			printf("Idle success!\r\n\n");
		}
		else
		{
			printf("Idle failed!\r\n");
			printf("Idle ret_code = 0x%02X\r\n\n",ret_code);
			break;
		}

		/* LOCK DATA ZONE */
		ret_code = ecc108_lock_data_zone();
		if (ret_code == ATCA_SUCCESS)
		{
			printf("lock data zone success!\r\n\n");
		}
		else
		{
			printf("lock data zone failed!\r\n\n");
			break;
		}
	} while (0);

	atca_sleep();
	
	return ret_code;
		
		
}