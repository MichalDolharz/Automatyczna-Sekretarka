/**
 ******************************************************************************
 * @file    wav_recorder.c
 * @author  Michał Dołharz
 * @brief   Wav recorder
 ******************************************************************************/

#include "main.h"
#include "wav_recorder.h"
#include "fatfs.h"
#include "string.h"
#include "stm32f411e_discovery_audio.h"

// Zmienne
uint8_t recStatus = 0; // flaga nagrywania
uint8_t header[44]; // naglowek pliku wav, wymaga 44 znakow
uint16_t recBuffer[RECBUFFER_SIZE]; // bufor nagrywania
uint32_t dataSize; // zmienna do zapisaywania ilosci danych (rozmiaru pliku)
FIL WavFile; // Plik, domyslnie w formacie .wav
static uint16_t internalBuffer[INTERNAL_BUFF_SIZE]; // w tym buforze zapisywane sa biezace nagrania
static uint16_t PCMOutBuffer[PCM_OUT_SIZE * 2]; // w tym buforze zapisywane sa nagrania w modulacji PCM
__IO uint8_t dataReady = 0; // flaga gotowosci danych do zapisu do pliku
__IO uint32_t bufferOffset = 0; // offset buffora nagrywania
__IO uint32_t counter = 0; // licznik danych
__IO FRESULT fresult; // zmienna do kontroli niektorych funkcji obslugi USB

// Zewnetrzne zmienne
extern __IO uint32_t recordingTime; // biezacy czas nagrywania (odliczanie w stm32f4xx_it.h)
extern __IO uint32_t recordingStatus; // status nagrywania
extern char USBHPath[4]; // zmienna do obslugi USB
//extern UART_HandleTypeDef huart2; // UART do ewentualnego bardziej zaawansowanego debugowania niz Info_UART()
//extern ApplicationTypeDef Appli_state; // stan pamieci USB

// Deklaracje funkcji statycznych
static void WavFileHeaderInit(uint8_t *header);
static void WavFileHeaderUpdate(uint8_t *header);

// Definicje funkcji

// Poczatek nagrywania
uint8_t StartRecording(uint16_t *buff, uint32_t dataSize) {
	return (BSP_AUDIO_IN_Record(buff, dataSize));
}
// Koniec nagrywania
uint32_t StopRecording(void) {
	return BSP_AUDIO_IN_Stop();
}

