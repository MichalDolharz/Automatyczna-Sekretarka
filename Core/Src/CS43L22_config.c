#include "CS43L22_config.h"

static I2C_HandleTypeDef i2cx;

//zapis wartości do rejestru
static void CS43l22_write(uint8_t reg, uint8_t value)
{
	HAL_I2C_Mem_Write(&i2cx, DAC_I2C_ADDR, reg, 1, &value, sizeof(value), HAL_MAX_DELAY);
}

//Inicjalizacja
void CS43L22_Init(I2C_HandleTypeDef i2c_handle) {
	//Pin Audio_Reset
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);
	//Przekazanie uchwytu i2c
	i2cx = i2c_handle;
	//Power down
	CS43l22_write(POWER_CONTROL1, 0x01);
	//obie słuchawki włączone, oba głośniki wyłączone
	CS43l22_write(POWER_CONTROL2, 0xAF);
	//slave, I2s oraz 16bit
	CS43l22_write(INTERFACE_CONTROL1,0x07);
}

// Ustawienia głośności 0-255
void CS43L22_SetVolume(uint8_t volume) {
	volume = VOLUME_MASTER(volume);
	CS43l22_write(CS43L22_REG_MASTER_A_VOL, volume);
	CS43l22_write(CS43L22_REG_MASTER_B_VOL, volume);
}

//wyciszanie/odciszanie dźwięku
void CS43L22_SetMute(bool mute) {
	//mutowanie
	if (mute) {
		//wyłączone głośniki i słuchawki
		CS43l22_write(POWER_CONTROL2,0xFF);
		//wyciszanie obu słuchawek
		CS43l22_write(CS43L22_REG_HEADPHONE_A_VOL,0x01);
		CS43l22_write(CS43L22_REG_HEADPHONE_B_VOL,0x01);
	} else { //odmutowanie
		//głośność na 0dB w obu słuchawkach
		CS43l22_write(CS43L22_REG_HEADPHONE_A_VOL,0x00);
		CS43l22_write(CS43L22_REG_HEADPHONE_B_VOL,0x00);
		//obie słuchawki włączone, głośniki wyłączone
		CS43l22_write(POWER_CONTROL2,0xAF);

	}
}

//Start układu DAC
void CS43L22_Start(void) {
	//odmutowanie
	CS43L22_SetMute(0);
	//Power up
	CS43l22_write(POWER_CONTROL1,0x9E);
}

//zatrzymanie układu
void CS43L22_Stop(void) {
	//wyciszenie
	CS43L22_SetMute(1);
	//filtr deemfazy nakłada - pozbywa się szumu po zatrzymaniu
	CS43l22_write(MISCELLANEOUS_CONTRLS,0x04);
	//Power down
	CS43l22_write(POWER_CONTROL1,0x9F);
}
