/**
 *
 * \file
 *
 * \brief WINC1500 temperature sensor demo.
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
#include <ctype.h>
#include <stdio.h>
#include <atca_comm.h>
#include <atca_protocol_adapter.h>
#include <atca_cmds.h>
#include "asf.h"
#include "demo.h"
#include "bsp/include/nm_bsp.h"
#include "socket/include/socket.h"
#include "driver/include/m2m_wifi.h"
#include "conf_winc.h"
#include "demo_global_defines.h"
#include "demo_console_init.h"
#include "demo_crypto_init.h"
#include "atca_protocol_adapter.h"
#include "atca_config.h"
#include "atca_i2c.h"
#include "atca_comm.h"
#include "demo_crypto.h"
#include "atca_protocol_adapter.h"
#include "polarssl-1.3.8\include\polarssl\sha256.h"
#include <at30tse75x.h>

#define STRING_EOL    "\r\n"
#define STRING_HEADER "-- WINC1500 Smartlock example --"STRING_EOL \
	"-- "BOARD_NAME " --"STRING_EOL	\
	"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL
	
	


#define fc "sfe45f3de2de4fr5ws3de4fre3w3swd3"
#define rn2 "324bk32bl443jblesefeseferfeeaew2"
#define AUTH_KEY_ID			((uint8_t)1)
#define authSuccess        "Authentication Successed"


/** UART module for debug. */
static struct usart_module;

/** Message format definitions. */
typedef struct s_msg {
	uint8_t status;
} t_s_msg;

static t_s_msg msg_wifi = {
      .status=NULL,	
};

/**Random check*/

/** Receive buffer definition. */
static uint8_t gau8SocketTestBuffer[MAIN_WIFI_M2M_BUFFER_SIZE];

/** Socket for TCP communication */
static SOCKET tcp_server_socket = -1;
static SOCKET tcp_client_socket = -1;

/** Wi-Fi connection state */
static uint8_t wifi_connected;

/*security part*/
static uint8_t usedRandom[5][32];
int count=0;
uint8_t check;
uint8_t bufferI[32];
uint8_t bufferII[32];
uint8_t bufferIII[32];
uint8_t MAC_Buffer[32];
uint8_t ret=ATCA_SUCCESS;

static bool check_random(uint8_t a[32],uint8_t b[10][32],int c){
	int used =0;
	for(int i=0;i<10;i++){
		if(memcmp(a,b[i],32)==0){
			used++;
		}	
	}
	if(used==0){
	
		memcpy(b[c],a,32);
		if(c==10){
			c=0;
		}else{
			c++;
		}
		return true;
	}else{
		return false;
	}
	
}

/** socket callback function */
static void socket_cb(SOCKET sock, uint8_t u8Msg, void *pvMsg)
{
	switch (u8Msg) {
		/* Socket bind */
		case SOCKET_MSG_BIND:
		{
			tstrSocketBindMsg *pstrBind = (tstrSocketBindMsg *)pvMsg;
			if (pstrBind && pstrBind->status == 0) {
				printf("socket_cb: bind success!\r\n");
				listen(tcp_server_socket, 0);
				} else {
				printf("socket_cb: bind error!\r\n");
				close(tcp_server_socket);
				tcp_server_socket = -1;
			}
		}
		break;

		/* Socket listen */
		case SOCKET_MSG_LISTEN:
		{
			tstrSocketListenMsg *pstrListen = (tstrSocketListenMsg *)pvMsg;
			if (pstrListen && pstrListen->status == 0) {
				printf("socket_cb: listen success!\r\n");
				accept(tcp_server_socket, NULL, NULL);
				} else {
				printf("socket_cb: listen error!\r\n");
				close(tcp_server_socket);
				tcp_server_socket = -1;
			}
		}
		break;

		/* Connect accept */
		case SOCKET_MSG_ACCEPT:
		{
			tstrSocketAcceptMsg *pstrAccept = (tstrSocketAcceptMsg *)pvMsg;
			if (pstrAccept) {
				printf("socket_cb: accept success!\r\n");
				accept(tcp_server_socket, NULL, NULL);
				tcp_client_socket = pstrAccept->sock;
				recv(tcp_client_socket, gau8SocketTestBuffer, sizeof(gau8SocketTestBuffer), 0);
				} else {
				printf("socket_cb: accept error!\r\n");
				close(tcp_server_socket);
				tcp_server_socket = -1;
			}
		}
		break;

		/* Message send */
		case SOCKET_MSG_SEND:
		{
			printf("socket_cb: send success!\r\n");
			break;
			//close(tcp_client_socket);
		}
		

		/* Message receive */
		case SOCKET_MSG_RECV:
		{
			tstrSocketRecvMsg *pstrRecv = (tstrSocketRecvMsg *)pvMsg;
			if (pstrRecv && pstrRecv->s16BufferSize > 0) {
				printf("socket_cb: recv success!\r\n");						
	
						memcpy(bufferI,pstrRecv->pu8Buffer,32);
						memcpy(bufferII,pstrRecv->pu8Buffer+32,32);
						memcpy(bufferIII,pstrRecv->pu8Buffer+64,32);
						delay_ms(100);
						if(check_random(bufferII,usedRandom,count))
						{
						printf("random number is valid\r\n");
						/*calculating MAC*/
						MAC_RSP(bufferI,bufferII,MAC_Buffer);
						check = MAC_Check(bufferIII,MAC_Buffer);
						}else{
						printf("random number is invalid\r\n");
							check=FAIL_TO_UNLOCK;
						}
						if(check==AUTHENTED){
							msg_wifi.status=UNLOCK;
						}else{
							msg_wifi.status=FAIL_TO_UNLOCK;
						}																
				         send(tcp_client_socket, &msg_wifi, sizeof(t_s_msg), 0);
				         msg_wifi.status=NULL;
						 close(tcp_client_socket);
				} else {
				printf("socket_cb: recv error!\r\n");
				close(tcp_server_socket);
				tcp_server_socket = -1;
			}
		}

		break;

		default:
		break;
	}
}


