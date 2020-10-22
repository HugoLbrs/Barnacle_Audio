#include "sdl/SDL.h"
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <fstream>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

//--- Using old functions, disable needed ---
#pragma warning(disable: 4996)

//--- Global variables --- 
//--- SDL variables ---
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_AudioSpec spec;
static SDL_AudioSpec spec2;
static SDL_AudioDeviceID devid_in = 0;
static SDL_AudioDeviceID devid_out = 0;

//--- Type variables ---
typedef int int32;
typedef short int16;
typedef char int8;

FILE* F;

//--- Record parameters ---
#define FREQUENCE 44100
#define BITPERSAMPLE 16

//--- Definition of functions ---
bool openAudioDevice(char** argv, SDL_AudioSpec& spec2);
bool recordBoolOk = false;
bool ouvertureDejaFaite = false;


//--------------------- WAV FILE STRUCTURE ------------------------------------
//--- Header Wav file structure ---
struct WavHeader
{
	int8 FileTypeBlocID[4];
	int32 FileSize;
	int8 FileFormatID[4];
};

//--- Wav format structure ---
struct WavFmt
{
	int8 FormatBlocID[4];
	int32 BlocSize;
	int16 AudioFormat;
	int16 NbrCanaux;
	int32 Frequence;
	int32 BytePerSec;
	int16 BytePerBloc;
	int16 BitsPerSample;
};
//------------------------------------------------------------------------------

//--- Function to write data in little endian ---
void ecrire_little_endian(unsigned int octets, int taille, FILE* fichier) {
	unsigned char faible;
	while (taille > 0) {
		faible = octets & 0x000000FF;
		fwrite(&faible, 1, 1, fichier);
		octets = octets >> 8;
		taille = taille - 1;
	}
}

//--- Some other variables ---
unsigned int taux;
unsigned long int taille;
double duree;

/*double* gauche, * droite;*/


//--- Write both HEADER & FORMAT ---
FILE* wavfile_open(const char* filename, FILE* F)
{
	unsigned short int nb_canaux = 1;
	unsigned short int nb_bits = 16;
	taux = 44100; // En Hz
	duree = 60; // En secondes
	taille = taux * duree;
	unsigned long int nb_octets_donnees = (nb_bits / 8) * nb_canaux * taille;

	fwrite("RIFF", 4, 1, F);
	ecrire_little_endian(36 + nb_octets_donnees, 4, F);

	fwrite("WAVE", 4, 1, F);
	fwrite("fmt ", 4, 1, F);
	ecrire_little_endian(16, 4, F);
	ecrire_little_endian(1, 2, F);
	ecrire_little_endian(nb_canaux, 2, F);
	ecrire_little_endian(taux, 4, F);
	ecrire_little_endian(taux * nb_canaux * (nb_bits / 8), 4, F);
	ecrire_little_endian(nb_canaux * (nb_bits / 8), 2, F);
	ecrire_little_endian(nb_bits, 2, F);
	fwrite("data", 4, 1, F);
	ecrire_little_endian(taille * nb_canaux * (nb_bits / 8), 4, F);

	return F;
}

//--- Write sound data in file ---
void wavfile_write(FILE * F, unsigned char* data, int32 length)
{
	if (F) {

		if (data[1] != (unsigned char)"") {
			fwrite(data, sizeof(unsigned char), length, F);
		}
	}
	else { std::cout << "Error file"; }
}

//--- Write FileSize and Data Size in Wav Header and close the file ---
void wavfile_close(FILE * file)
{
	int file_length = ftell(file); //in byte = octet
	int32 FileSize = file_length - 8;
	std::cout << "/******************************************************************/" << std::endl;
	std::cout << "Taille fichier en octets: " << FileSize;
	fseek(file, 4, SEEK_SET);
	ecrire_little_endian(FileSize, sizeof(FileSize), F);
	//fwrite(&FileSize, sizeof(FileSize), 1, file);
	fseek(file, 40, SEEK_SET);
	int32 DataSize = FileSize - 44;
	ecrire_little_endian(DataSize, sizeof(DataSize), F);
	//fwrite(&DataSize, sizeof(DataSize), 1, file);
	std::cout << " -- Taille data en octets: " << DataSize << std::endl;
	std::cout << "/******************************************************************/" << std::endl;

	char stop[5] = "STOP";
	fseek(file, 40, SEEK_END);
	fwrite(stop, 4, 1, file);

	fclose(file);
}



