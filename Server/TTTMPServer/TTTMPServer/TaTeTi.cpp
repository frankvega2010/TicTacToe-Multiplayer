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