static void wifi_cb(uint8_t u8MsgType, void *pvMsg)
{
	switch (u8MsgType) {
	case M2M_WIFI_RESP_CON_STATE_CHANGED:
	{
		tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
		if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) {
			printf("wifi_cb: M2M_WIFI_RESP_CON_STATE_CHANGED: CONNECTED\r\n");
			m2m_wifi_request_dhcp_client();
		} else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) {
			printf("wifi_cb: M2M_WIFI_RESP_CON_STATE_CHANGED: DISCONNECTED\r\n");
			wifi_connected = 0;
			m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, (char *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);
		}
	}
	break;

	case M2M_WIFI_REQ_DHCP_CONF:
	{
		uint8_t *pu8IPAddress = (uint8_t *)pvMsg;
		wifi_connected = 1;
		printf("wifi_cb: M2M_WIFI_REQ_DHCP_CONF: IP is %u.%u.%u.%u\r\n",
				pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);
	}
	break;

	default:
		break;
	}
}

/**
 * \brief Configure UART console.
 */

int main(void){
	tstrWifiInitParam param;
	int8_t ret;
	struct sockaddr_in addr;
	system_init();
	/* Initialize the UART console. */
	configure_console();
	printf(STRING_HEADER);
	/* Initialize the delay driver. */
	delay_init();
	atca_set_device_address(I2CBUS_0, ATCA_I2C_CONFIGURED_ADDRESS);
	/*Set the delay for ECCxxx*/
	atca_set_device_delay();
	atca_init();
	cademo_init();
	nm_bsp_init();
	/* Initialize socket address structure. */
	addr.sin_family = AF_INET;
	addr.sin_port = _htons(MAIN_WIFI_M2M_SERVER_PORT);
	addr.sin_addr.s_addr = 0;

	/* Initialize Wi-Fi parameters structure. */
	memset((uint8_t *)&param, 0, sizeof(tstrWifiInitParam));

	/* Initialize Wi-Fi driver with data and status callbacks. */
	param.pfAppWifiCb = wifi_cb;
	ret = m2m_wifi_init(&param);
	if (M2M_SUCCESS != ret) {
		printf("main: m2m_wifi_init call error!(%d)\r\n", ret);
		while (1) {
		}
	}

	/* Initialize socket module */
	socketInit();
	registerSocketCallback(socket_cb, NULL);

	/* Connect to router. */
	m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, (char *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);

	while (1) {
		/* Handle pending events from network controller. */
		m2m_wifi_handle_events(NULL);

		if (wifi_connected == M2M_WIFI_CONNECTED) {
			if (tcp_server_socket < 0) {
				/* Open TCP server socket */
				if ((tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
					printf("main: failed to create TCP server socket error!\r\n");
					continue;
				}

				/* Bind service*/
				bind(tcp_server_socket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
			}
		}
	}

	return 0;
}

	