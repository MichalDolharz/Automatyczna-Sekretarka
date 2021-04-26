/**
 ******************************************************************************
 * @file    wav_recorder.h
 * @author  Michał Dołharz
 * @brief   Header for wav_recorder.c module.
 ******************************************************************************/

#ifndef __WAV_RECORDER_H
#define __WAV_RECORDER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "main.h"
#include "usb_host.h"
#include "fatfs.h"

#define MAX_RECORDING_TIME 30000 // maksymalny czas nagrania (ms)
#define RECBUFFER_SIZE 4096 // rozmiar bufora dla half-word

#define MAX_RECORDS 10 // maksymalna liczba nagran

#define AUDIO_IN_FREQUENCY DEFAULT_AUDIO_IN_FREQ // czestotliwosc audio
// to sie znajduje w pliku stm32f4xx_hal_i2s, w folderze drivers/stm32..driver/inc.

#define NUMBER_OF_CHANNELS 2 // Liczba kanalow
#define AUDIO_RESOLUTION 16  // Rozdzielczosc, liczba bitow na probke

#define STATUS_RECORDING_INACTIVE 0 // nagrywanie nieaktywne
#define STATUS_RECORDING_ACTIVE 1 // nagrywanie aktywne

/*! @brief Starts recording
 * @param buff pointer to a buffer where data will be writen into
 * @param dataSize buffer size
 * @retval None
 */
uint8_t StartRecording(uint16_t *buff, uint32_t dataSize);

/*! @brief Stops recording
 * @param None
 * @retval None
 */
uint32_t StopRecording(void);

/*! @brief Processes recorded data
 * @param None
 * @retval None
 */
void WavRecordingProcess(uint8_t recordNumber);

#endif
