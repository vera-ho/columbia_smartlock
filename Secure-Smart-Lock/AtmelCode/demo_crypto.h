/**
 *
 * \file
 *
 * \brief CryptoAuth Xplained demo.
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



#ifndef DEMO_CRYPTO_H_
#define DEMO_CRYPTO_H_

#include "demo.h"

#ifdef __cplusplus
extern "C" {
	#endif

// Function prototypes - CryptoAuth entry points
uint8_t cademo_init(void);
uint8_t cademo_set_host_random(uint8_t* host_rand_num);
uint8_t cademo_set_host_fixed_chal(uint8_t* fixed_chal);
uint8_t cademo_set_not_selected(void);
uint8_t print_mem(uint8_t* mem, uint16_t mem_size, uint16_t  bytes_per_line);
uint8_t cademo_mac_fixed_chal_randomized(uint8_t key_id, uint8_t* fixed_chal, uint8_t* rand_num, uint8_t* mac_result);
uint8_t MAC_RSP(uint8_t* host_fix,uint8_t* host_rn,uint8_t*buffer);
uint8_t MAC_Check(uint8_t* host_mac,uint8_t* response_mac);

#ifdef __cplusplus
}
#endif

#endif /* DEMO_CRYPTO_H_ */