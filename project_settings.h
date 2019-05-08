/*
 * project_settings.h
 *
 *  Created on: Mar 1, 2019
 *      Author: sglas
 */

#ifndef PROJECT_SETTINGS_H_
#define PROJECT_SETTINGS_H_

#define FCPU 24000000 //16000000

// include the library header
#include "library.h"

#define USE_MODULE_TASK
#define USE_MODULE_TIMING
#define USE_MODULE_LIST
#define USE_MODULE_BUFFER
#define USE_MODULE_BUFFER_PRINTF
#define USE_MODULE_UART
#define USE_MODULE_SUBSYSTEM


#define USE_UART1
#define SUBSYSTEM_IO SUBSYSTEM_IO_UART
#define SUBSYSTEM_UART 1
#define SUBSYS_UART 1

#define UART1_TX_BUFFER_LENGTH 512
#define TASK_MAX_LENGTH 50

#define USE_I2C0
#define I2C_MAX_TX_SIZE 2
#define I2C_MAX_RX_SIZE 6

#define USE_SPI_B1
#define SPI_MAX_SIZE 33
#define THIEF_SPI SPI_B1
#define THIEF_BRAINFRAME_NETWORK

#define THIS_NODE SCRUM_PUN

#endif /* PROJECT_SETTINGS_H_ */
