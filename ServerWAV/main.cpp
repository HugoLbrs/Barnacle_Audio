#pragma warning(disable : 4996)

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

#include "microphoneRecorder.hpp"


// ---------------------------------------------------------------------------------------- //
// -------------------------------- Server main program ----------------------------------- //
// ---------------------------------------------------------------------------------------- //

// ------ Globals ------
std::atomic<bool> G_SoftStop = false;
std::atomic<bool> reboot = false;

int a = 0;
char** b = 0;

std::atomic<bool> test = false;//
std::atomic<bool> pathFileOk = false;//
std::atomic<bool> retry = true;

int longueurInt;
std::string longueurStr;

bool micIsOk = false;
bool utilisateurMicroOk = false;


// ----------------------- Server --------------------------

void mainServer() {

	// --------------------- Server loop ------------------------------------------------------------
	while (true) {

		/* Reinit all variables */
		reboot = false;
		retry = true;
		pathFileOk = false;
		test = false;
		utilisateurMicroOk = false;

		std::string phrase = "defaut.wav";
		std::string phraseDefaut = "defaut.wav";
		std::string phraseModif = "phrasetamponphrasetampon";		
		std::string* phraseRecord = new std::string();

		// ------------------------------------------ One action loop -------------------------------
		while (!reboot) {

			/* While no reboot asked */
			std::cout << "server has been rebooted" << std::endl;
			G_SoftStop = false;

			// Variables 
			Server server;
			std::atomic<bool> wavBuffering = false; 
			const size_t BUFFER_SIZE = 1000;

			// Finally, connect.
			server.connectAt(8888);

			server.onClientDisconnect([&](const Server::ClientInfo & client)
			{
				std::cout << "Client disconnected!" << std::endl;
				/* If client disconnected, reboot server */
				reboot = true;
			});

			server.onClientConnect([&](const Server::ClientInfo & client) {
				std::cout << "New client !" << client.id() << std::endl;
			});

			server.onData([&](const Server::ClientInfo & client, const Message & message) {
				/* Controle audio streaming */
				if (message.code() & Message::DEVICE) {
					std::string msgstr(message.content());

					if (msgstr == "start") {
						/* User wants to play a file on server */
						wavBuffering = true;
						std::cout << "started \n";
					}
					else if (msgstr == "stop") {
						/* User wants to stop playing file */
						wavBuffering = false;
						std::cout << "stopped \n";
					}
					else { /* Bug with reading */
						wavBuffering = !wavBuffering;
						std::cout << (wavBuffering ? "started" : "stopped") << "\n";
					}
				}

				/* To get file name */
				if (message.code() & Message::PATHFILE) {
					std::string phraseFinale = "phrasetamponphrasetampon";
					phrase = message.content();
					
					std::cout << "Name i received is: " << phrase << std::endl;		
					int a = 0;					

					/* Only keep ASCII letters, japanese computer sometimes write kanjis */
					for (std::string::size_type i = 0; i < phrase.length(); ++i)
					{
						char c = phrase[i];
						if ((c < 127) && (c > 32)) { 
							std::cout << c;
							phraseModif[a] = c;
							std::cout << "{" << phraseModif[a] << "}" << std::endl;
							a++;
						}
					}

					/* Check if name has correctly been modified */
					std::cout << "PHRASE MODIF 1 = " << phraseModif << std::endl;
					phraseModif.erase(phraseModif.begin() + longueurInt, phraseModif.end());
					std::cout << "PHRASE MODIF 2 = " << phraseModif << std::endl;

					phrase = phraseModif;
					phraseDefaut = phraseModif;
					
					*phraseRecord = phraseModif;
					std::cout << "All names processed: " << phraseModif << "//" << phraseRecord << "//" << *phraseRecord << "//" << &phraseRecord << std::endl;

					/* Once we got name, we can go further */
					test = true;
					retry = true;

				}

				if (message.code() & Message::FORMAT) {
					test = true;
				}

				if (message.code() & Message::LENGTH) {
					/* Check name length to check it*/
					longueurStr = message.content();
					std::cout << "Longueur str: " << longueurStr << "*" << std::endl;
					sscanf(longueurStr.c_str(), "%d", &longueurInt);
					Timer::wait(1000);
					std::cout << "Longueur int: " << longueurInt << "*" << std::endl;
				}

				/* User checked that microphone was there */
				if (message.code() & Message::MICRO) {
					utilisateurMicroOk = true;
				}

				});

			std::cout << "O";
			/* Blocking functions, unlocked when we got all needed informations */
			while (!test){}

			/* Lock untilwe received file name */
			while (!pathFileOk) {
				if (retry) {
					std::cout << "Final sentence is: " << phrase << std::endl;
					WaveFile wavFile(phrase);

					if (wavFile.isValide()) {
						std::cout << "wavFile valide.\n";

						for (auto& client : server.getClients()) {
							server.sendInfo(client, Message(Message::PATHFILE, "fileok"));
						}

						pathFileOk = true;
						retry = false;

					}

					if (!wavFile.isValide()) {
						std::cout << "wavFile invalide.\n";

						for (auto& client : server.getClients()) {
							server.sendInfo(client, Message(Message::PATHFILE, "error!"));
						
						}
						retry = false;
					}
				}
			}

			std::cout << "phraseDefaut Wave file: " << phraseDefaut << std::endl;
			WaveFile wavFile(phraseDefaut);
			
			/* Define server  */
			server.onInfo([&](const Server::ClientInfo & client, const Message & message) {

				/* Get Wav format */
				if (message.code() & Message::FORMAT) {
					MessageFormat cmd;
					cmd.add("numChannels", wavFile.get(WaveFile::numChannels));
					cmd.add("sampleRate", wavFile.get(WaveFile::sampleRate));
					cmd.add("bitsPerSample", wavFile.get(WaveFile::bitsPerSample));
					cmd.add("bufferSize", BUFFER_SIZE);

					std::cout << "Server send format\n";
					server.sendInfo(client, Message(Message::FORMAT, cmd.str()));
				}				

				/* If client wants to record */
				if (message.code() & Message::RECORD) {
					/* Get name of the file */
					std::string msgstr(message.content());
					std::cout << msgstr << std::endl;
					msgstr.erase(msgstr.begin() + 6, msgstr.end());
					std::cout << msgstr << std::endl;

					/* Client start record*/
					if (msgstr == "record") {

						std::cout << "record \n";
						shouldIrecord(true);
						std::cout << "File name: //" << phraseRecord << "//" << *phraseRecord << "//" << &phraseRecord << "//"  << std::endl;
						
						bool ouvertureDejaFaite = false;						
						
						/* While no audio record device, try again */
						while (!ouvertureDejaFaite) { std::cout << "wait for mic" << std::endl; ouvertureDejaFaite = record(a, b, *phraseRecord, false);};

						/* Say to client that all is ready to record audio */
						std::cout << "++++++++++++++++++++++++++Mic Ok++++++++++++++++++++++++++" << std::endl;
						server.sendInfo(client, Message(Message::MICRO, "allisOk!"));
						std::cout << "++++++++++++++++++++++++++Mic Ok++++++++++++++++++++++++++" << std::endl;

						/* Wait client anwser */
						while(!utilisateurMicroOk){}

						/* Start record */
						record(a, b, *phraseRecord, true);
						

					}
					
					/* If client wants to stop recording session */
					else if (msgstr == "stopre") {
						std::cout << "stopped_record \n";
						/* Stop record */
						shouldIrecord(false);
					}
					else { // Bug with reading
						std::cout << "problem \n";
					}
				}

				/* If reboot asked, to do something else */
				if (message.code() & Message::DEVICE) {
					reboot = true;
					G_SoftStop = true;
				}
				});

			/* Main Loop */
			/* While user don't want to stop what is happening */

			while (!G_SoftStop) {
				/* Streaming data if user want to play something */
				/* Lock until client ask for playing audio file */
				/* Stay in the while if user only wants to record */
				if (wavBuffering) {
					for (auto& client : server.getClients()) {
						WaveData wavData(BUFFER_SIZE);
						wavFile.readSamples(&wavData);
						// Data to string (serialize)
						server.sendData(client, Message(wavData.str()));
					}
					Timer::wait(2);
				}
			}

			/* End */
			/* Means that client finished want he wanted to do */
			/* Record done, playing audio file finished */
			/* Server going back to beginning, ready to do something else */

			wavBuffering = false;
			server.disconnect();
		
		}
	}
}

// ------------------------------------- Entry point -------------------------------------
int main(int argc, char** argv) {
	a = argc;
	b = argv;
	// Launch server soft
	std::thread threadServer(mainServer);

	// Wait for server to be ready
	Timer::wait(500);

	// Loop until a key is pressed
	do {
		Timer::wait(10);
	} while (!_kbhit());


	// End
	G_SoftStop = true;

	if (threadServer.joinable())
		threadServer.join();

	std::cout << "Exit.\n";
	return 0;
}