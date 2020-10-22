#include <stdlib.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <sdl/SDL.h>
#include <sdl/SDL_ttf.h>
#include <cstdio>


#include <fstream>
#include <deque>
#include <atomic>
#include <conio.h>

//--- Global variables & functions ---
void pause();
int action = 0;
bool enter = false;

bool again = NULL;

//--- Menu function ---
int gui_menu(int argc, char* argv[])
{

	SDL_Init(SDL_INIT_VIDEO); /* Initializing SDL */

	SDL_Window* window = SDL_CreateWindow("Barnacle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 683, 482, 0);
	
	SDL_Surface* logo = SDL_LoadBMP("logo.bmp");
	SDL_SetWindowIcon(window, logo);


	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Surface* image = SDL_LoadBMP("menu.bmp");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	SDL_Delay(1500);
	image = SDL_LoadBMP("menuEnter.bmp");
	texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	pause();
	
	image = SDL_LoadBMP("menuChoose.bmp");
	texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	/* Pause */
	pause(); 

	/* Stop SDL */
	/* Important if you don't want any crash */
	SDL_Quit(); 

	/* Quit window */
	return action; 
}

//--- Pause function, to get user command ---
void pause()
{
	int continuer = 1;
	SDL_Event event;

	/* While no key pressed */
	while (continuer)
	{
		SDL_WaitEvent(&event);
		switch (event.type)
		{
		/*case SDL_QUIT:
			continuer = 0;
			break;*/
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_r: {
				action = 1;
				continuer = 0;
				break; }
			case SDLK_p: {
				action = 2;
				continuer = 0;
				break; }
			case SDLK_RETURN: {
				continuer = 0;
				break; }
			case SDLK_e:
				continuer = 0;
				break;
			case SDLK_o: {
				continuer = 0;
				break; 
			}
			case SDLK_y: {
				std::cout << "Pressed Y!" << std::endl;
				again = true;
				continuer = 0;
				break;
			}
			case SDLK_n: {
				again = false;
				continuer = 0;
				break;
			}
			}
		}
	}
}

