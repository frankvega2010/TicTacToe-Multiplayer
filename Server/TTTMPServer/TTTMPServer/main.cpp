#include "TaTeTi.h"

int matchesId = 0;

struct message
{
	byte cmd;
	char data[255];
};

int sockInit(void)
{
#ifdef _WIN32
	WSADATA wsa_data;
	return WSAStartup(MAKEWORD(1, 1), &wsa_data);
#else
	return 0;
#endif
}

int sockQuit(void)
{
#ifdef _WIN32
	return WSACleanup();
#else
	return 0;
#endif
}

/* Note: For POSIX, typedef SOCKET as an int. */
int sockClose(SOCKET sock)
{
	int status = 0;
#ifdef _WIN32
	status = shutdown(sock, SD_BOTH);
	if (status == 0) { status = closesocket(sock); }
#else
	status = shutdown(sock, SHUT_RDWR);
	if (status == 0) { status = close(sock); }
#endif
	return status;
}

int TryToGetEmptyMatch(vector<Match>& matches, TaTeTi& ttt)
{
	for (int i = 0; i < matches.size(); i++)
	{
		if (!matches[i].isInUse)
		{
			return matches[i].ID;
		}
	}

	return -1;
}

bool TryStartGame(vector<Match>& matches, TaTeTi& ttt, vector<User>& currentUsers)
{
	int player1Index = -1;
	int player2Index = -1;
	int currentID = -1;
	currentID = TryToGetEmptyMatch(matches, ttt);

	if (currentID == -1)
	{
		matches.push_back(Match());
		matches[matches.size() - 1].ID = matchesId;
		matches[matches.size() - 1].isInUse = true;
		currentID = matches[matches.size() - 1].ID;
		matchesId++;
		ttt.RestartCells(matches[currentID]);
	}
	else
	{
		ttt.SetIsInUse(matches[currentID], true);
		ttt.RestartCells(matches[currentID]);
		ttt.SetTurns(matches[currentID], 8);
		matches[currentID].player1HasEntered = false;
		matches[currentID].player2HasEntered = false;
	}
	
	for (int i = 0; i < currentUsers.size(); i++)
	{
		if (!currentUsers[i].isPlaying && !currentUsers[i].connectionLost && currentUsers[i].wantsToPlayAgain)
		{
			if (!matches[currentID].player1HasEntered)
			{
				player1Index = i;
				currentUsers[i].matchID = currentID;
				currentUsers[i].isPlayer1 = true;
				currentUsers[i].isPlayer2 = false;
				currentUsers[i].isPlaying = true;
				currentUsers[i].wantsToPlayAgain = false;
				currentUsers[i].cellType = cross;
				matches[currentID].player1HasEntered = true;
			}
			else if (!matches[currentID].player2HasEntered)
			{
				player2Index = i;
				currentUsers[i].matchID = currentID;
				currentUsers[i].isPlayer2 = true;
				currentUsers[i].isPlayer1 = false;
				currentUsers[i].isPlaying = true;
				currentUsers[i].wantsToPlayAgain = false;
				currentUsers[i].cellType = dot;
				matches[currentID].player2HasEntered = true;
				ttt.StartGame(matches[currentID]);//StartGame(currentID);
				i = currentUsers.size();
			}
		}
	}

	if (!matches[currentID].player1HasEntered || !matches[currentID].player2HasEntered)
	{
		if (matches[currentID].player1HasEntered)
		{
			currentUsers[player1Index].matchID = -1;
			currentUsers[player1Index].isPlayer1 = false;
			currentUsers[player1Index].isPlayer2 = false;
			currentUsers[player1Index].isPlaying = false;
			currentUsers[player1Index].wantsToPlayAgain = false;
			matches[currentID].player1HasEntered = false;
		}
		if (matches[currentID].player2HasEntered)
		{
			currentUsers[player2Index].matchID = -1;
			currentUsers[player2Index].isPlayer1 = false;
			currentUsers[player2Index].isPlayer2 = false;
			currentUsers[player2Index].isPlaying = false;
			currentUsers[player2Index].wantsToPlayAgain = false;
			matches[currentID].player2HasEntered = false;
		}

		matches[currentID].isInUse = false;

		matches.pop_back();
		return false;
	}

	return true;
}

// devuelve un float random
int irand()
{
	return (int)rand() / (int)RAND_MAX;
}

// devuelve un valor random entre a y b.
int irand(float a, float b)
{
	return irand() * (b - a) + a;
}

