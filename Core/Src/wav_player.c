#include "CS43L22_config.h"
#include "wav_player.h"
#include "stm32f4xx_hal.h"
#include "fatfs.h"

static FIL wavFile;

static uint32_t fileLength;
#define AUDIO_BUFFER_SIZE  4096
static uint8_t audioBuffer[AUDIO_BUFFER_SIZE];
static __IO uint32_t audioRemainSize = 0;

static UINT playerReadBytes = 0;
static bool isFinished = 0;

//stany transmisji
typedef enum {
	PLAYER_CONTROL_Idle = 0,
	PLAYER_CONTROL_HalfBuffer,
	PLAYER_CONTROL_FullBuffer,
	PLAYER_CONTROL_EndOfFile,
} PLAYER_CONTROL_e;
static volatile PLAYER_CONTROL_e playerControlSM = PLAYER_CONTROL_Idle;

static I2S_HandleTypeDef *hAudioI2S;

//ustawienie uchwytu i2s
void audioI2S_setHandle(I2S_HandleTypeDef *pI2Shandle) {
	hAudioI2S = pI2Shandle;
}

//przerwanie po zakończeniu transmisji
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
	if (hi2s->Instance == SPI3) {
		playerControlSM = PLAYER_CONTROL_FullBuffer;
	}
}

//przerwanie w połowie transmisji bufora
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
	if (hi2s->Instance == SPI3) {
		playerControlSM = PLAYER_CONTROL_HalfBuffer;
	}
}

//reset
static void wavPlayer_reset(void) {
	audioRemainSize = 0;
	playerReadBytes = 0;
}

//wybranie pliku do odtworzenia
bool wavPlayer_fileSelect(const char *filePath) {
	WAV_HeaderTypeDef wavHeader;
	UINT readBytes = 0;
	if (f_open(&wavFile, filePath, FA_READ) != FR_OK) {
		return false;
	}
	f_read(&wavFile, &wavHeader, sizeof(wavHeader), &readBytes);
	fileLength = wavHeader.FileSize;
	return true;
}

//start odtwarzania
void wavPlayer_play(void) {
	isFinished = false;
	f_lseek(&wavFile, 0);
	f_read(&wavFile, &audioBuffer[0], AUDIO_BUFFER_SIZE, &playerReadBytes);
	audioRemainSize = fileLength - playerReadBytes;
	CS43L22_Start();
	HAL_I2S_Transmit_DMA(hAudioI2S,(uint16_t*) &audioBuffer[0], DMA_MAX( AUDIO_BUFFER_SIZE/AUDIODATA_SIZE));
}

//odczyt danych
void wavPlayer_process(void) {
	switch (playerControlSM) {
	case PLAYER_CONTROL_Idle:
		break;

	case PLAYER_CONTROL_HalfBuffer:
		playerReadBytes = 0;
		playerControlSM = PLAYER_CONTROL_Idle;
		f_read(&wavFile, &audioBuffer[0], AUDIO_BUFFER_SIZE / 2,
				&playerReadBytes);
		if (audioRemainSize > (AUDIO_BUFFER_SIZE / 2)) {
			audioRemainSize -= playerReadBytes;
		} else {
			audioRemainSize = 0;
			playerControlSM = PLAYER_CONTROL_EndOfFile;
		}
		break;

	case PLAYER_CONTROL_FullBuffer:
		playerReadBytes = 0;
		playerControlSM = PLAYER_CONTROL_Idle;
		f_read(&wavFile, &audioBuffer[AUDIO_BUFFER_SIZE / 2],
				AUDIO_BUFFER_SIZE / 2, &playerReadBytes);
		if (audioRemainSize > (AUDIO_BUFFER_SIZE / 2)) {
			audioRemainSize -= playerReadBytes;
		} else {
			audioRemainSize = 0;
			playerControlSM = PLAYER_CONTROL_EndOfFile;
		}
		break;

	case PLAYER_CONTROL_EndOfFile:
		f_close(&wavFile);
		wavPlayer_reset();
		isFinished = true;
		playerControlSM = PLAYER_CONTROL_Idle;
		break;
	}
}

//zatrzymanie odtwarzania
void wavPlayer_stop(void) {
	CS43L22_Stop();
	HAL_I2S_DMAStop(hAudioI2S);
	isFinished = true;
}

//pauza
void wavPlayer_pause(void) {
	CS43L22_Stop();
	HAL_I2S_DMAPause(hAudioI2S);
}

//wznowienie odtwarzania
void wavPlayer_resume(void) {
	CS43L22_Start();
	HAL_I2S_DMAResume(hAudioI2S);
}

//czy zakończono odczyt pliku
bool wavPlayer_isFinished(void) {
	return isFinished;
}

