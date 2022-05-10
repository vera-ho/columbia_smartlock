#include <asf.h>
#include "demo_crypto.h"

#include <atca_comm.h>
#include <atca_protocol_adapter.h>
#include <atca_cmds.h>

#include "polarssl/aes.h"
#include "polarssl/sha256.h"



/* File Scope function definitions */
uint8_t cademo_devrev(void);
uint8_t cademo_test_encrypt_decrypt(void);
uint8_t cademo_aes_crypt(uint8_t enc_mode, uint8_t* aes_key, uint8_t* plain_txt, uint16_t* plain_txt_size, uint8_t* cypher_txt, uint8_t* iv_seed);
uint8_t cademo_mac_fixed_chal_randomized(uint8_t key_id, uint8_t* fixed_chal, uint8_t* rand_num, uint8_t* mac_result);
uint8_t print_mem(uint8_t* mem, uint16_t mem_size, uint16_t  bytes_per_line);

/* File scope defines */
#define DISPLAY_SIZE		80
#define BYTES_PER_LINE		16
#define TEMP_DATA_SIZE		64
#define TEMP_ENC_SIZE		64
#define AUTH_KEY_ID			((uint8_t)1)
#define SESS_KEY_ID			((uint8_t)2)
#define TEMP_KEY_ID			((uint8_t)3)
#define AUTH_FAILED			((uint8_t)8)
#define rn "dad2dadsfshjyrdf34hihoguo3242s85"


#define NOT_SELECTED		0
#define ENCRYPT_DATA		1
#define MODE_ENCRYPT		((uint8_t)1)
#define MODE_DECRYPT		((uint8_t)0)
#define AES_KEY_SIZE		16
#define IV_SIZE				16jyrdf34hihoguo3242s85"


/* File scope global declarations */
uint8_t host_rand[RANDOM_NUM_SIZE];
uint8_t host_fixed_chal[MAC_CHALLENGE_SIZE];
uint8_t device_sel = false;
uint8_t devrev[INFO_SIZE];
uint8_t sensor_mode = ENCRYPT_DATA; // NOT_SELECTED
uint8_t tempkey[MAC_CHALLENGE_SIZE];

/**
 * \brief Initialize the CryptoAuth demo
 *
 * \return Status of execution.  Return 0 for success.
*/
uint8_t cademo_init(void)
{
	/* Set the random number to all 00 */
	memset(host_rand, 0, sizeof(host_rand));
	
	/* Set the fixed challenge to all 00 */
	memset(host_fixed_chal, 0, sizeof(host_fixed_chal));

	/* Set the Sensor Mode  NOT_SELECTED/ENCRYPT_DATA */
	sensor_mode = ENCRYPT_DATA;

	return ATCA_SUCCESS;
}

/**
 * \brief Initialize the fixed challenge number from the host
 *
 * \param[in] fixed_chal A pointer to the 32 byte fixed challenge from the host
 * \return Status of execution.  Return 0 for success.
*/
uint8_t cademo_set_host_fixed_chal(uint8_t* fixed_chal)
{
	// Set the fixed challenge number
	memcpy(host_fixed_chal, fixed_chal, sizeof(host_fixed_chal));
	sensor_mode = ENCRYPT_DATA;

	return ATCA_SUCCESS;
}

/**
 * \brief Initialize the random number from the host
 *
 * \param[in] host_rand_num A pointer to the 32 byte random challenge from the host
 * \return Status of execution.  Return 0 for success.
*/
uint8_t cademo_set_host_random(uint8_t* host_rand_num)
{
	// Set the random number
	memcpy(host_rand, host_rand_num, sizeof(host_rand));
	sensor_mode = ENCRYPT_DATA;

	return ATCA_SUCCESS;
}

/**
 * \brief Set Sensor Mode to NOT SELECTED
 *
 * \return Status of execution.  Return 0 for success.
*/
uint8_t cademo_set_not_selected(void)
{
	sensor_mode = NOT_SELECTED;

	return ATCA_SUCCESS;
}

