/*
 * demo_console_init.h
 *
 * Created: 3/8/2015 3:21:20 PM
 *  Author: oscar.sanchez
 */ 


#ifndef DEMO_CONSOLE_INIT_H_
#define DEMO_CONSOLE_INIT_H_



/** Usart Receive menu buffer **/
/** UART module for debug. */
extern struct usart_module cdc_uart_module;
extern volatile uint8_t u8menuBuff[2];
extern volatile uint8_t u8menuState;

extern void usart_read_callback(const struct usart_module *const usart_module);
extern void configure_console(void);

#endif /* DEMO_CONSOLE_INIT_H_ */