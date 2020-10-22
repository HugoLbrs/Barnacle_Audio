#include <iostream>
#include <deque>
#include <conio.h>

#include "Wave/WaveFile.hpp"
#include "Wave/WaveReader.hpp"
#include "Wave/WaveData.hpp"

#include "Network/Client.hpp"
#include "Network/Server.hpp"

#include "Tool/Timer.hpp"
#include "Tool/Buffer.hpp"

#include "sdl/SDL.h"

#include "gui.hpp"

// ---------------------------------------------------------------------------------------- //
// -------------------------------- Client main program ----------------------------------- //
// ---------------------------------------------------------------------------------------- //

// ------ Globals ------
std::atomic<bool> G_SoftStop = false;
std::atomic<bool> reboot = false;
std::atomic<bool> var = false;
std::atomic<bool> lockKeyboard = true; 
std::atomic<bool> pathFileOk = false;
std::atomic<bool> retry = true;
std::atomic<bool> gogo = true;

int nbreReboot = 0;

int a = 0;
char** b = 0;

bool doSmthingElse = false;
bool microIsOk = false;


// ----------------------- Client --------------------------
void mainClient() {

	/* While client is still active */
	while (gogo) {
		/* reinit all variables */
		var = false;
		lockKeyboard = true;
		pathFileOk = false;
		retry = true;	
		doSmthingElse = false;

		reboot = false;

		/* Entering one action loop */
		while (!reboot) {

			if (nbreReboot > 0){ 
				std::cout << "-------------Hi again !-----------" << std::endl; 
			}
			
			/*  Variables */ 
			Client client;
			std::atomic<bool> openWave = false;
			std::atomic<bool> wavBuffering = false;

			MsgBuffer bufferData;
			int key;

			/*  Wav reader */
			WaveReader wavReader;

			/* Define client events */
			client.onInfo([&](const Message & message) {

				std::cout << "On Info Client !" << std::endl;

				/* Format exchange */
				if (message.code() & Message::FORMAT) {
					/* Set wav format */
					MessageFormat cmd(message.content());
					int numChannels = cmd.valueOf<int>("numChannels");
					int sampleRate = cmd.valueOf<int>("sampleRate");
					int bitsPerSample = cmd.valueOf<int>("bitsPerSample");
					int bufferSize = cmd.valueOf<int>("bufferSize");

					// Non null ?
					if (numChannels != 0 && sampleRate != 0 && bitsPerSample != 0 && bufferSize != 0) {
						/* Setup reader */
						wavReader.setFormat(numChannels, sampleRate, bitsPerSample);
						wavReader.setBuffers(5, bufferSize);

						/* Send nudes */
						client.sendData(Message(Message::DEVICE, "start"));
						wavBuffering = true;

						/* Try open */
						if (!wavReader.open()) {
							std::cout << "wavReader invalide.\n";
							return;
						}
						openWave = true;
					}
					else {
						std::cout << "Format exchange failed\n";
					}
				}

				/* File name exchange*/
				if (message.code() & Message::PATHFILE) {
					std::string msgstr(message.content());
					std::cout << msgstr << std::endl;
					msgstr.erase(msgstr.begin() + 6, msgstr.end());
					std::cout << msgstr << std::endl;

					if (msgstr == "fileok") {
						pathFileOk = true;
						retry = false;
					
					}
					else if (msgstr == "error!") {
						std::cout << "Error: This file doesn't exist !" << std::endl;
						retry = true;
					}
				}

				/* If record audio device correctly plugged */
				if (message.code() & Message::MICRO) {
					microIsOk = true;
				}
				});

			/* Audio streaming */
			client.onData([&](const Message & message) {
				// Store message
				bufferData.lock();
				bufferData.push(message, message.timestamp());
				bufferData.unlock();
				});


			client.onConnect([&]() {			

				/* Choice of the action to do*/
				std::cout << "[ Press 'R' to record sound / Press 'P' to play recorded file ]" << std::endl;

				int action = 0;
				microIsOk = false;

				/* Menu gui*/
				action = gui_menu(a, b);				

				/* If User wants to record */
				if (action == 1) { 

					std::cout << "[------------------------------]" << std::endl;
					std::cout << "[------------RECORD------------]" << std::endl;
					std::cout << "[------------------------------]" << std::endl;

					/* Ask server to record */
					client.sendData(Message(Message::FORMAT, "record"));					
					Timer::wait(200);

					std::string phrase;

					std::cout << "ask saisie" << std::endl;
					/* User fill name of the file he wants to record */
					phrase = saisieRecord(a, b);
					std::cout << "saisie done" << std::endl;

					std::cout << phrase << std::endl;
					lockKeyboard = false;

					int size = phrase.size();
					std::stringstream ss;
					ss << size;
					std::string str = ss.str();

					client.sendData(Message(Message::LENGTH, str));
					client.sendData(Message(Message::PATHFILE, phrase));
					Timer::wait(200);

					std::cout << "Client ask recording" << std::endl;

					/* Record finally starts*/
					client.sendInfo(Message(Message::RECORD, "record"));			

					std::cout << "[ Server recording: Press E when you're done! ]" << std::endl;

					/* While no audio record device plugged */
					while (!microIsOk) { 						
						std::cout << "No mic" << std::endl;
						/* Gui */
						gui_recordNoDevice(a, b);
					}

					/* Client sends Ok message*/
					client.sendData(Message(Message::MICRO, "allisOk!"));

					/* Gui record*/
					/* doSmthingElse becomes true when record finishes */
					doSmthingElse = gui_record(a, b);

					/* Record while E isn't pressed */
					while (GetKeyState('E') != -127) { std::cout << "*" << std::endl; }

					/* Client asks server to stop current record*/
					client.sendInfo(Message(Message::RECORD, "stopre"));

					bool hello = false;
					do { hello = gui_recordDone(a, b); } while (!hello);
					{
						/* Basic gui temporisation */
					}
					var = true;
				}
				
				/* User wants server to stream audio file*/
				if (action == 2) {
			
					std::cout << "[------------------------------]" << std::endl;
					std::cout << "[-------------PLAY-------------]" << std::endl;
					std::cout << "[------------------------------]" << std::endl;

					/* While server didn't get a name file he knows*/
					while (!pathFileOk) {
						/* Then client asks user to write again a file name */
						if (retry) {
							/* Ask for info */
							std::string phrase;

							std::cout << "ask saisie" << std::endl;
 							phrase = saisie(a, b);
							std::cout << "saisie done" << std::endl;

							phrase = phrase + ".wav";
							char cstr[256];
							memset(cstr, 0, 256);
							std::copy(phrase.begin(), phrase.end(), cstr);

							std::cout << "Write the name of the file you want to play: ";
							std::cout << phrase << std::endl;
							
							int size = phrase.size();
							std::stringstream ss;
							ss << size;
							std::string str = ss.str();	
						
							client.sendData(Message(Message::LENGTH, str));

							client.sendData(Message(Message::PATHFILE, phrase));
							retry = false;

							/* Timer to tempo server anwser concerning file name*/
							Timer::wait(200);

						}
					}

					lockKeyboard = false;

					std::cout << "Client ask format\n";
					client.sendInfo(Message(Message::FORMAT, "?"));
					std::cout << "[ Press O to stop lecture ! ]" << std::endl;

					/* Play gui */
					/* doSmthingElse becomes true when client want to stop current audio streaming */
					doSmthingElse = gui_play(a, b);
					var = doSmthingElse;
				}
				});

			// Finally, connect.
			client.connectTo("127.0.0.1", 8888); // localhost

			std::cout << "*Client conneting to server*" << std::endl;
			std::cout << std::endl;

			/* Loop	 */
			Message msg(" ");
			
			/* Lock program while user is writing something*/
			while (lockKeyboard){}			

			/* Main audio stream loop*/
			while ((!G_SoftStop) && !(GetKeyState('O') < 0) && !reboot && !var && !doSmthingElse) {
				
				std::cout << "E: " << GetKeyState('E') << std::endl;
				std::cout << "O:" << GetKeyState('O') << "!G_SoftStop: " << !G_SoftStop  << "!reboot" << !reboot << "!var:" << !var << std::endl;

				/* if audio playing device working */
				if (openWave) {
					/* Unload a message */
					bufferData.lock();
					bool updated = bufferData.update(msg);
					size_t bufferDataSize = bufferData.size();
					bufferData.unlock();

					if (!updated)
						continue;

					/* String to data (unserialize) */
					WaveData wavData(msg.str());

					/* Add to lecture queue	*/		
					wavReader.queueData(wavData);

					/* Regulate buffer size */
					if (bufferDataSize > 100 && wavBuffering) {
						client.sendData(Message(Message::DEVICE, "stop"));
						wavBuffering = false;
					}

					if (bufferDataSize < 20 && !wavBuffering) {
						client.sendData(Message(Message::DEVICE, "start"));
						wavBuffering = true;
					}
					printf("%zd", bufferDataSize);
				}
			}

			std::cout << std::endl;

			/* Client wants to stop current audio streaming */
			client.sendData(Message(Message::DEVICE, "stop"));

			/* End */
			wavReader.close();
			wavBuffering = false;

			std::cout << "Am there" << std::endl;

			/* Lock until current record or current audio streaming finishes */
			while (!doSmthingElse) { };

			/* Second lock, needed to get the app working */
			while (!var) {};

			/* Gui asking client if he wants to do something else */
			bool action2 = NULL;
			std::cout << "[ Press 'Y' to do something else / Press 'N' if not ]" << std::endl;
			/* action2 becomes true if Yes and false if No */
			action2 = gui_smthingElse(a, b);

			std::cout << "Again :" << action2 << std::endl;

			/* User wants to do something else */
			if (action2) {
				std::cout << "Time for new adventures !" << std::endl;
				client.sendInfo(Message(Message::DEVICE, "rebot"));
				client.disconnect();
				/* Reboot asked, goes back to the beginning of the program*/
				reboot = true;
			}

			/* User doesn't want to do something else */
			if (!action2) {
				std::cout << "[ Press Q... ]" << std::endl;
				reboot = true;
				gogo = false;
				client.sendInfo(Message(Message::DEVICE, "rebot"));
				client.disconnect();
			}

			client.disconnect();
		}
		Timer::wait(500);
	}

	//Press Q broken
	std::cout << "Exit.\n";
	exit(0);

}



// ------------------------------------- Entry point -------------------------------------
int main(int argc, char** argv) {

	a = argc;
	b = argv;

	// Launch client soft
	std::thread threadClient(mainClient);

	std::cout << "[-----------------------------------------------]" << std::endl;
	std::cout << "[-----Press 'Q' if you want to leave us :'(-----]" << std::endl;
	std::cout << "[-----------------------------------------------]" << std::endl;

	int ch;

	
	// Loop until a key is pressed
	do {
		Timer::wait(10);
		ch = _getch();
		ch = toupper(ch);

	} while (ch != 'Q');

	// End
	G_SoftStop = true;  

	/*if (threadClient.joinable())
		threadClient.join();*/

	std::cout << "Exit.\n";

	

	exit(0);
	return 0;
}