//--- Play function ---
bool gui_play(int argc, char* argv[])
{

	SDL_Init(SDL_INIT_VIDEO); // Initialisation de la SDL

	SDL_Window* window = SDL_CreateWindow("Barnacle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 683, 482, 0);

	SDL_Surface* logo = SDL_LoadBMP("logo.bmp");
	SDL_SetWindowIcon(window, logo);


	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Surface* image = SDL_LoadBMP("playPlaying.bmp");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	/* Pause */
	pause();

	/* Stop SDL */
	/* Important if you don't want any crash */
	SDL_Quit();

	/* Quit window */
	return true;
}

//--- Record function ---
bool gui_record(int argc, char* argv[])
{

	SDL_Init(SDL_INIT_VIDEO); // Initialisation de la SDL

	
	SDL_Window* window = SDL_CreateWindow("Barnacle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 683, 482, 0);

	SDL_Surface* logo = SDL_LoadBMP("logo.bmp");
	SDL_SetWindowIcon(window, logo);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Surface* image = SDL_LoadBMP("recording.bmp");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	/* Pause */
	pause();

	/* Stop SDL */
	/* Important if you don't want any crash */
	SDL_Quit();

	/* Quit window */
	return true;
}

//--- Record done function ---
bool gui_recordDone(int argc, char* argv[])
{

	SDL_Init(SDL_INIT_VIDEO); // Initialisation de la SDL

	SDL_Window* window = SDL_CreateWindow("Barnacle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 683, 482, 0);

	SDL_Surface* logo = SDL_LoadBMP("logo.bmp");
	SDL_SetWindowIcon(window, logo);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Surface* image = SDL_LoadBMP("recordDone.bmp");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	SDL_Delay(3000); 

	/* Stop SDL */
	/* Important if you don't want any crash */
	SDL_Quit();

	/* Quit window */
	return true;
}

//--- Do you want to do something else function? ---
bool gui_smthingElse(int argc, char* argv[])
{
	//bool again = NULL;
	SDL_Init(SDL_INIT_VIDEO); // Initialisation de la SDL

	SDL_Window* window = SDL_CreateWindow("Barnacle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 683, 482, 0);

	SDL_Surface* logo = SDL_LoadBMP("logo.bmp");
	SDL_SetWindowIcon(window, logo);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Surface* image = SDL_LoadBMP("menuSmthingElse.bmp");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	/* Pause */
	pause();

	/* Stop SDL */
	/* Important if you don't want any crash */
	SDL_Quit();

	/* Quit window */
	return again;
}

//--- No record device function ---
bool gui_recordNoDevice(int argc, char* argv[])
{
	//bool again = NULL;
	SDL_Init(SDL_INIT_VIDEO); // Initialisation de la SDL

	SDL_Window* window = SDL_CreateWindow("Barnacle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 683, 482, 0);

	SDL_Surface* logo = SDL_LoadBMP("logo.bmp");
	SDL_SetWindowIcon(window, logo);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Surface* image = SDL_LoadBMP("recordNoDevice.bmp");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	/* Pause */
	pause();

	/* Stop SDL */
	/* Important if you don't want any crash */
	SDL_Quit();

	/* Quit window */
	return true;
}

std::string saisie(int argc, char* argv[])
{

	TTF_Init();
	TTF_Font* police = NULL;
	SDL_Color couleurNoire = { 162, 0, 37};
	SDL_Surface* texte;
	SDL_Rect position;

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Barnacle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);

	SDL_Surface* logo = SDL_LoadBMP("logo.bmp");
	SDL_SetWindowIcon(window, logo);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Surface* image = SDL_LoadBMP("playEnter.bmp");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	/* Loading font */
	police = TTF_OpenFont("font.ttf", 40);
	if (!police) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		/* Handle error */
	}
	   
	bool isRunning = true;
	SDL_Event ev;

	std::string text = "";
	SDL_StartTextInput();

	/* Kind of homemade scanf function, because SDL don't have any */
	while (isRunning) {	
		while (SDL_PollEvent(&ev) != 0)
			if (ev.type == SDL_QUIT)
				isRunning = false;
			else if ((ev.type == SDL_TEXTINPUT || ev.type == SDL_KEYDOWN) && ev.key.keysym.sym != SDLK_RETURN) {
				if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_BACKSPACE && text.length() > 0)
				{
					std::cout << "1";
					text = text.substr(0, text.length() - 1);			
				}
				else if (ev.type == SDL_TEXTINPUT)
				{
					std::cout << "2";
					text += ev.text.text;
					
					/* Write text in SDL_Surface */

					char cstr[256];
					memset(cstr, 0, 256);
					std::copy(text.begin(), text.end(), cstr);

					texte = TTF_RenderText_Blended(police, cstr, couleurNoire);
					if (!texte) {
						printf("TTF_TEXTE: %s\n", TTF_GetError());
						/* Handle error */
					}

					/* Text position on screen */
					position.x = 200;
					position.y = 265;

					/* Blit text */
					SDL_BlitSurface(texte, NULL, image, &position); 
					texture = SDL_CreateTextureFromSurface(renderer, image);
					SDL_RenderCopy(renderer, texture, NULL, NULL);
					SDL_RenderPresent(renderer);					
				}

				else if (ev.type == SDLK_RETURN)
				{
					/* If enter, stop scanf */
					isRunning = false;
					std::cout << "ENTER" << std::endl;
				}

			std::cout << text << std::endl;
			}

			if (ev.key.keysym.sym == SDLK_RETURN) {
				/* if enter, stop scanf */
				std::cout << "ENTER" << std::endl;
				isRunning = false;
			}

	}

	std::cout << text << std::endl;

	/* If User pressed enter without anything written, set defaut name, otherwise program crash*/
	if (text == "") { text = "defaut"; }

	SDL_StopTextInput();
	SDL_Quit();

	return text;	
}

/* Almost same function as before */
std::string saisieRecord(int argc, char* argv[])
{

	TTF_Init();
	TTF_Font* police = NULL;
	SDL_Color couleurNoire = { 162, 0, 37};
	SDL_Surface* texte;
	SDL_Rect position;

	SDL_Init(SDL_INIT_VIDEO); 
	SDL_Window* window = SDL_CreateWindow("Barnacle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);

	SDL_Surface* logo = SDL_LoadBMP("logo.bmp");
	SDL_SetWindowIcon(window, logo);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Surface* image = SDL_LoadBMP("recordSaisie.bmp");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	
	police = TTF_OpenFont("font.ttf", 40);
	if (!police) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
	}


	bool isRunning = true;
	SDL_Event ev;

	std::string text = "";
	SDL_StartTextInput();

	while (isRunning) {

		while (SDL_PollEvent(&ev) != 0)
			if (ev.type == SDL_QUIT)
				isRunning = false;
			else if ((ev.type == SDL_TEXTINPUT || ev.type == SDL_KEYDOWN) && ev.key.keysym.sym != SDLK_RETURN) {
				//system("cls");
				if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_BACKSPACE && text.length() > 0)
				{
					std::cout << "1";
					text = text.substr(0, text.length() - 1);
				}
				else if (ev.type == SDL_TEXTINPUT)
				{
					std::cout << "2";
					text += ev.text.text;					

					char cstr[256];
					memset(cstr, 0, 256);
					std::copy(text.begin(), text.end(), cstr);

					texte = TTF_RenderText_Blended(police, cstr, couleurNoire);
					if (!texte) {
						printf("TTF_TEXTE: %s\n", TTF_GetError());						
					}

					position.x = 200;
					position.y = 265;

					SDL_BlitSurface(texte, NULL, image, &position); 
					texture = SDL_CreateTextureFromSurface(renderer, image);
					SDL_RenderCopy(renderer, texture, NULL, NULL);
					SDL_RenderPresent(renderer);
				}

				else if (ev.type == SDLK_RETURN)
				{
					isRunning = false;
					std::cout << "ENTER" << std::endl;
				}

				std::cout << text << std::endl;

			}

		if (ev.key.keysym.sym == SDLK_RETURN) {
			std::cout << "ENTER" << std::endl;
			isRunning = false;
		}

	}

	std::cout << text << std::endl;
	if (text == "") { text = "failname"; }

	SDL_StopTextInput();
	SDL_Quit();

	return text;

}


