/*
 * demo_console_init.c
 *
 * Created: 3/8/2015 3:20:59 PM
 *  Author: oscar.sanchez
 */ 

#include "asf.h"
#include "demo_global_defines.h"
#include "stdio_serial.h"

/** Usart Receive menu buffer **/
struct usart_module cdc_uart_module;
volatile uint8_t u8menuBuff[2] = {};
volatile uint8_t u8menuState = 'M';

void usart_read_callback(const struct usart_module *const usart_module)
{
	usart_read_buffer_job(&cdc_uart_module,(uint8_t*)&u8menuBuff[0],1);
	u8menuState = u8menuBuff[0];
	asm("nop");
}


/**
 * \brief Configure UART console.
 */
void configure_console(void)
{
	struct usart_config usart_conf;

	usart_get_config_defaults(&usart_conf);
	usart_conf.mux_setting = CONF_STDIO_MUX_SETTING;
	usart_conf.pinmux_pad0 = CONF_STDIO_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = CONF_STDIO_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = CONF_STDIO_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = CONF_STDIO_PINMUX_PAD3;
	usart_conf.baudrate    = CONF_STDIO_BAUDRATE;

	stdio_serial_init(&cdc_uart_module, CONF_STDIO_USART_MODULE, &usart_conf);
	usart_register_callback(&cdc_uart_module, usart_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
	usart_enable_callback(&cdc_uart_module, USART_CALLBACK_BUFFER_RECEIVED);
	usart_read_buffer_job(&cdc_uart_module,(uint8_t*)&u8menuBuff[0],1);
	usart_enable(&cdc_uart_module);
}