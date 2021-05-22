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
static uint32_t samplingFreq;

//stany transmisji
typedef enum {
	PLAYER_CONTROL_Idle = 0,
	PLAYER_CONTROL_HalfBuffer,
	PLAYER_CONTROL_FullBuffer,
	PLAYER_CONTROL_EndOfFile,
} PLAYER_CONTROL_e;
static volatile PLAYER_CONTROL_e playerControlSM = PLAYER_CONTROL_Idle;

static I2S_HandleTypeDef *hAudioI2S;

extern const uint32_t I2SFreq[8];
extern const uint32_t I2SPLLN[8];
extern const uint32_t I2SPLLR[8];

//Aktualizacja konfiguracji zegarów w zależności od częstotliwości pliku dźwiękowego
static void audioI2S_pllClockConfig(uint32_t audioFreq)
{
  RCC_PeriphCLKInitTypeDef rccclkinit;
  uint8_t index = 0, freqindex = 0xFF;

  for(index = 0; index < 8; index++)
  {
    if(I2SFreq[index] == audioFreq)
    {
      freqindex = index;
    }
  }

  HAL_RCCEx_GetPeriphCLKConfig(&rccclkinit);

  if ((freqindex & 0x7) == 0)
  {
    rccclkinit.PeriphClockSelection = RCC_PERIPHCLK_I2S;
    rccclkinit.PLLI2S.PLLI2SN = I2SPLLN[freqindex];
    rccclkinit.PLLI2S.PLLI2SR = I2SPLLR[freqindex];
    HAL_RCCEx_PeriphCLKConfig(&rccclkinit);
  }
  else
  {
    rccclkinit.PeriphClockSelection = RCC_PERIPHCLK_I2S;
    rccclkinit.PLLI2S.PLLI2SN = 258;
    rccclkinit.PLLI2S.PLLI2SR = 3;
    HAL_RCCEx_PeriphCLKConfig(&rccclkinit);
  }
}

//aktualizacja parametru częstotliwości w I2S3
static bool I2S3_freqUpdate(uint32_t AudioFreq)
{
  hAudioI2S->Instance         = SPI3;

  __HAL_I2S_DISABLE(hAudioI2S);

  hAudioI2S->Init.AudioFreq   = AudioFreq;
  hAudioI2S->Init.ClockSource = I2S_CLOCK_PLL;
  hAudioI2S->Init.CPOL        = I2S_CPOL_LOW;
  hAudioI2S->Init.DataFormat  = I2S_DATAFORMAT_16B;
  hAudioI2S->Init.MCLKOutput  = I2S_MCLKOUTPUT_ENABLE;
  hAudioI2S->Init.Mode        = I2S_MODE_MASTER_TX;
  hAudioI2S->Init.Standard    = I2S_STANDARD_PHILIPS;

  if(HAL_I2S_Init(hAudioI2S) != HAL_OK)
  {
    return false;
  }
  else
  {
    return true;
  }
}

//inicjalizacja I2S w zależności od częstotliwośći pliku dźwiękowego
bool audioI2S_init(uint32_t audioFreq)
{
  audioI2S_pllClockConfig(audioFreq);
  I2S3_freqUpdate(audioFreq);
  return true;
}


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
	samplingFreq = wavHeader.SampleRate;
	return true;
}

//start odtwarzania
void wavPlayer_play(void) {
	isFinished = false;
	audioI2S_init(samplingFreq);
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
		wavPlayer_stop();
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

