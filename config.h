/*
 * config.h
 *
 *  Created on: 29 gru 2016
 *      Author: Luke
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define VESC_1_ID 1
#define VESC_2_ID 2
#define DUC_ID 3
#define CU_ID 4

#define BM_BATTERY_CELLS_COUNT 6
#define BM_MIN_BATTERY_CELL_VOLTAGE 3.0
#define BM_MAX_BATTERY_CELL_VOLTAGE 4.3
#define BM_MIN_BATTERY_VOLTAGE (BM_BATTERY_CELLS_COUNT * BM_MIN_BATTERY_CELL_VOLTAGE)

#define AM_ADC_MAX_VALUE 4095
#define AM_ADC_REF_VOLTAGE 3.28
#define AM_CHANNELS_COUNT 7
#define AM_SAMPLES_COUNT 1

#define TM_MOSFET_TEMP_CHANNEL 6

#define SM_PACKET_UPDATE_PERIOD 200

#endif /* CONFIG_H_ */