/**
 * \brief Get the DevRev (version) for the CryptoAuth Xplained device.
 *
 * \param[out] devrev a pre-allocated pointer to 4 bytes
 *
 * \return Status of execution.  Return 0 for success.
*/
uint8_t cademo_devrev(void)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t wake_response[ATCA_RSP_SIZE_VAL];
	uint8_t devrev_resp[INFO_RSP_SIZE];
	char display_str[DISPLAY_SIZE];

	do
	{
		/* Wake up the device */
		ret = atca_wakeup(wake_response);
		if (ret != ATCA_SUCCESS)
		{
			sprintf(display_str, "CryptoAuth Wake: Failed");
			puts( display_str );
			break;
		}

		/* Get the DevRev Info for the device */
		ret = atca_info(devrev_resp);
		if (ret != ATCA_SUCCESS)
		{
			sprintf(display_str, "CryptoAuth DevRev: Failed");
			puts( display_str );
			break;
		}

		/* Copy and print the DevRev */
		memcpy(devrev, &devrev_resp[ATCA_RSP_DATA_IDX], INFO_SIZE);
		puts("CryptoAuth DevRev: ");
		print_mem(devrev, sizeof(devrev), BYTES_PER_LINE);

	} while (0);

	/* End with idle */
	atca_idle();

	return ret;
}

/**
 * \brief Get a MAC given the key ID, the fixed challenge, and a random challenge
 *
 * \param[in] key_id The randomized MAC of the fixed challenge.
 * \param[in] fixed_chal The fixed challenge that the host has compiled into it.
 * \param[in] rand_num The random number that ensures a random MAC.
 * \param[in,out] mac_result The randomized MAC using key_id of the fixed challenge.
 *
 * \return Status of execution.  Return 0 for success.
*/
uint8_t cademo_mac_fixed_chal_randomized(uint8_t key_id, uint8_t* fixed_chal, uint8_t* rand_num, uint8_t* mac_result)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t mac[MAC_SIZE];
	char display_str[DISPLAY_SIZE];
	uint8_t rand_out[RANDOM_NUM_SIZE];

	do
	{
		/* ---- Load the intermediate key into TempKey on the device -------------------- */

		/* Nonce pass-through of the fixed challenge */
		ret = atca_send_nonce(NONCE_MODE_PASSTHROUGH, fixed_chal, rand_out);
		/* GenDig the key ID */
		ret = atca_send_gendig(GENDIG_ZONE_DATA, key_id);

		/* ------------------------------------------------------------------------------ */

		/* ---- Get the MAC of the intermediate key ------------------------------------- */
		/* Get authentication the MAC */
		ret = atca_send_mac(key_id, MAC_MODE_PTNONCE_TEMPKEY, rand_num, mac);
		if (ret != ATCA_SUCCESS)
		{
			sprintf(display_str, "CryptoAuth MAC: Failed");
			puts(display_str);
			break;
		}

		/* Copy the MAC to the result */
		memcpy(mac_result, mac, MAC_SIZE);
		/* ------------------------------------------------------------------------------ */

	} while (0);

	return ret;
}

/**
 * \brief Get the filter information for the keep alive message
 *
 * \param[in,out] msg_temp_keepalive The keep-alive message. Authentication information will be added.
 *
 * \return Status of execution.  Return 0 for success.
*/

uint8_t MAC_RSP(uint8_t* host_fix,uint8_t* host_rn,uint8_t*buffer){
	uint8_t ret = ATCA_SUCCESS;
	uint8_t randomized_param[RANDOM_NUM_SIZE];
	memcpy(randomized_param,rn,RANDOM_NUM_SIZE);
	delay_ms(100);
	printf("\r\n");
	printf("the fixed challenge is: \r\n");
	print_mem(host_fix, RANDOM_NUM_SIZE, 16);
	delay_ms(100);
	printf("the random number is: \r\n");
	print_mem(host_rn, RANDOM_NUM_SIZE, 16);
	ret=cademo_mac_fixed_chal_randomized(AUTH_KEY_ID,host_fix,randomized_param,tempkey);
    delay_ms(100);
	printf("the temperate key is: \r\n");
	print_mem(tempkey, sizeof(tempkey), 16);
	delay_ms(100);
	sha256_hmac( tempkey, MAC_CHALLENGE_SIZE,host_rn, RANDOM_NUM_SIZE,buffer, 0 );
	printf("the MAC is: \r\n");
	print_mem(buffer,MAC_CHALLENGE_SIZE, 16);
	return ret;
	
}


