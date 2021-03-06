/*
 * STM32_TNC.c
 *
 *  Created on: Nov 21, 2017
 *      Author: nje
 */

#include "STM32_TNC.h"
//#include <ax5043.h>
#include <main.h>
//#include <stm32f4xx_hal.h>
//#include <stm32f4xx_hal_gpio.h>
//#include <stm32f4xx_hal_uart.h>
#include <sys/_stdint.h>
//#include <Serial.h>
//#include "defines.h"
#include "stdbool.h"
//#include "Radio.h"
//#include "CAPE-3_Status_Vars.h"
//#include "beacon.h"
//#include "stm_power_ina219.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

uint8_t tnc_buff[1024];
uint16_t pkt_len = 0;
uint8_t tnc_message[1];

uint8_t tnc_array[1024];
uint8_t tnc_cmd_array[1024];
uint16_t last_tnc_count = 0;
uint16_t last_tnc_cmd_len = 0;


int TNC_Tx(uint8_t *message, uint16_t msg_len)
{

	uint8_t tnc_msg[400];
	uint16_t current_index = 0;


	//Header bytes

//	if (current_ax->RADIO_TYPE == 'u')
//	{
//		TNC_UHF
//		if (enable_uhf_hp)
//		{
//			RADIO_FE_NORMAL
//		}
//		RADIO_UHF_TX
//	}
//	else if (current_ax->RADIO_TYPE == 'v')
//	{
//		TNC_VHF
//		if (enable_vhf_hp)
//		{
//			RADIO_FE_HP
//		}
//		RADIO_VHF_TX
//	}

	/*
	 * SSID format: CRRSSID0
	 *
	 * TNC Header Format:
	 * 0: 0xC0: Delimiting Character
	 * 1: 0x00: remainder of frame is HDLC DATA
	 * 			DESTINATION ADDRESS: "W5UL  1"
	 * 2: 0xAE: 'W' << 1 (0x57)
	 * 3: 0x6A (j): '5' << 1 (0x35)
	 * 4: 0xAA: 'U' << 1 (0x55)
	 * 5: 0x98: 'L' << 1 (0x4C)
	 * 6: 0x40: ' ' << 1 (0x20)
	 * 7: 0x40: ' '		 (0x20)
	 * 8: 0x62 (b):  0110 0010 (response, res, res, 0001)
	 * 			SOURCE ADDRESS: "W5UL  0"
	 * 9: 0xAE: 'W' << 1
	 * 10: 0x6A: '5' << 1
	 * 11: 0xAA: 'U' << 1
	 * 12: 0x98: 'L' << 1
	 * 13: 0x40: ' ' << 1
	 * 14: 0x40: ' ' << 1
	 * 15: 0xF7:	1111 0111 (command, res, res, 1011)
	 * 16: 0x03:	Control field (Unnumbered information frame)
	 * 17: 0xF0:	PID field (no layer 3 protocol implemented)
	 *
	 *
	 *
	 *
	 */



	uint8_t tnc_header[18] = {0xC0, 0x00, 0xAE, 0x6A, 0xAA, 0x98, 0x40, 0x40, 0x62, 0xAE, 0x6A, 0xAA, 0x98, 0x40, 0x40, 0xF7, 0x03, 0xF0};
	uint8_t end_char[1] = {0xC0};

	//	TerminalWrite("Sending test to TNC:"); NL();
	//	TerminalWrite(tnc_header); NL();

//	if (current_ax->ax_fm_mode != 't')
//	{
//		AX_FM_TX();
//
//		//PulseWDT();
//
//		HAL_Delay(500);
//		//			HAL_Delay(10);
//	}
//	else
//	{
//		TerminalWrite("AX5043 already in FM TX mode!"); NL();
//	}


	//PulseWDT();

//	HAL_Delay(1);
//
//	int16_t urb_v_trans = getBusVoltage_mV(URB_INA_ADD);
//	HAL_Delay(1);
//	int16_t urb_i_trans = getCurrent_mA(URB_INA_ADD);
//	HAL_Delay(1);

	//PulseWDT();
//
//	if (message[22] == 'B')
//	{
//		//		packet_length = 201;
//
//		memcpy(message + BEACON_PKT_URB_V_TRANS, Split16Bit((uint16_t)urb_v_trans), 2);
//
//		memcpy(message + BEACON_PKT_URB_I_TRANS, Split16Bit((uint16_t)urb_i_trans), 2);
//
//		//		TerminalWrite("URB Transmit: ");
//		//		TerminalNum(urb_v_trans); TerminalWrite(" mV, ");
//		//		TerminalNum(urb_i_trans); TerminalWrite(" mA."); NL();
//
//
//	}



//	NL();
	// perform escaping
	for (uint16_t i = 0; i < msg_len; i ++)
	{
		//		TerminalHex(message[i]);
		if (message[i] == 0xC0)
		{

			//		TerminalHex(message[i]);
			//		TerminalNum(i);
			//		TerminalWrite(" Escaping FEND...   ");
			tnc_msg[current_index] = 0xDB;
			current_index ++;
			tnc_msg[current_index] = 0xDC;
		}
		else if (message[i] == 0xDB)
		{
			//		TerminalHex(message[i]);
			//		TerminalNum(i);
			//		TerminalWrite("Escaping FESC...   ");
			tnc_msg[current_index] = 0xDB;
			current_index ++;
			tnc_msg[current_index] = 0xDD;
		}
		else
		{
			tnc_msg[current_index] = message[i];

		}
		current_index ++;
	}

//	NL(); NL();
//	TerminalWrite("Length: "); TerminalNum(current_index); NL();




	//	     HAL_UART_Transmit(&huart2, startHeader, 2, 50);
	//
	//	     HAL_UART_Transmit(&huart2, destination, 7, 50);
	//
	//	     HAL_UART_Transmit(&huart2, callsign, 7, 50);
	//
	//	     HAL_UART_Transmit(&huart2, digipeat, 7, 50);
	//
	//	     HAL_UART_Transmit(&huart2, endHeader, 2, 50);

	HAL_UART_Transmit(&huart1, tnc_header, 18, 50);

	HAL_UART_Transmit(&huart1, tnc_msg, current_index, 500);

	HAL_UART_Transmit(&huart1, end_char, 1, 50);

	return current_index;


}