User* GetPlayerData(vector<User>& users, vector<Match>& matches, TaTeTi& ttt, int matchID)
{
	User* users2 = new User[PLAYER_SIZE];

	int playersFound = 0;

	for (int i2 = 0; i2 < users.size(); i2++)
	{
		if (playersFound < 2)
		{
			if (users[i2].matchID == matchID && !users[i2].connectionLost && users[i2].isPlaying && (users[i2].isPlayer1 || users[i2].isPlayer2))
			{
				//GET DATA
				users2[playersFound] = users[i2];

				playersFound++;
			}
		}
		else
		{
			i2 = users.size();
		}
	}

	return users2;
}

void ShowBoard(vector<User>& users, vector<Match>& matches, TaTeTi& ttt, int matchID, message& msg)
{
	int playersFound = 0;

	for (int i2 = 0; i2 < users.size(); i2++)
	{
		if (playersFound < 2)
		{
			if (users[i2].matchID == matchID && !users[i2].connectionLost && users[i2].isPlaying && (users[i2].isPlayer1 || users[i2].isPlayer2))
			{
				//SEND BOARD
				string command = "0" + ttt.GetCurrentGameBoard(matches[matchID]);

				msg = *((message*)command.c_str());
				sendto(users[i2].listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&users[i2].client, sizeof(users[i2].client));

				playersFound++;
			}
		}
		else
		{
			i2 = users.size();
		}
	}
}

void NextTurn(vector<User>& users, vector<Match>& matches, TaTeTi& ttt, int matchID, message& msg)
{
	ttt.NextPlayer(matches[matchID]);

	if (!matches[matchID].nextPlayer)
	{
		//SAVE PLAYER1 ALIAS AND CELLTYPE.
		string player1CellType = "";
		string name;

		for (int i2 = 0; i2 < users.size(); i2++)
		{
			if (users[i2].matchID == matchID && !users[i2].connectionLost && users[i2].isPlaying && users[i2].isPlayer1)
			{
				//IT IS YOUR TURN (CMD 1)
				// SAVE PLAYER DATA.
				string cellType(1, ttt.CellToChar(users[i2].cellType));
				player1CellType = cellType;
				name = users[i2].alias;

				string command = "1Es su turno.Usted es " + player1CellType + ". Para elegir un espacio escriba '2NumeroDeEspacio', ej: 21,22,23,etc.";
				msg = *((message*)command.c_str());
				sendto(users[i2].listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&users[i2].client, sizeof(users[i2].client));

				i2 = users.size();
			}
		}

		for (int i2 = 0; i2 < users.size(); i2++)
		{
			if (users[i2].matchID == matchID && !users[i2].connectionLost && users[i2].isPlaying && users[i2].isPlayer2)
			{
				//IT IS NOT YOUR TURN (CMD 0)
				// DISPLAY PLAYER 1 DATA
				string command = "0Es turno de " + name + "(" + player1CellType + ")";

				msg = *((message*)command.c_str());
				sendto(users[i2].listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&users[i2].client, sizeof(users[i2].client));

				i2 = users.size();
			}
		}
	}
	else
	{
		//SAVE PLAYER2 ALIAS AND CELLTYPE.
		string player2CellType = "";
		string name;


		for (int i2 = 0; i2 < users.size(); i2++)
		{
			if (users[i2].matchID == matchID && !users[i2].connectionLost && users[i2].isPlaying && users[i2].isPlayer2)
			{
				//IT IS YOUR TURN (CMD 1)
				// SAVE PLAYER 2 DATA.
				string cellType(1, ttt.CellToChar(users[i2].cellType));
				player2CellType = cellType;
				name = users[i2].alias;

				string command = "1Es su turno.Usted es " + player2CellType + ". Para elegir un espacio escriba '2NumeroDeEspacio', ej: 21,22,23,etc.";
				msg = *((message*)command.c_str());
				sendto(users[i2].listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&users[i2].client, sizeof(users[i2].client));

				i2 = users.size();
			}
		}

		for (int i2 = 0; i2 < users.size(); i2++)
		{
			if (users[i2].matchID == matchID && !users[i2].connectionLost && users[i2].isPlaying && users[i2].isPlayer1)
			{
				//IT IS NOT YOUR TURN (CMD 0)
				// DISPLAY PLAYER 2 DATA
				string command = "0Es turno de " + name + "(" + player2CellType + ")";

				msg = *((message*)command.c_str());
				sendto(users[i2].listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&users[i2].client, sizeof(users[i2].client));

				i2 = users.size();
			}
		}
	}
}