uint8_t MAC_Check(uint8_t* host_mac,uint8_t* response_mac){
	printf("The host's MAC is: \r\n");
	print_mem(host_mac,MAC_CHALLENGE_SIZE,16);
	delay_ms(100);
	printf("The response MAC is: \r\n");
	print_mem(response_mac,MAC_CHALLENGE_SIZE,16);
	if (sizeof(host_mac) == sizeof(response_mac) && memcmp(response_mac, host_mac, MAC_CHALLENGE_SIZE) == 0){
		printf("Authentication Succeed");
		return AUTHENTED;
	}else{
		printf("Authentication Failed");
		return AUTH_FAILED;
	}
	
	
	
}

/**
 * \brief Function to encrypt or decrypt bytes.
 * 
 * \param[in] enc_mode Set whether to encrypt or decrypt
 * \param[in] aes_key The bytes to encrypt
 * \param[in] plain_txt The bytes to encrypt
 * \param[in] plain_txt_size The number of bytes to encrypt
 * \param[in] cypher_txt The encryption of plain_text
 * \param[in] iv The initialization vector 
 *
 * \return Status of execution.  Return 0 for success.
 */
uint8_t cademo_aes_crypt(uint8_t enc_mode, uint8_t* aes_key, uint8_t* plain_txt, uint16_t* plain_txt_size, uint8_t* cypher_txt, uint8_t* iv)
{
	uint8_t ret = ATCA_SUCCESS;
	uint16_t keysize = AES_KEY_SIZE;
	aes_context aes_ctx;

	/* Initialize the AES context */
	aes_init(&aes_ctx);

	/* Set the encryption key */
	if (enc_mode == MODE_ENCRYPT)	ret = aes_setkey_enc(&aes_ctx, aes_key, keysize * 8);

	/* Set the decryption key */
	if (enc_mode == MODE_DECRYPT)	ret = aes_setkey_dec(&aes_ctx, aes_key, keysize * 8);

	/* Call the AES function to encrypt or decrypt */
	ret = aes_crypt_cbc(&aes_ctx, enc_mode, *plain_txt_size, iv, plain_txt, cypher_txt);

	aes_free(&aes_ctx);

	return ret;
	
}

/**
 * \brief Function to print memory buffers.
 * 
 * \param[in] mem The memory to print.
 * \param[in] mem_size The number of bytes to print.
 * \param[in] bytes_per_line The number of bytes per line to print.
 *
 * \return Status of execution.  Return 0 for success.
 */