void TNC_Tx_APRS(uint8_t *message, uint16_t msg_len)
{

	uint8_t tnc_msg[400];
	uint16_t current_index = 0;


	//Header bytes

	/*
	 * SSID format: CRRSSID0
	 *
	 * TNC Header Format:
	 * 0: 0xC0: Delimiting Character
	 * 1: 0x00: remainder of frame is HDLC DATA
	 * 			DESTINATION ADDRESS: "W5UL  1"
	 * 2: 0xAE: 'W' << 1 (0x57)
	 * 3: 0x6A (j): '5' << 1 (0x35)
	 * 4: 0xAA: 'U' << 1 (0x55)
	 * 5: 0x98: 'L' << 1 (0x4C)
	 * 6: 0x40: ' ' << 1 (0x20)
	 * 7: 0x40: ' '		 (0x20)
	 * 8: 0x62 (b):  0110 0010 (response, res, res, 0001)
	 * 			SOURCE ADDRESS: "W5UL  0"
	 * 9: 0xAE: 'W' << 1
	 * 10: 0x6A: '5' << 1
	 * 11: 0xAA: 'U' << 1
	 * 12: 0x98: 'L' << 1
	 * 13: 0x40: ' ' << 1
	 * 14: 0x40: ' ' << 1
	 * 15: 0xF7:	1111 0111 (command, res, res, 1011)
	 * 16: 0x03:	Control field (Unnumbered information frame)
	 * 17: 0xF0:	PID field (no layer 3 protocol implemented)
	 *
	 *
	 *
	 *
	 */



	uint8_t tnc_header[25] = {0xC0, 0x00, 0x96, 0x8C, 0x6A, 0x84, 0x9C, 0x98, 0xEE, 0xAE, 0x6A, 0xAA, 0x98, 0x40, 0x40, 0x76, 0xAE, 0x92, 0x88, 0x8A, 0x64, 0x40, 0x65, 0x03, 0xF0};
	uint8_t end_char[1] = {0xC0};

	//	TerminalWrite("Sending test to TNC:"); NL();
	//	TerminalWrite(tnc_header); NL();

//	if (current_ax->ax_fm_mode != 't')
//	{
//		AX_FM_TX();
//
//		//PulseWDT();
//
//		HAL_Delay(500);
//		//			HAL_Delay(10);
//	}
//	else
//	{
//		TerminalWrite("AX5043 already in FM TX mode!"); NL();
//	}


	//PulseWDT();

	HAL_Delay(1);

//	int16_t urb_v_trans = getBusVoltage_mV(URB_INA_ADD);
//	HAL_Delay(1);
//	int16_t urb_i_trans = getCurrent_mA(URB_INA_ADD);
//	HAL_Delay(1);

	//PulseWDT();

//	if (message[22] == 'B')
//	{
//		//		packet_length = 201;
//
//		memcpy(message + BEACON_PKT_URB_V_TRANS, Split16Bit((uint16_t)urb_v_trans), 2);
//
//		memcpy(message + BEACON_PKT_URB_I_TRANS, Split16Bit((uint16_t)urb_i_trans), 2);
//
//		//		TerminalWrite("URB Transmit: ");
//		//		TerminalNum(urb_v_trans); TerminalWrite(" mV, ");
//		//		TerminalNum(urb_i_trans); TerminalWrite(" mA."); NL();
//
//
//	}



//	NL();
	// perform escaping
	for (uint16_t i = 0; i < msg_len; i ++)
	{
		//		TerminalHex(message[i]);
		if (message[i] == 0xC0)
		{

			//		TerminalHex(message[i]);
			//		TerminalNum(i);
			//		TerminalWrite(" Escaping FEND...   ");
			tnc_msg[current_index] = 0xDB;
			current_index ++;
			tnc_msg[current_index] = 0xDC;
		}
		else if (message[i] == 0xDB)
		{
			//		TerminalHex(message[i]);
			//		TerminalNum(i);
			//		TerminalWrite("Escaping FESC...   ");
			tnc_msg[current_index] = 0xDB;
			current_index ++;
			tnc_msg[current_index] = 0xDD;
		}
		else
		{
			tnc_msg[current_index] = message[i];

		}
		current_index ++;
	}

//	NL(); NL();
//	TerminalWrite("Length: "); TerminalNum(current_index); NL();




	//	     HAL_UART_Transmit(&huart2, startHeader, 2, 50);
	//
	//	     HAL_UART_Transmit(&huart2, destination, 7, 50);
	//
	//	     HAL_UART_Transmit(&huart2, callsign, 7, 50);
	//
	//	     HAL_UART_Transmit(&huart2, digipeat, 7, 50);
	//
	//	     HAL_UART_Transmit(&huart2, endHeader, 2, 50);

	HAL_UART_Transmit(&huart1, tnc_header, 25, 50);

	HAL_UART_Transmit(&huart1, tnc_msg, current_index, 500);

	HAL_UART_Transmit(&huart1, end_char, 1, 50);

//	do
//	{
//
//		HAL_Delay(1);
//	}
//
//	while (!(HAL_GPIO_ReadPin(TNC_PTT_GPIO_Port, TNC_PTT_Pin)));
//
//	do
//	{
//
//		//PulseWDT();
//		HAL_Delay(1);
//	}
//
//	while ((HAL_GPIO_ReadPin(TNC_PTT_GPIO_Port, TNC_PTT_Pin)));


//	HAL_Delay(500);

	//AX_FM_RX();

//	AX_FM_EXIT();

//	TerminalWrite("TNC Transmission Complete."); NL();


//	RADIO_FE_LP
//	RADIO_VHF_RX
//	RADIO_UHF_RX








	return;


}
