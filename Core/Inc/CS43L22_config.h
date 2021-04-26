
#include "stm32f4xx_hal.h"
#include "stdbool.h"

#define POWER_CONTROL1					0x02
#define POWER_CONTROL2					0x04
#define INTERFACE_CONTROL1			0x06
#define MISCELLANEOUS_CONTRLS		0x0E
#define   CS43L22_REG_MASTER_A_VOL        0x20
#define   CS43L22_REG_MASTER_B_VOL        0x21
#define   CS43L22_REG_HEADPHONE_A_VOL     0x22
#define   CS43L22_REG_HEADPHONE_B_VOL     0x23
#define DAC_I2C_ADDR 			0x94
#define VOLUME_MASTER(volume)       (((volume) >= 231 && (volume) <=255)? (volume-231) : (volume+25))

void CS43L22_Init(I2C_HandleTypeDef i2c_handle);
void CS43L22_SetVolume(uint8_t volume);
void CS43L22_SetMute(bool mute);
void CS43L22_Start(void);
void CS43L22_Stop(void);
