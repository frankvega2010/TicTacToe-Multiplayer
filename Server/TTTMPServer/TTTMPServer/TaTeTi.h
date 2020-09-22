#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

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