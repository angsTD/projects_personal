/*
 * testheader.h
 *
 *  Created on: Apr 21, 2016
 *      Author: Angelo
 */

#ifndef TESTHEADER_H_
#define TESTHEADER_H_
#include "stdint.h"
#define SVCALL(number, return_type, signature) return_type signature


SVCALL(SD_BLE_GAP_ADDRESS_GET, uint32_t, sd_ble_gap_address_get(uint32_t * const p_addr));

#endif /* TESTHEADER_H_ */
