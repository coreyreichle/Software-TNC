/*
 * AX.25.h
 *
 *  Created on: Sep 2, 2020
 *      Author: Kobe
 *
 *      This file is for formatting and detecting AX.25 Packets
 */

#ifndef SRC_AX_25_H_
#define SRC_AX_25_H_
#include "main.h"
#include <stdio.h>
#include <stdbool.h>
#include "FreqIO.h"


//*************** AX.25 Fields******************************************************************************************
#define FLAG_END_COUNT	1
#define FLAG_SIZE		8
#define address_len		112 //min number of bits in address field/224 bits is the max for digipeting
#define control_len		8 //MIN number of bits in control field/MAX is 16 bits supposedly
#define PID_len			8 //number of bits in PID field (subfield for info frames)
#define MAX_INFO		2048 //Max number of bits in information field(sub field for info frames)
#define FCS_len			16 //number of bits in FCS frames
#define MAX_Stuffed     463 //possible max number of bit stuffed zeros
#define bool_size		sizeof(bool) //size of a boolean data type, in case you didn't know

//*************** variables for detecting and validating  AX.25  ******************************************************
#define AX25_PACKET_MAX			address_len + control_len + PID_len + MAX_INFO +FCS_len	+ MAX_Stuffed		//max bits in a packet, not including flags
#define INFO_offset_woFlag		144		//Subtract from packet bit count w/o flags and FCS to get number of info bits
#define INFO_offset_wFlag_woFCS	152		//Subtract from packet bit count with flags and port info to get number of info bits


extern int rxBit_count; 							//keeps count of the temp buffer index
extern bool AX25TBYTE[FLAG_SIZE];							//Array to store AX.25 terminate flag in binary
extern bool local_address[address_len/2];	//address set to this TNC
//*********************************************************************************************************************

//**************** KISS *************************************************************************************************************
#define KISS_SIZE		FLAG_SIZE + address_len + control_len + PID_len + MAX_INFO + FLAG_SIZE //size of kiss packet
#define KISS_SIZE_BYTES	(int) (KISS_SIZE/8)
#define UART_RX_IT_CNT 1

bool KISS_FLAG[FLAG_SIZE];
//*************************************************************************************************************************************

struct UART_INPUT {
	int rx_cnt;
	int received_byte_cnt;
	int flags;
	uint8_t input;
	bool got_packet;

	//HEX Members, includes frame end flags
	uint8_t HEX_KISS_PACKET[KISS_SIZE_BYTES];//This is the buffer used to hold hex bits from UART
}UART_packet;

struct PACKET_STRUCT {
	//AX.25 Members, does not include frame end flags
	bool AX25_PACKET[AX25_PACKET_MAX];//temporary stores bits received from radio, before formatting into AX.25 format

	//KISS Members, includes frame end flags
	bool KISS_PACKET[KISS_SIZE];

	/*
	 * 	Packet Pointers:
	 * 	Can reference data in either AX.25 packet or KISS packet
	 */
	bool *address;			//Pointer to address field in global buffer
	bool *control;			//Pointer to control field in global buffer
	bool *PID; 				//Pointer to PID field in global buffer, only present for I frames
	bool *Info;				//Pointer to info field in global buffer
	int  Info_Len;			//Length of info field, in bits
	bool *FCS;				//Pointer to fcs field in global buffer
	bool i_frame_packet;	//Flag to signal if packet is of type i-frames

	bool got_packet;
	int byte_cnt;


	//count for bit stuffed zeros in each field
	int stuffed_address;
	int stuffed_control;
	int stuffed_PID;
	int stuffed_Info;
	int stuffed_FCS;

	int bit_stuffed_zeros;

	//CRC
	uint16_t crc; 				//crc value after calculating data from PC
	int crc_count;
	bool check_crc;			//indicates weather validating fcs field or creating fcs field
}global_packet;

//Conversion Functions
/*
 * 	Converts hex to bin
 * 		hex_byte_in 	- input hex value
 * 		bin_byte_out 	- output binary pointer to write to. Should point to
 */
void conv_HEX_to_BIN(uint16_t hex_byte_in, bool *bin_byte_out,bool select_8_16);
/*
 * 	Converts bin to hex
 * 		bin_byte_in		- input binary pointer to a byte
 *
 * 		returns the byte in decimal HEX value
 * 		00 = 0
 * 		C0 = 192
 * 		FF = 255
 */
uint16_t conv_BIN_to_HEX(bool *bin_byte_in,bool select_8_16);

//General Program
//****************************************************************************************************************
void init_AX25();

/*
 * 	Function ran in main
 */
void tx_rx();

void output_AX25();
void clear_AX25();

void output_HEX();
void clear_KISS();

//AX.25 to KISS data flow
//****************************************************************************************************************

bool receiving_AX25();
void slide_bits(bool* array,int bits_left); //discards bit stuffed 0 and slide remaining bits over bits over
void remove_bit_stuffing(); //remove bit stuffing zeros after every 5 consecutive 1's

/*
 * 	Function that iterates through the AX.25_temp_buffer found in global packet to determine if
 * 	data in the buffer is a valid packet structure
 * 		returns true if the packet is valid
 * 		returns false if the packet is invalid in any way
 */
bool AX25_Packet_Validate();
/*
 * 	reverse elements in array, called by set_packet_pointer_AX_25, since all AX.25 fields were sent LSB first except FCS field
 */
void reverse_array(bool *array,int size);
void set_packet_pointer_AX25();
void AX25_TO_KISS();

//****************************************************************************************************************
//END OF AX.25 to KISS data flow

//KISS to AX.25 data flow
//****************************************************************************************************************

bool receiving_KISS();
void set_packet_pointer_KISS();
bool KISS_TO_AX25();
void bit_stuff_fields();
/*
 *	Helper function for shifting bits up when a bit stuffed zero is added
 */
void bit_shift(bool* array,int bits_left);

/*
 *	Add bitstuffed zeros to AX25 packet before transmitting it through radio
 */
int bitstuffing(bool* packet,int len,int bits_left, int ones_count,int *stuff);

//****************************************************************************************************************
//END OF KISS to AX.25 data flow

//---------------------- FCS Generation -----------------------------------------------------------------------------------------------

/*
 * 	Helper function to generate CRC. Simply computes CRC from one bit
 */
void crc_calc(int in_bit, int * crc_ptr_in, int * crc_count_ptr_in, bool tx_rx);

/*
 *	Generates CRC from KISS Packet ot AX.25 packet.
 *	If given a true input, will generate from AX.25 Data Fields
 *	If given a false input, will generate from Kiss Data Fields
 */
void crc_generate(bool tx_rx);

/*
 * 	Helper function to validate the fcs field of received packet from radio.
 * 	generates a crc value based on the the subfields besides the FCS and compares with FCS field
 * 	returns true or false based on if the generated crc matches the FCS or not respectively.
 */
bool crc_check();

#endif /* SRC_AX_25_H_ */