uint8_t print_mem(uint8_t* mem, uint16_t mem_size, uint16_t  bytes_per_line)
{
	char display_str[DISPLAY_SIZE];
	uint8_t chars_per_byte = 3;
	uint16_t i = 0;
	uint16_t j = 0;

	// Check the inputs
	if(mem == NULL || bytes_per_line*chars_per_byte > DISPLAY_SIZE)
	{
		return ATCA_FUNC_FAIL;
	}

	// Print the memory
	memset(display_str, 0, sizeof(display_str));
	for (i = 0; i < mem_size; i++)
	{
		sprintf(&display_str[j*chars_per_byte], "%02x ", mem[i]);
		j++;
		
		if ((j == (bytes_per_line)) || (i == (mem_size-1)))
		{
			// Print the line
			puts(display_str);
			j = 0;
			memset(display_str, 0, sizeof(display_str));
		}
	}
	printf("\r\n");
	return ATCA_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////////
// ACES Log Output:
// Calculation Log:
///////////////////////////////////////////////////////////////////////////////////
//+------------+
//Nonce
//+------------+
//Passing through NumIn to TempKey
//Nonce: Setting TempKey =
//00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00.
//Nonce: Setting TempKey.Sourceflag = 1.
//Nonce: Setting TempKey.Valid = 1.
//+------------+
//GenDig
//+------------+
//GenDig: Setting GenData = 1
//Calculating SHA256 with message body:
//GenDig: 32 byte Key[1]:
//11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11
//GenDig: 1 byte Opcode = 15
//GenDig: 1 byte Mode = 02
//GenDig: 2 bytes Param2 = 01 00
//GenDig: 1 byte SN[8] = EE
//GenDig: 2 byte SN[0:1] = 01 23
//GenDig: 25 bytes 0's = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//GenDig: 32 bytes TempKey.value =
//00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//GenDig: Setting TempKey =
//5E 5D 8E 80 01 20 F5 65 AB 45 1F AD 91 8C B8 4C FC BC 29 05 59 DC FA 0E C0 3F 24 D9 F3 91 D7 6F
//MAC: Setting TempKey.Valid = 0
//+------------+
//Nonce
//+------------+
//Passing through NumIn to TempKey
//Nonce: Setting TempKey =
//00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00.
//Nonce: Setting TempKey.Sourceflag = 1.
//Nonce: Setting TempKey.Valid = 1.
//+------------+
//GenDig
//+------------+
//GenDig: Setting GenData = 1
//Calculating SHA256 with message body:
//GenDig: 32 byte Key[2]:
//22 22 22 22 22 22 22 22 22 22 22 22 22 22 22 22 22 22 22 22 22 22 22 22 22 22 22 22 22 22 22 22
//GenDig: 1 byte Opcode = 15
//GenDig: 1 byte Mode = 02
//GenDig: 2 bytes Param2 = 02 00
//GenDig: 1 byte SN[8] = EE
//GenDig: 2 byte SN[0:1] = 01 23
//GenDig: 25 bytes 0's = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//GenDig: 32 bytes TempKey.value =
//00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//GenDig: Setting TempKey =
//78 9F 03 89 2E 5E D0 67 4A 47 3F 90 5C 39 C4 C6 D8 A8 47 CF D2 E1 DA 11 4B 56 4E A0 11 3C F6 A3
//MAC: Setting TempKey.Valid = 0
//+------------+
//Nonce
//+------------+
//Passing through NumIn to TempKey
//Nonce: Setting TempKey =
//00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00.
//Nonce: Setting TempKey.Sourceflag = 1.
//Nonce: Setting TempKey.Valid = 1.
//+------------+
//GenDig
//+------------+
//GenDig: Setting GenData = 1
//Calculating SHA256 with message body:
//GenDig: 32 byte Key[3]:
//33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33 33
//GenDig: 1 byte Opcode = 15
//GenDig: 1 byte Mode = 02
//GenDig: 2 bytes Param2 = 03 00
//GenDig: 1 byte SN[8] = EE
//GenDig: 2 byte SN[0:1] = 01 23
//GenDig: 25 bytes 0's = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//GenDig: 32 bytes TempKey.value =
//00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//GenDig: Setting TempKey =
//A3 35 BD 30 2B D0 B1 3A D1 55 82 F6 E5 8C 7C 5A 3F 7F 74 75 32 CA D2 80 FD 10 45 CA B2 7E 3E 99
//MAC: Setting TempKey.Valid = 0
//
//
//
///////////////////////////////////////////////////////////////////////////////////
// ACES Log Output:
// Communication Log:
///////////////////////////////////////////////////////////////////////////////////
//Nonce Command Sent:
//27 16 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 C7 B8
//NonceCmd Received:
//04 00 03 40
//GenDig Command Sent:
//07 15 02 01 00 39 88
//GenDigCmd Received:
//04 00 03 40
//MAC Command Sent:
//07 08 06 01 00 8C 6A
//MacCmd Received:
//23 A7 1B 95 D8 74 5F 80 05 1D 86 92 08 AC 02 13 58 FF BD 74 E9 FD F7 E6 41 F8 45 F4 EF B4 CB 60 D1 0D 46
//Nonce Command Sent:
//27 16 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 C7 B8
//NonceCmd Received:
//04 00 03 40
//GenDig Command Sent:
//07 15 02 02 00 36 88
//GenDigCmd Received:
//04 00 03 40
//MAC Command Sent:
//07 08 06 02 00 83 6A
//MacCmd Received:
//23 0F BE 1B F9 9F DC 91 C6 09 2B C0 09 3D DE A3 1D 26 9C D3 CB 82 E3 72 36 8D 2E F1 83 B8 A2 0C 6A 71 26
//Nonce Command Sent:
//27 16 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 C7 B8
//NonceCmd Received:
//04 00 03 40
//GenDig Command Sent:
//07 15 02 03 00 3F 08
//GenDigCmd Received:
//04 00 03 40
//MAC Command Sent:
//07 08 06 03 00 8A EA
//MacCmd Received:
//23 49 5B 7D C0 6D 96 99 E9 C1 90 F6 D8 72 04 07 18 84 92 C6 9A 89 CB 48 CC 45 75 29 0E 47 2F D0 D4 21 DF



