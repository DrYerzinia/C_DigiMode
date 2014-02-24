/**
 * @file	APRSPacket.h
 * @author	Michael Marques <dryerzinia@gmail.com>
 * @brief	Contains functions for parsing APRS data from packets and
 * 			structs for storing that data
 * @ingroup	Packet
 *
 * File contains all the necessary functions for parsing APRS packets.
 * Contains structs for storing the APRS related data, and functions
 * for accessing it and converting it to string form for printing.
 */

#ifndef APRSPACKET_H
#define APRSPACKET_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../crc/crcccitt.h"

#include "../container/char_array.h"

/**
 * Value of CRC Polynomial used in AX-25
 */
#define APRS_CRC_POLY 0x8408

/**
 * @ingroup Packet
 * Contains all information related to the APRS packet include the raw data,
 * the AX-25 Addresses, and any parsed information based on standard APRS
 * messages
 */
typedef struct {

	/**
	 * Raw data of APRS packet including CRC
	 */
	char_array	data;

	/**
	 * Destination address of packet with SSID
	 * May contain Mic-E data
	 */
	char		destination_address[8];
	/**
	 * Source of the packet with SSID
	 */
	char		source_address[8];

	/**
	 * Number of repeaters
	 * A maximum of 8 repeaters is allowed
	 */
	uint8_t		repeaters;
	/**
	 * List of repeaters
	 */
	char		**repeater_addresses;

} APRSPacket;

/**
 * @ingroup Packet
 * Creates an APRSPacket strut from a char array of packet data parsing any
 * relevant information for easier access later.
 */
void APRSPacket_from_data(APRSPacket *packet, char_array *data);

/**
 * @ingroup Packet
 * Calculates CRC of packet data using the APRS_CRC_POLY
 * @returns CRC value of packet data
 */
uint16_t APRSPacket_crc(APRSPacket *packet);

/**
 * @ingroup Packet
 * Frees any memory in use by this packet
 */
void APRSPacket_destroy(APRSPacket *packet);

#endif
