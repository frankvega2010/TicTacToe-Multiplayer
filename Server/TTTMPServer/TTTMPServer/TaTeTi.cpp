#include "TaTeTi.h"

#include "iostream"
#include <vector>
#include <cmath>

using namespace std;

TaTeTi::TaTeTi()
{
	//int playerInput;
}

bool TaTeTi::CheckVertical(Match& match)
{
	if (match.cellArray[6] != emptyCell && match.cellArray[6] == match.cellArray[3] && match.cellArray[3] == match.cellArray[0])
		return true;
	else
		if (match.cellArray[7] != emptyCell && match.cellArray[7] == match.cellArray[4] && match.cellArray[4] == match.cellArray[1])
			return true;
		else
			if (match.cellArray[8] != emptyCell && match.cellArray[8] == match.cellArray[5] && match.cellArray[5] == match.cellArray[2])
				return true;
			else
				return false;
}

bool TaTeTi::CheckHorizontal(Match& match)
{
	if (match.cellArray[6] != emptyCell && match.cellArray[6] == match.cellArray[7] && match.cellArray[7] == match.cellArray[8])
		return true;
	else
		if (match.cellArray[3] != emptyCell && match.cellArray[3] == match.cellArray[4] && match.cellArray[4] == match.cellArray[5])
			return true;
		else
			if (match.cellArray[0] != emptyCell && match.cellArray[0] == match.cellArray[1] && match.cellArray[1] == match.cellArray[2])
				return true;
			else
				return false;
}

bool TaTeTi::CheckCrosses(Match& match)
{
	if (match.cellArray[6] != emptyCell && match.cellArray[6] == match.cellArray[4] && match.cellArray[4] == match.cellArray[2])
		return true;
	else
		if (match.cellArray[8] != emptyCell && match.cellArray[8] == match.cellArray[4] && match.cellArray[4] == match.cellArray[0])
			return true;
		else
			return false;
}

bool TaTeTi::CheckAll(Match& match)
{
	return CheckHorizontal(match) || CheckVertical(match) || CheckCrosses(match);
}

bool TaTeTi::CheckInput(Match& match, int input)
{
	if (input > 0 && input <= 9 && match.cellArray[input - 1] == emptyCell)
		return true;
	else
		return false;
}

/*bool TaTeTi::CheckAll(int matchID)
{
	return false;
}*/

char TaTeTi::CellToChar(Cell input)
{
	char output;
	switch (input)
	{
	case emptyCell:
		output = '.';
		break;
	case dot:
		output = 'O';
		break;
	case cross:
		output = 'X';
		break;
	}
	return output;
}

bool TaTeTi::MakeMove(Match& match, User player, int cell)
{
	if (CheckInput(match, cell))
	{
		match.cellArray[cell - 1] = player.cellType;
		match.turnsLeft--;

		return true;
	}
	else
	{
		return false;
	}
}

/*int TaTeTi::GetNextPlayerIndex(vector<Match>& matches, int matchID)
{
	for (int i = 0; i < matches.size(); i++)
	{
		if (match.ID == matchID)
		{
			return match.nextPlayerIndex;
		}
	}
	return -1;
}*/

/*void TaTeTi::SetNextPlayerIndex(Match& match, int index)
{
	for (int i = 0; i < matches.size(); i++)
	{
		if (match.ID == matchID)
		{
			match.nextPlayerIndex = index;
			i = matches.size();
		}
	}
}*/

void TaTeTi::SetNextPlayer(Match& match, bool state)
{
	match.nextPlayer = state;
}

void TaTeTi::SetIsInUse(Match& match, bool isInUse)
{
	match.isInUse = isInUse;
}

void TaTeTi::SetTurns(Match& match, int turns)
{
	match.turnsLeft = turns;
}

/*bool TaTeTi::MakeMove(User* player, int cell)
{
	if (CheckInput(cell, currentGamestate))
	{
		currentGamestate.cellArray[cell - 1] = player->cellType;
		turnsLeft--;
		return true;
	}
	else
		return false;
}*/

/*void TaTeTi::AddPlayer(User* &newPlayer)
{
	if (firstPlayer)
	{
		secondPlayer = newPlayer;
		secondPlayer->currentRoom = this;
		SetupPlayers(firstPlayer, secondPlayer);
	}

	firstPlayer = newPlayer;
	firstPlayer->currentRoom = this;
}*/

/*bool TaTeTi::CanRecievePlayer()
{
	if (firstPlayer && secondPlayer)
		return false;

	return true;
}*/

/*void TaTeTi::SetupPlayers(User*& first, User*& second)
{
	first->cellType = dot;
	second->cellType = cross;
}*/

void TaTeTi::StartGameLocal()
{
	/*
	for (int i = 0; i < PLAYER_SIZE; i++)
	{
		players[i].cellType;
	}

	players[0].cellType = cross;
	players[1].cellType = dot;
	int playerInput;

	while (!gameEnded)
	{
		for (int i = 0; i < PLAYER_SIZE; i++)
		{
			cout << "player " << i+1 << " input number :" << endl;
			bool validInput = false;
			while (!validInput)
			{
				cin >> playerInput;
				validInput = CheckInput(playerInput, currentGamestate);
			}
			currentGamestate.cellArray[playerInput - 1] = players[i].cellType;

			for (int i = 2; i >= 0; i--)
			{
				for (int i2 = 0; i2 < 3; i2++)
				{
					cout << CellToChar(currentGamestate.cellArray[i * 3 + i2]) << "  ";
				}
				cout << endl;
			}

			if (CheckAll(currentGamestate) || turnsLeft == 0)
			{
				gameEnded = true;
				i = PLAYER_SIZE;
			}
				
			turnsLeft -= 1;
		}
		
	}
	*/
}

void TaTeTi::StartGame(Match& match)
{
	for (int f = 0; f < 9; f++)
	{
		match.cellArray[f] = emptyCell;
	}

	match.isInUse = true;
}

void TaTeTi::NextPlayer(Match& match)
{
	match.nextPlayer = !match.nextPlayer;
}

/*Match TaTeTi::GetMatch(vector<Match>& matches, int matchID)
{
	for (int i = 0; i < matches.size(); i++)
	{
		if (match.ID == matchID)
		{
			return match;
		}
	}

}*/

void TaTeTi::RestartCells(Match& match)
{
	for (int f = 0; f < 9; f++)
	{
		match.cellArray[f] = emptyCell;
	}

}

string TaTeTi::GetCurrentGameBoard(Match& match)
{
	match.currentGameBoard = "";
	match.currentGameBoard += '\n';
	for (int i2 = 2; i2 >= 0; i2--)
	{
		for (int i3 = 0; i3 < 3; i3++)
		{
			match.currentGameBoard += CellToChar(match.cellArray[i2 * 3 + i3]);
			match.currentGameBoard += ' ';
		}
		match.currentGameBoard += '\n';
	}

	return match.currentGameBoard;

	
}
