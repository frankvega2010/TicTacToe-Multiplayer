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

/*struct Gamestate
{
	Cell cellArray[9];
};*/

/*struct Node
{
	Gamestate state;
	int value;
	//vector<Node>Childs;
};*/

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
	
	//Gamestate currentGamestate;
	

	/*User players[PLAYER_SIZE];
	User* firstPlayer = nullptr;
	User* secondPlayer = nullptr;*/
	

public:
	TaTeTi();
	
	bool CheckVertical(Match& match);
	bool CheckHorizontal(Match& match);
	bool CheckCrosses(Match& match);
	bool CheckAll(Match& match);
	bool CheckInput(Match& match, int input);
	char CellToChar(Cell input);
	bool MakeMove(Match& match, User player, int cell);
	//int GetNextPlayerIndex(Match& match);
	//void SetNextPlayerIndex(Match& match, int index);
	void SetNextPlayer(Match& match, bool state);
	void SetIsInUse(Match& match, bool isInUse);
	void SetTurns(Match& match, int turns);
	//bool MakeMove(struct User* player, int cell);
	/////bool GetGameEnded() { return gameEnded; }
	//void AddPlayer(User* &newPlayer);
	//bool CanRecievePlayer();
	//void SetupPlayers(User* &first, User* &second);
	void StartGameLocal();
	void StartGame(Match& match);
	void NextPlayer(Match& match);
	//Match GetMatch(Match& match);
	void RestartCells(Match& match);
	string GetCurrentGameBoard(Match& match);
};