//--- Record, playback, write data ---
static void loop()
{
	SDL_bool please_quit = SDL_FALSE;
	SDL_Event e;

	//https://wiki.libsdl.org/SDL_PollEvent
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			please_quit = SDL_TRUE;
		}
		else if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_ESCAPE) {
				please_quit = SDL_TRUE;
				std::cout << "Escape pressed" << std::endl;
			}
		}
	}

	if (please_quit) {
		/* Close everything */
		wavfile_close(F);
		SDL_Log("Shutting down.\n");
		SDL_PauseAudioDevice(devid_in, 1);
		SDL_CloseAudioDevice(devid_in);

		SDL_Quit();
		exit(0);
	}

	int avant = SDL_GetTicks();
	std::cout << recordBoolOk;
	while (recordBoolOk) {
		std::cout << recordBoolOk;
		SDL_PauseAudioDevice(devid_in, SDL_FALSE);
	}
	std::cout << recordBoolOk;

	wavfile_close(F);
	SDL_Log("Shutting down.\n");
	SDL_PauseAudioDevice(devid_in, 1);
	SDL_CloseAudioDevice(devid_in);


	SDL_Quit();
}

//--- Callback function ---
static void _sdl_cb(void* userdata, unsigned char* pcm, int len) {
	if (SDL_GetAudioDeviceStatus(devid_in) == SDL_AUDIO_PLAYING) {
		wavfile_write(F, pcm, len);		
	}
}

//--- Record function ---
bool record(int argc, char** argv, std::string name, bool micro)
{
	std::string buffer = name;
	name = name + ".wav";
	char cstr[256];
	memset(cstr, 0, 256);
	copy(name.begin(), name.end(), cstr);
	   
	std::cout << "Record file name is: " << cstr << std::endl;
	F = fopen(cstr, "wb");

	const char* devname = argv[1];
	SDL_AudioSpec wanted;
	int devcount;
	int i;

	/* Enable standard application logging */
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

	/* Load the SDL library */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
		return (1);
	}

	/* Get audio driver */
	SDL_Log("Using audio driver: %s\n", SDL_GetCurrentAudioDriver());

	/* Get audio device name */
	devcount = SDL_GetNumAudioDevices(SDL_TRUE);
	for (i = 0; i < devcount; i++) {
		SDL_Log(" Capture device #%d: '%s'\n", i, SDL_GetAudioDeviceName(i, SDL_TRUE));
	}

	/* Fill wanted format */
	SDL_zero(wanted);
	wanted.freq = 44100;
	wanted.format = AUDIO_S16;
	wanted.channels = 1;
	wanted.samples = 4096;
	wanted.callback = NULL;

	SDL_zero(spec);

	SDL_Log("Opening default playback device...\n");

	spec2 = spec;
	spec2.callback = _sdl_cb;

	SDL_Log("Opening capture device %s%s%s...\n",
		devname ? "'" : "",
		devname ? devname : "[[default]]",
		devname ? "'" : "");

	//https://wiki.libsdl.org/SDL_OpenAudioDevice

	/* Open audio device */
	ouvertureDejaFaite = openAudioDevice(argv, spec2);
	
	/* Try until audio device is open, usefull in case no microphone plugged */
	if (!ouvertureDejaFaite && !micro) { SDL_Quit(); return false; }
	if (ouvertureDejaFaite && !micro) { SDL_Quit(); return true; }

	/* We are sure that microphone is plugged */
	if (micro) {

		/* Write Header and Format */
		wavfile_open("recordtest.wav", F);

#ifdef __EMSCRIPTEN__
		emscripten_set_main_loop(loop, 0, 1);
#else

		/* Record, playback and write data */

		loop();
		std::cout << "Record ended";
#endif
		return true;
	}
	return true;
}

//--- Test function, useless there ---
int blabla() {
	std::cout << "blabla";
	return 0;
}

//--- To check if user want to record ---
void shouldIrecord(bool recordBool) {
	recordBoolOk = recordBool;
}

//--- Open audio device function ---
bool openAudioDevice(char** argv,  SDL_AudioSpec &spec2) {
	devid_in = SDL_OpenAudioDevice(argv[1], SDL_TRUE, &spec2, &spec2, 0);
	if (!devid_in) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't open an audio device for capture: %s!\n", SDL_GetError());
		devid_in = SDL_OpenAudioDevice(argv[1], SDL_TRUE, &spec2, &spec2, 0);
		return false;

	}
	return true;
}
	