// Nagrywanie
void WavRecordingProcess(uint8_t recordNumber) {

	// Rozmiar nagranych danych w bajtach
	uint32_t bytesWritten = 0;

	// Zamontowanie zew. pamieci USB
	if (f_mount(&USBHFatFS, USBHPath, 1) != FR_OK) {
		Info_UART("NIE ZAMONTOWANO POPRAWNIE USB (main.c)\r\n");
	} else {
		Info_UART("Zamontowano USB\r\n");
	}

	// Ustalenie nazwy pliku
	char filename[] = "RECn.wav";
	sprintf(filename, "REC%d.wav", recordNumber);
	Info_UART("Nazwa pliku: ");
	Info_UART(filename);
	Info_UART("\r\n");

	// Jezeli plik istnieje, zostanie nadpisany
	f_unlink(filename);

	// Plik jest otwierany/tworzony
	if ((f_open(&WavFile, filename, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)) {
		Info_UART("NIE MOGE OTWORZYC PLIKU (wav_recorder.c)\r\n");
	} else {
		Info_UART("Otworzono plik (wav_recorder.c)\r\n");
	}

	// Stworzenie wstepnego naglowka pliku .wav
	// Potem zostanie zaktualizowany o informacje o nagraniu
	WavFileHeaderInit(header);

	// Wpisanie naglowka do pliku
	if ((f_write(&WavFile, header, 44, (void*) &bytesWritten) != FR_OK)) {
		Info_UART("PROBLEM Z WPISANIE NAGLOWKA DO PLIKU (wav_recorder.c)\r\n");
	}

	// Pierwszy zapis rozmiaru danych
	dataSize = bytesWritten;

	// Rozpoczecie nagrywania
	if (BSP_AUDIO_IN_Record((uint16_t*) &internalBuffer[0],
	INTERNAL_BUFF_SIZE) != AUDIO_OK)
		Info_UART("NIE UDALO SIE ROZPOCZECIA NAGRYWANIA (wav_recorder.c)\r\n");
	else
		Info_UART("Rozpoczeto nagrywanie \r\n");

	// Reset czasu trwania nagrania.
	// Samo odliczanie ma miejsce w pliku stm32f4xx_it.c
	recordingTime = 0;

	// licznik danych
	counter = 0;

	while (1) {
		// Jezeli czas nagrywania nie zostal przekroczony
		if (recordingTime <= MAX_RECORDING_TIME) {

			// Sprawdza, czy sa dostepne dane do wprowadzenia do pliku .wav
			if (dataReady) {
				// Wyzerowanie flagi
				dataReady = 0;

				// Wpisanie danych do pliku
				if (f_write(&WavFile, (uint8_t*) (recBuffer + bufferOffset),
				RECBUFFER_SIZE, (void*) &bytesWritten) != FR_OK) {
					Info_UART(
							"BLAD PRZY WPROWADZANIU DANYCH DO PLIKU (wav_recorder.c)\r\n");
				}

				// Kolejny zapis do rozmiaru danych
				dataSize += bytesWritten;
			}

			// Jezeli przycisk zostal wcisniety, to zmienil sie status nagrywania
			if (recordingStatus == STATUS_RECORDING_INACTIVE) {
				StopRecording();
				Info_UART("Koniec nagrywania - przycisk\r\n");
				break;
			}
		}
		// Jezeli przekroczono czas nagrywania
		else {
			StopRecording();
			dataReady = 0;
			recordingStatus = STATUS_RECORDING_INACTIVE; // zmiana statusu
			Info_UART("Koniec nagrywania - timeout\r\n");
			break;
		}
	}

	// Po zakonczonym nagrywaniu trzeba zaktualizowac naglowek

	// Przesuwa wskaznik odczytu odczytu/zapisu pliku na poczatek (dla zachowania sposobu zapisu naglowka pliku)
	f_lseek(&WavFile, 0);

	// Aktualizuje naglowek o nowe informacje o nagraniu
	WavFileHeaderUpdate(header);

	// Wpisanie zaktualizowanego naglowka do pliku
	if (f_write(&WavFile, header, 44, (void*) bytesWritten) != FR_OK)
		Info_UART(
				"NIE UDALO SIE AKTUALIZOWANIE NAGLOWKA USB (wav_recorder.c)\r\n");
	else
		Info_UART("Naglowek zaktualizowany\r\n");

	// Zamkniecie pliku
	if (f_close(&WavFile) != FR_OK)
		Info_UART("NIE UDALO SIE ZAMKNIECIE PLIKU (wav_recorder.c)\r\n");
	else
		Info_UART("Plik zamkniety\r\n");

	// Odmontowanie pamieci USB
	if (f_mount(NULL, USBHPath, 1) != FR_OK)
		Info_UART("NIE UDALO SIE ODMONTOWANIE USB (wav_recorder.c)\r\n");
	else
		Info_UART("Odmontowano USB\r\n\r\n");
}

//static uint32_t WavFileHeaderInit(uint32_t *header)
static void WavFileHeaderInit(uint8_t *header) {
	// Format wav rozpoczyna sie od znakow "RIFF"
	header[0] = 'R';
	header[1] = 'I';
	header[2] = 'F';
	header[3] = 'F';

// Rozmiar pliku (z naglowkiem, ale bez 8 bajtow odpowiedzialnych za "RIFF" i fileSize)
// Zostanie nadpisany po nagraniu wiadomosci
	header[4] = 0x00;
	header[5] = 0x00;
	header[6] = 0x00;
	header[7] = 0x00;

	// Identyfikator pliku, "WAVE"
	header[8] = 'W';
	header[9] = 'A';
	header[10] = 'V';
	header[11] = 'E';

	// Format "chunk marker", ciag znakow "fmt " (ze spacja)
	header[12] = 'f';
	header[13] = 'm';
	header[14] = 't';
	header[15] = ' ';

	// Rozmiar formatu danych w bajtach
	header[16] = 0x10;
	header[17] = 0x00;
	header[18] = 0x00;
	header[19] = 0x00;

	// Typ formatu (1 - PCM)
	header[20] = 0x01;
	header[21] = 0x00;

	// Liczba kanalow
	header[22] = NUMBER_OF_CHANNELS;
	header[23] = 0x00;

	/* Czestotliwosc probkowania
	 * Czestotliwosc trzeba sparsowac na cztery bajty.
	 * Za odpowiednia kolejnosc odpowiada przesuniecie bitowe >>
	 * Za pobranie tylko jednego bajtu odpowiada maska 0xFF
	 * Tak sparsowane dane sa konwertowane do uint8_t*/
	uint32_t sampleRate = AUDIO_IN_FREQUENCY;
	header[24] = (uint8_t) ((sampleRate & 0xFF));
	header[25] = (uint8_t) ((sampleRate >> 8) & 0xFF);
	header[26] = (uint8_t) ((sampleRate >> 16) & 0xFF);
	header[27] = (uint8_t) ((sampleRate >> 24) & 0xFF);

	/* Liczba bajtow na sekunde
	 * Wzor:
	 * (Liczba_kanalow * Czestotliwosc_probkowania * rozdzielczosc) / 8
	 * Parsowanie bajtow analogiczne jak powyzej */
	uint32_t bytesPerSecond = (NUMBER_OF_CHANNELS * AUDIO_IN_FREQUENCY
			* AUDIO_RESOLUTION) / 8;
	header[28] = (uint8_t) ((bytesPerSecond & 0xFF));
	header[29] = (uint8_t) ((bytesPerSecond >> 8) & 0xFF);
	header[30] = (uint8_t) ((bytesPerSecond >> 16) & 0xFF);
	header[31] = (uint8_t) ((bytesPerSecond >> 24) & 0xFF);

	/* Liczba bajtow na probke
	 * Wzor:
	 * (Liczba_kanalow * rozdzielczosc) / 8 */
	uint16_t bytesPerSample = (NUMBER_OF_CHANNELS * AUDIO_RESOLUTION) / 8;
	header[32] = bytesPerSample;
	header[33] = 0x00; // mozna wpisac na stale

	// Liczba bitow na probke/kanal
	uint16_t bitsPerSample = AUDIO_RESOLUTION;
	header[34] = bitsPerSample;
	header[35] = 0x00; // mozna wpisac na stale

	// Informacja o poczatku danych, ciag znakow "data"
	header[36] = 'd';
	header[37] = 'a';
	header[38] = 't';
	header[39] = 'a';

	// Rozmiar danych w bajtach, iloczyn danych i czasu nagrania w sekundach.
	// Zostanie nadpisany po nagraniu wiadomosci
	header[40] = 0x00;
	header[41] = 0x00;
	header[42] = 0x00;
	header[43] = 0x00;

}

static void WavFileHeaderUpdate(uint8_t *header) {
// Rozmiar pliku (z naglowkiem, ale bez 8 bajtow odpowiedzialnych za "RIFF" i fileSize)
	header[4] = (uint8_t) ((dataSize & 0xFF));
	header[5] = (uint8_t) ((dataSize >> 8) & 0xFF);
	header[6] = (uint8_t) ((dataSize >> 16) & 0xFF);
	header[7] = (uint8_t) ((dataSize >> 24) & 0xFF);

	// Rozmiar danych w bajtach, iloczyn danych i czasu nagrania w sekundach.
	// (danych, a wiec bez naglowka)
	dataSize -= 44;
	header[40] = (uint8_t) ((dataSize & 0xFF));
	header[41] = (uint8_t) ((dataSize >> 8) & 0xFF);
	header[42] = (uint8_t) ((dataSize >> 16) & 0xFF);
	header[43] = (uint8_t) ((dataSize >> 24) & 0xFF);
}

void BSP_AUDIO_IN_TransferComplete_CallBack(void) {

	// Konwersja PDM do PCM
	BSP_AUDIO_IN_PDMToPCM((uint16_t*) &internalBuffer[INTERNAL_BUFF_SIZE / 2],
			(uint16_t*) &PCMOutBuffer[0]);

	// Kopiuje dane
	memcpy((uint16_t*) &recBuffer[counter * (PCM_OUT_SIZE * 2)], PCMOutBuffer,
	PCM_OUT_SIZE * 4);

	if (counter == (RECBUFFER_SIZE / (PCM_OUT_SIZE * 4)) - 1) {
		dataReady = 1;
		bufferOffset = 0;
		counter++;
	} else if (counter == (RECBUFFER_SIZE / (PCM_OUT_SIZE * 2)) - 1) {
		dataReady = 1;
		bufferOffset = RECBUFFER_SIZE / 2;
		counter = 0;
	} else {
		counter++;
	}
}

void BSP_AUDIO_IN_HalfTransfer_CallBack(void) {
	// Konwersja PDM do PCM
	BSP_AUDIO_IN_PDMToPCM((uint16_t*) &internalBuffer[0],
			(uint16_t*) &PCMOutBuffer[0]);

	// Kopiuje dane
	memcpy((uint16_t*) &recBuffer[counter * (PCM_OUT_SIZE * 2)], PCMOutBuffer,
	PCM_OUT_SIZE * 4);

	if (counter == (RECBUFFER_SIZE / (PCM_OUT_SIZE * 4)) - 1) {
		dataReady = 1;
		bufferOffset = 0;
		counter++;
	} else if (counter == (RECBUFFER_SIZE / (PCM_OUT_SIZE * 2)) - 1) {
		dataReady = 1;
		bufferOffset = RECBUFFER_SIZE / 2;
		counter = 0;
	} else {
		counter++;
	}
}

