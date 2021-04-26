
#ifndef WAV_PLAYER_H_
#define WAV_PLAYER_H_

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"

typedef enum
{
  BUFFER_OFFSET_NONE = 0,
  BUFFER_OFFSET_HALF,
  BUFFER_OFFSET_FULL,
}BUFFER_StateTypeDef;

typedef enum
{
  PLAY_Idle=0,
  PLAY_Pause,
  PLAY_Resume,
}PLAY_State_e;

typedef struct
{
  uint32_t   ChunkID;       /* 0 */
  uint32_t   FileSize;      /* 4 */
  uint32_t   FileFormat;    /* 8 */
  uint32_t   SubChunk1ID;   /* 12 */
  uint32_t   SubChunk1Size; /* 16*/
  uint16_t   AudioFormat;   /* 20 */
  uint16_t   NbrChannels;   /* 22 */
  uint32_t   SampleRate;    /* 24 */

  uint32_t   ByteRate;      /* 28 */
  uint16_t   BlockAlign;    /* 32 */
  uint16_t   BitPerSample;  /* 34 */
  uint32_t   SubChunk2ID;   /* 36 */
  uint32_t   SubChunk2Size; /* 40 */

}WAV_HeaderTypeDef;


#define DMA_MAX_SZE                 0xFFFF
#define DMA_MAX(_X_)                (((_X_) <= DMA_MAX_SZE)? (_X_):DMA_MAX_SZE)
#define AUDIODATA_SIZE              2   //16 bitów

void audioI2S_setHandle(I2S_HandleTypeDef *pI2Shandle);
void audioI2S_halfTransfer_Callback(void);
void audioI2S_fullTransfer_Callback(void);
bool wavPlayer_fileSelect(const char* filePath);
void wavPlayer_play(void);
void wavPlayer_stop(void);
void wavPlayer_process(void);
bool wavPlayer_isFinished(void);
void wavPlayer_pause(void);
void wavPlayer_resume(void);

#endif /* WAV_PLAYER_H_ */