void main()
{
	TaTeTi ttt;
	vector<Match> matches;
	vector<User> users;

	// iniciar winsock
	WSADATA data;
	int wsOk = sockInit();
#ifdef _WIN32
	if (wsOk != 0)
	{
		cerr << " No pudo iniciar winsock" << endl;
	}
#else
	return 0;
#endif
	

#ifdef WIN32
	SOCKET listening = socket(AF_INET, SOCK_DGRAM, 0);
#else
	int listening = socket(AF_INET, SOCK_DGRAM, 0);
#endif

	// crear un listen socket 
	//SOCKET listening = socket(AF_INET, SOCK_DGRAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "invalid socket " << endl;

	}

	string ipToUse = "";
	int portToUse = 0;

	cout << "Escribe la ip para el servidor: ";
	cin >> ipToUse;
	cout << endl;
	cout << "Escribe el puerto para el servidor: ";
	cin >> portToUse;
	cout << endl;

	// bind el socket ( seleccionar/pegar el socket a una dupla ip:puerto / direccion de socket)
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(portToUse);
	//hint.sin_addr.S_un.S_addr = ADDR_ANY; // es lo mismo que abajo
	inet_pton(AF_INET, ipToUse.c_str(), &hint.sin_addr); // direccion ip destino	

	int bindResult = bind(listening, (sockaddr*)&hint, sizeof(hint));

	if (bindResult == SOCKET_ERROR)
	{
		cerr << "no se pudo hacer el bind " << endl;
		// error
	}

	// recibir data del socket, y procesarla (proceso bloqueante) 

	char buf[1024];
	message msgRcd;
	int clientIndex = 0;
	message msg;

	// estructura con la data del cliente que nos esta enviando mensaje
	sockaddr_in client;
	int clientSize = sizeof(client);

	//ZeroMemory(buf, sizeof(buf)); //recorre el buffer y lo llena de ceros
	memset(buf, 0, sizeof(buf)); // es lo mismo que arriba pero ZeroMemory es solo de windows 
	memset(&msgRcd, 0, sizeof(msgRcd));
	memset(&msg, 0, sizeof(msg));

	while (buf != "\n")
	{
		memset(buf, 0, sizeof(buf));
		memset(&msgRcd, 0, sizeof(msgRcd));
		memset(&msg, 0, sizeof(msg));

		// funcion bloqueante
		//int bytesIn = recvfrom(listening, buf, sizeof(buf), 0, (sockaddr*)&client, &clientSize);
		int bytesIn = recvfrom(listening, (char*)&msgRcd, sizeof(msgRcd), 0, (sockaddr*)&client, &clientSize);

		char ip[1024];
		unsigned short port = client.sin_port;

		inet_ntop(AF_INET, &client.sin_addr, ip, sizeof(ip));

		if (bytesIn == SOCKET_ERROR)
		{
			cerr << "Error al recibir data" << endl;
		}

		switch (msgRcd.cmd)
		{
		case '0':
		{
			bool found = false;
			for (int i = 0; i < users.size(); i++)
			{
				if ((users[i].client.sin_addr.s_addr == client.sin_addr.s_addr) && (users[i].client.sin_port == client.sin_port))
				{
					found = true;
					i = users.size();
				}
			}

			if (found)
			{
				msg = *((message*)"1Usuario ya registrado, eliga otro comando del 0 al 4!.");
				sendto(listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&client, sizeof(client));
			}
			else
			{
				User u;
				u.client = client;
				u.listening = listening;
				u.connectionLost = false;
				users.push_back(u);
				cout << ip << ":" << port << " se ha conectado al servidor" << endl;
				msg = *((message*)"1Registro exitoso, escriba 1NombreDeJugador para elegir un alias.");
				sendto(listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&client, sizeof(client));
			}
		}
		break;
		case '1':
		{
			bool found = false;
			int index = -1;
			for (int i = 0; i < users.size(); i++)
			{
				if ((users[i].client.sin_addr.s_addr == client.sin_addr.s_addr) && (users[i].client.sin_port == client.sin_port))
				{
					strcpy_s(users[i].alias, msgRcd.data);
					cout << endl;
					found = true;
					index = i;
					i = users.size();
				}
			}

			if (!found)
			{
				msg = *((message*)"1Para poner un alias usted debera unirse a la sala de espera escribiendo '0'.");
				sendto(listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&client, sizeof(client));
			}
			else
			{
				msg = *((message*)"1Alias creado con exito, para entrar a la sala de espera y jugar escriba 3.");
				sendto(listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&client, sizeof(client));

				cout << ip << ":" << port << " ha cambiado su alias a " << users[index].alias << "." << endl;
			}
		}
		break;
		case '2':
		{
			// Hacer jugada
			bool found = false;
			int index = 0;
			for (int i = 0; i < users.size(); i++)
			{
				if ((users[i].client.sin_addr.s_addr == client.sin_addr.s_addr) && (users[i].client.sin_port == client.sin_port))
				{
					if (users[i].isPlaying && !users[i].wantsToPlayAgain)
					{
						found = true;
						index = i;
						i = users.size();
					}
				}
			}

			if (found)
			{
				int playerInput = (int)msgRcd.data[0] - '0';
				int matchID = users[index].matchID;

				if (ttt.MakeMove(matches[matchID], users[index], playerInput))
				{
					ShowBoard(users, matches,ttt,matchID, msg);

					if (ttt.CheckAll(matches[matchID]))
					{
						// A player wins
						//-----------------
						string winnerCellType(1, ttt.CellToChar(users[index].cellType));
						string winnerName = users[index].alias;


						string command = "1Ganaste! Si queres jugar de nuevo escribe 3, si no escribe 4";
						msg = *((message*)command.c_str());
						sendto(listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&client, sizeof(client));

						users[index].isPlaying = false;
						users[index].wantsToPlayAgain = false;

						if (users[index].isPlayer1) // if player1 Won
						{
							for (int i2 = 0; i2 < users.size(); i2++)
							{
								if (users[i2].matchID == matchID && !users[i2].connectionLost && users[i2].isPlayer2)
								{
									string command = "1" + winnerName + "(" + winnerCellType + ")" + " Gano! Si queres jugar de nuevo escribe 3, si no escribe 4";

									msg = *((message*)command.c_str());
									sendto(users[i2].listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&users[i2].client, sizeof(users[i2].client));

									users[i2].isPlaying = false;
									users[i2].wantsToPlayAgain = false;

									i2 = users.size();
								}
							}
						}
						else if (users[index].isPlayer2) // if player2 Won
						{
							for (int i2 = 0; i2 < users.size(); i2++)
							{
								if (users[i2].matchID == matchID && !users[i2].connectionLost && users[i2].isPlayer1)
								{
									string command = "1" + winnerName + "(" + winnerCellType + ")" + " Gano! Si queres jugar de nuevo escribe 3, si no escribe 4";

									msg = *((message*)command.c_str());
									sendto(users[i2].listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&users[i2].client, sizeof(users[i2].client));

									users[i2].isPlaying = false;
									users[i2].wantsToPlayAgain = false;

									i2 = users.size();
								}
							}
						}

						cout << "Una partida ha finalizado. ID : " << matchID << endl;

						matches[matchID].isInUse = false;

					}
					else if (matches[matchID].turnsLeft == 0)
					{
						// Tie
						//-----------------

						int playersFound = 0;

						for (int i2 = 0; i2 < users.size(); i2++)
						{
							if (playersFound < 2)
							{
								if (users[i2].matchID == matchID && !users[i2].connectionLost && (users[i2].isPlayer1 || users[i2].isPlayer2))
								{
									//SEND TIE MESSAGE
									string command = "1Empate! Si queres jugar de nuevo escribe 3, si no escribe 4";

									msg = *((message*)command.c_str());
									sendto(users[i2].listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&users[i2].client, sizeof(users[i2].client));

									users[i2].isPlaying = false;
									users[i2].wantsToPlayAgain = false;

									playersFound++;
								}
							}
							else
							{
								i2 = users.size();
							}
						}

						matches[matchID].isInUse = false;
					}
					else
					{
						// Next Turn
						NextTurn(users, matches, ttt, matchID, msg);
					}
				}
				else
				{
					msg = *((message*)"1Espacio invalido o ya elegido, seleccione otro: ");
					sendto(listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&client, sizeof(client));
				}

			}
			else
			{
				msg = *((message*)"1Usuario no encontrado, revise si se ha conectado escribiendo '0'.");
				sendto(listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&client, sizeof(client));
			}
		}
			break;
		case '3':
		{
			bool found = false;
			int index = 0;
			int usersNotPlaying = 0;
			for (int i = 0; i < users.size(); i++)
			{
				if ((users[i].client.sin_addr.s_addr == client.sin_addr.s_addr) && (users[i].client.sin_port == client.sin_port))
				{
					users[i].wantsToPlayAgain = true;

					if (!users[i].isPlaying)
					{
						users[i].isPlaying = false;
						found = true;
						index = i;
						i = users.size();
					}

				}
			}

			for (int i = 0; i < users.size(); i++)
			{
				if (users[i].wantsToPlayAgain)
				{
					usersNotPlaying++;
				}
			}

			if (!found)
			{
				msg = *((message*)"1Para poder jugar usted debera elegir un alias escribiendo 1NombreDeJugador o terminar la partida actual.");
				sendto(listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&client, sizeof(client));
			}
			else
			{
				if (usersNotPlaying % 2 == 0 && usersNotPlaying != 0)
				{
					if (TryStartGame(matches, ttt, users))
					{
						int matchID = users[index].matchID;
						User* users2 = new User[PLAYER_SIZE];

						users2 = GetPlayerData(users, matches, ttt, matchID);

						char ip01[1024];
						unsigned short port01 = users2[0].client.sin_port;
						inet_ntop(AF_INET, &users2[0].client.sin_addr, ip01, sizeof(ip01));
						char ip02[1024];
						unsigned short port02 = users2[1].client.sin_port;
						inet_ntop(AF_INET, &users2[1].client.sin_addr, ip02, sizeof(ip02));

						string name = users2[1].alias;
						string text = "0Comenzando partida, usted se enfrentara a " + name;

						msg = *((message*)text.c_str());
						sendto(users2[0].listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&users2[0].client, sizeof(users2[0].client));

						name = users2[0].alias;
						text = "0Comenzando partida, usted se enfrentara a " + name;

						msg = *((message*)text.c_str());
						sendto(users2[1].listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&users2[1].client, sizeof(users2[1].client));


						cout << "Partida creada. ID : " << matchID << ", Jugadores : " << users2[0].alias << "(" << ip01 << ":" << port01 << ")" << " y " << users2[1].alias << "(" << ip02 << ":" << port02 << ")" << endl;
						delete[] users2;

						int randomValue = irand(0,1);
						ttt.SetNextPlayer(matches[matchID], (bool)randomValue);
						ShowBoard(users, matches, ttt, matchID, msg);
						NextTurn(users, matches, ttt, matchID, msg);
					}
					else
					{
						msg = *((message*)"0No hay suficientes jugadores para empezar la partida,por favor espere...");
						sendto(listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&client, sizeof(client));

						cout << users[index].alias << "(" << ip << ":" << port << ") ha entrado a la sala de espera." << endl;
					}
				}
				else
				{
					msg = *((message*)"0No hay suficientes jugadores para empezar la partida,por favor espere...");
					sendto(listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&client, sizeof(client));

					cout << users[index].alias << "(" << ip << ":" << port << ") ha entrado a la sala de espera." << endl;
				}
			}
		}
		break;
		case '4':
		{
			bool found = false;
			int index = -1;
			for (int i = 0; i < users.size(); i++)
			{
				if ((users[i].client.sin_addr.s_addr == client.sin_addr.s_addr) && (users[i].client.sin_port == client.sin_port))
				{
					users[i].connectionLost = true;
					users[i].wantsToPlayAgain = false;
					found = true;
					index = i;
					i = users.size();
				}
			}

			if (found)
			{
				msg = *((message*)"4");
				sendto(listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&client, sizeof(client));
				int matchID = users[index].matchID;

				cout << users[index].alias << "(" << ip << ":" << port << ") ha salido del server." << endl;

				//CANCEL CURRENT MATCH IF HE WAS IN ONE
				//--------------------------------------

				if (users[index].isPlaying)
				{
					int playersFound = 0;

					for (int i2 = 0; i2 < users.size(); i2++)
					{
						if (playersFound < 1)
						{
							if (users[i2].matchID == matchID && !users[i2].connectionLost && (users[i2].isPlayer1 || users[i2].isPlayer2))
							{
								//SEND TIE MESSAGE
								string command = "1Tu oponente ha salido de la partida, si queres volver a jugar escribi 3, si queres salir escribi 4.";

								msg = *((message*)command.c_str());
								sendto(users[i2].listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&users[i2].client, sizeof(users[i2].client));

								users[i2].isPlaying = false;
								users[i2].wantsToPlayAgain = false;

								playersFound++;
							}
						}
						else
						{
							i2 = users.size();
						}
					}

					matches[matchID].isInUse = false;
				}
			}
		}
		break;
		default:
			cout << "byte invalido " << msgRcd.cmd << endl;
			msg = *((message*)"1Byte invalido, seleccione una opcion del 0 al 4.");
			sendto(listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&client, sizeof(client));
			break;
		}
	}

	// destruir el socket
	closesocket(listening);

	// cleanup winsock
	sockQuit();
}
