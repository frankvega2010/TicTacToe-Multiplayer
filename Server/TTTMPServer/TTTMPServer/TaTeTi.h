#pragma once

#ifdef _WIN32
// taken from https://stackoverflow.com/questions/28027937/cross-platform-sockets
/* See http://stackoverflow.com/questions/12765743/getaddrinfo-on-win32 */
/*#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501  // Windows XP. 
#endif*/
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#else
/* Assume that any non-Windows platform uses POSIX-style sockets instead. */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <unistd.h> /* Needed for close() */
typedef int SOCKET;
#endif

#include <iostream>
#include <vector>
#include <string>

using namespace std;

#define PLAYER_SIZE 2

enum Cell
{
	emptyCell, dot, cross
};

struct User
{
	sockaddr_in client;
	SOCKET listening;
	char alias[255];
	bool isPlaying = false;
	bool isPlayer1 = false;
	bool isPlayer2 = false;
	bool wantsToPlayAgain = false;
	int matchID;
	Cell cellType;
	bool connectionLost = false;
};

struct Match
{
	int ID;
	Cell cellArray[9];
	bool isInUse = false;
	bool nextPlayer = true;
	bool player1HasEntered = false;
	bool player2HasEntered = false;
	int turnsLeft = 8;
	string currentGameBoard;
};


class TaTeTi
{
private:	

public:
	TaTeTi();
	
	bool CheckVertical(Match& match);
	bool CheckHorizontal(Match& match);
	bool CheckCrosses(Match& match);
	bool CheckAll(Match& match);
	bool CheckInput(Match& match, int input);
	char CellToChar(Cell input);
	bool MakeMove(Match& match, User player, int cell);
	void SetNextPlayer(Match& match, bool state);
	void SetIsInUse(Match& match, bool isInUse);
	void SetTurns(Match& match, int turns);
	void StartGame(Match& match);
	void NextPlayer(Match& match);
	void RestartCells(Match& match);
	string GetCurrentGameBoard(Match& match);
};