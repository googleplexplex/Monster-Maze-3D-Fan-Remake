#pragma once
#include <windows.h>
#include <time.h>
#include <math.h>
#include <atomic>
#include "map.hpp"
#include "output.hpp"
#include "direction.hpp"
POINT GetTextExtentPoint32Size(const char* str);
#define pointsEqual(f, s) (((f).x == (s).x) && ((f).y == (s).y))
#define sqr(x) ((x)*(x))


typedef enum playerBlock
{
	wasnot = 0,
	was = 1
};
playerBlock playerMap[mapSize.x][mapSize.y];
class playerClass
{
public:
	POINT pos;
	direction viewDirection = E;
	void moveForward()
	{
		POINT goTo = pos;
		switch (viewDirection)
		{
		case N: goTo.y--; break;
		case W: goTo.x--; break;
		case E: goTo.x++; break;
		case S: goTo.y++; break;
		}

		if (gameMap[goTo.x][goTo.y] != wall)
		{
			pos = goTo;
			playerMap[goTo.x][goTo.y] = was;

			if (gameMap[goTo.x][goTo.y] == door)
				goToPage(winPage);
		}

		refreshCanvas();
	}
	void inline turnLeft()
	{
		viewDirection = turnDirectionLeft(viewDirection);

		refreshCanvas();
	}
	void inline turnRight()
	{
		viewDirection = turnDirectionRight(viewDirection);

		refreshCanvas();
	}
	void inline turnAround()
	{
		viewDirection = turnDirectionAround(viewDirection);

		refreshCanvas();
	}
}player;

constexpr POINT compasPos = { 520, 20 };
constexpr POINT compasFontSize = { 8, 16 };
constexpr POINT compasPosIndentation = { 20, 20 };
#define relativeRectangle(x, y, xsize, ysize) Rectangle(mainWindowHDC, x, y, x + xsize, y + ysize)
#define relativeRectangle(x, y, xsize, ysize) Rectangle(mainWindowHDC, x, y, x + xsize, y + ysize)
LPSTR toString(char charset)
{
	LPSTR result = new char[2];
	result[0] = charset;
	result[1] = NULL;
	return result;
}
void showCompas() //TODO
{
	HBRUSH oldBrush = (HBRUSH)SelectObject(mainWindowHDC, (HBRUSH)CreateSolidBrush(RGB(255, 255, 255)));

	SetTextColor(mainWindowHDC, RGB(255, 255, 255));
	SetBkColor(mainWindowHDC, RGB(0, 0, 0));

	relativeRectangle(compasPos.x + (0 * compasPosIndentation.x), compasPos.y + (0 * compasPosIndentation.y), compasFontSize.x, compasFontSize.y);
	TextOut(mainWindowHDC, compasPos.x + (1 * compasPosIndentation.x), compasPos.y + (0 * compasPosIndentation.y), toString(directionName(player.viewDirection)), 1);
	relativeRectangle(compasPos.x + (2 * compasPosIndentation.x), compasPos.y + (0 * compasPosIndentation.y), compasFontSize.x, compasFontSize.y);

	TextOut(mainWindowHDC, compasPos.x + (0 * compasPosIndentation.x), compasPos.y + (1 * compasPosIndentation.y), toString(directionName(turnDirectionLeft(player.viewDirection))), 1);
	relativeRectangle(compasPos.x + (1 * compasPosIndentation.x), compasPos.y + (1 * compasPosIndentation.y), compasFontSize.x, compasFontSize.y);
	TextOut(mainWindowHDC, compasPos.x + (2 * compasPosIndentation.x), compasPos.y + (1 * compasPosIndentation.y), toString(directionName(turnDirectionRight(player.viewDirection))), 1);

	relativeRectangle(compasPos.x + (0 * compasPosIndentation.x), compasPos.y + (2 * compasPosIndentation.y), compasFontSize.x, compasFontSize.y);
	TextOut(mainWindowHDC, compasPos.x + (1 * compasPosIndentation.x), compasPos.y + (2 * compasPosIndentation.y), toString(directionName(turnDirectionAround(player.viewDirection))), 1);
	relativeRectangle(compasPos.x + (2 * compasPosIndentation.x), compasPos.y + (2 * compasPosIndentation.y), compasFontSize.x, compasFontSize.y);

	SelectObject(mainWindowHDC, oldBrush);
}
double inline getRangeBehind(POINT f, POINT s)
{
	return sqrt(sqr(s.x - f.x) + sqr(s.y - f.y));
}

class monsterClass
{
public:
	POINT pos;
	direction target = null;
	unsigned short targetRange = 0;
	const unsigned int speed = 1000;
	bool seeThePlayer = false;
	std::pair<direction, unsigned short> seesPlayer() //return null, if false, direction if true
	{
		unsigned short rangeToPlayer = 1;
		if (player.pos.x == monster.pos.x)
		{
			if (player.pos.y > monster.pos.y)
			{
				for (; monster.pos.y + rangeToPlayer < player.pos.y; rangeToPlayer++)
				{
					if (gameMap[monster.pos.x][monster.pos.y + rangeToPlayer] == wall)
						return { null, NULL };
				}
				return { S, rangeToPlayer };
			}
			else {
				for (; monster.pos.y - rangeToPlayer > player.pos.y; rangeToPlayer++)
				{
					if (gameMap[monster.pos.x][monster.pos.y - rangeToPlayer] == wall)
						return { null, NULL };
				}
				return { N, rangeToPlayer };
			}
		}
		else if (player.pos.y == monster.pos.y)
		{
			if (player.pos.x > monster.pos.x)
			{
				for (; monster.pos.x + rangeToPlayer < player.pos.x; rangeToPlayer++)
				{
					if (gameMap[monster.pos.x + rangeToPlayer][monster.pos.y] == wall)
						return { null, NULL };
				}
				return { E, rangeToPlayer };
			}
			else {
				for (; monster.pos.x - rangeToPlayer > player.pos.x; rangeToPlayer++)
				{
					if (gameMap[monster.pos.x - rangeToPlayer][monster.pos.y] == wall)
						return { null, NULL };
				}
				return { W, rangeToPlayer };
			}
		}
		return { null, NULL };
	}
	bool inline monsterInTarget()
	{
		if (seeThePlayer)
			seeThePlayer = false;
		return (targetRange == 0);
	}

	struct passDescriptionStruct
	{
		bool isPass = false;
		unsigned short* ranges;
		unsigned short rangesCount = 0;
		void addRange(unsigned short addedRange)
		{
			unsigned short* temp = ranges;
			ranges = new unsigned short[rangesCount + 1];
			memcpy(ranges, temp, rangesCount * sizeof(unsigned short));
			ranges[rangesCount++] = addedRange;
		}
		void fill(POINT scannedPos, direction scannedDirection)
		{
			unsigned short range = 1;
			switch (scannedDirection)
			{
			case N:
				while (gameMap[scannedPos.x][scannedPos.y - range] != wall)
				{
					if (gameMap[scannedPos.x - 1][scannedPos.y - range] == none || gameMap[scannedPos.x + 1][scannedPos.y - range] == none)
					{
						isPass = true;
						addRange(range);
					}
					range++;
				}
				return;
			case W:
				while (gameMap[scannedPos.x - range][scannedPos.y] != wall)
				{
					if (gameMap[scannedPos.x - range][scannedPos.y - 1] == none || gameMap[scannedPos.x - range][scannedPos.y + 1] == none)
					{
						isPass = true;
						addRange(range);
					}
					range++;
				}
				return;
			case E:
				while (gameMap[scannedPos.x + range][scannedPos.y] != wall)
				{
					if (gameMap[scannedPos.x + range][scannedPos.y - 1] == none || gameMap[scannedPos.x + range][scannedPos.y + 1] == none)
					{
						isPass = true;
						addRange(range);
					}
					range++;
				}
				return;
			case S:
				while (gameMap[scannedPos.x][scannedPos.y + range] != wall)
				{
					if (gameMap[scannedPos.x - 1][scannedPos.y + range] == none || gameMap[scannedPos.x + 1][scannedPos.y + range] == none)
					{
						isPass = true;
						addRange(range);
					}
					range++;
				}
				return;
			}
		}
	};
	struct positionInfoStruct
	{
		passDescriptionStruct passDescriptions[4];
		positionInfoStruct(POINT scannedPoint)
		{
			for (int i = 0; i < 4; i++)
			{
				passDescriptions[i].fill(scannedPoint, (direction)i);
			}
		}
		unsigned short sumOfPasses()
		{
			unsigned short res = 0;
			for (int i = 0; i < 4; i++)
			{
				if (passDescriptions[i].isPass)
					res++;
			}
			return res;
		}
		int getFirstPass()
		{
			for (int i = 0; i < 4; i++)
			{
				if (passDescriptions[i].isPass)
					return i;
			}
		}
		int getRandomPass()
		{
			rerandom:
			for (int i = 0; i < 4; i++)
			{
				if (passDescriptions[i].isPass && bool(rand() % 2))
					return i;
			}
			goto rerandom;
		}
	};
	void setRandomTarget()
	{
		positionInfoStruct positionInfo(monster.pos); //�������� ��� ���������� � ��������� �������

		if (positionInfo.sumOfPasses() == 1 && positionInfo.passDescriptions[(int)turnDirectionAround(target)].isPass) //���� ����� ������ �� ��������� �� �����, � ������ ����� ���...
		{
			target = turnDirectionAround(target); //���������������
			passDescriptionStruct wayBackDescription = positionInfo.passDescriptions[(int)target];
			targetRange = wayBackDescription.ranges[rand() % wayBackDescription.rangesCount];
			return;
		}

		//���� �����, �������� ��������, ���������...
		positionInfo.passDescriptions[(int)turnDirectionAround(target)].isPass = false;
		if (positionInfo.sumOfPasses() == 1) //���� �����, �� �������� ��������, ������ ����...
		{
			target = (direction)positionInfo.getFirstPass(); //��� � ����
			passDescriptionStruct turnDescription = positionInfo.passDescriptions[(int)target];
			targetRange = turnDescription.ranges[rand() % turnDescription.rangesCount];
			return;
		}

		//���� �����, �� �������� ��������, ���������...
		target = (direction)positionInfo.getRandomPass(); //��� � ��������� �� ���
		passDescriptionStruct turnDescription = positionInfo.passDescriptions[(int)target];
		targetRange = turnDescription.ranges[rand() % turnDescription.rangesCount];
	}
	void moveToTarget()
	{
		switch (target)
		{
		case N: pos.y--; break;
		case W: pos.x--; break;
		case E: pos.x++; break;
		case S: pos.y++; break;
		}
		targetRange--;
	}
	void alifeTick()
	{
		std::pair<direction, unsigned short> seePlayer = seesPlayer();

		if (seePlayer.first != null)
		{
			seeThePlayer = true;
			target = seePlayer.first;
			targetRange = seePlayer.second;
		}
		else if (monsterInTarget())
			setRandomTarget();
		
		moveToTarget();
	}
	bool inline catchPlayer()
	{
		return (pointsEqual(player.pos, monster.pos));
	}
	void setFirstTarget()
	{
		positionInfoStruct positionInfo(monster.pos); //�������� ��� ���������� � ��������� �������

		if (positionInfo.sumOfPasses() == 1) //���� ���� �� �������� �����, ��� ������ ��������, ������ ����...
		{
			target = (direction)positionInfo.getFirstPass(); //��� � ����
			passDescriptionStruct turnDescription = positionInfo.passDescriptions[(int)target];
			targetRange = turnDescription.ranges[rand() % turnDescription.rangesCount];
			return;
		}

		//���� ����� ����� ���������...
		target = (direction)positionInfo.getRandomPass(); //��� � ��������� �� ���
		passDescriptionStruct turnDescription = positionInfo.passDescriptions[(int)target];
		targetRange = turnDescription.ranges[rand() % turnDescription.rangesCount];
	}
}monster;
constexpr const char* monsterStatuses[4] = { "RUN", "I hear steps...", "He is close", "He is far" };
void showStatus()
{
	if (monster.seesPlayer().first != null)
	{
		TextOutA(mainWindowHDC,
			screenSize.y / 2 - (GetTextExtentPoint32Size(monsterStatuses[0]).x) / 2,
			screenSize.y - GetTextExtentPoint32Size(monsterStatuses[0]).y - 30,
			monsterStatuses[0], strlen(monsterStatuses[0]));
		return;
	}

	unsigned int rangeBehingPlayerAndMonster = getRangeBehind(player.pos, monster.pos);
	if (rangeBehingPlayerAndMonster <= 5) {
		TextOutCenterGame(mainWindowHDC, screenSize.y - GetTextExtentPoint32Size(monsterStatuses[1]).y - 30, monsterStatuses[1]);
	}
	else if (rangeBehingPlayerAndMonster <= 10) {
		TextOutCenterGame(mainWindowHDC, screenSize.y - GetTextExtentPoint32Size(monsterStatuses[2]).y - 30, monsterStatuses[2]);
	}
	else {
		TextOutCenterGame(mainWindowHDC, screenSize.y - GetTextExtentPoint32Size(monsterStatuses[3]).y - 30, monsterStatuses[3]);
	}
}

//TOLIB
void inline clearPlayerMap()
{
	for (int i = 0; i < mapSize.x; i++)
	{
		for (int j = 0; j < mapSize.y; j++)
		{
			playerMap[i][j] = wasnot;
		}
	}
}
void inline updatePlayerMap(POINT updatedPoint)
{
	if (playerMap[updatedPoint.x][updatedPoint.y] == wasnot)
		playerMap[updatedPoint.x][updatedPoint.y] = was;
}
const POINT playerMapBlockSize = { 12, 12 };
const POINT playerMapPosOffset = { 15, 20 };
const POINT playerMapPos = { screenSize.x - mapSize.x * playerMapBlockSize.x - playerMapPosOffset.x, screenSize.y - mapSize.y * playerMapBlockSize.y - playerMapPosOffset.y };
const COLORREF playerMapBlockColor = RGB(255, 255, 255);
const COLORREF playerMapColor = RGB(0, 255, 0);
const COLORREF monsterMapColor = RGB(255, 0, 0);
void inline showPlayerBlock(int x, int y)
{
	POINT posBlockOnScreen = { playerMapPos.x + playerMapBlockSize.x * x, playerMapPos.y + playerMapBlockSize.y * y };
	Rectangle(mainWindowHDC, posBlockOnScreen.x, posBlockOnScreen.y, posBlockOnScreen.x + playerMapBlockSize.x, posBlockOnScreen.y + playerMapBlockSize.y);
}
void showPlayerMap()
{
	HBRUSH oldBrush = (HBRUSH)SelectObject(mainWindowHDC, (HBRUSH)CreateSolidBrush(playerMapBlockColor));
	for (int i = 0; i < mapSize.y; i++)
	{
		for (int j = 0; j < mapSize.x; j++)
		{
			if (playerMap[j][i] == was)
			{
				showPlayerBlock(j, i);
			}
		}
	}
	SelectObject(mainWindowHDC, oldBrush);

	oldBrush = (HBRUSH)SelectObject(mainWindowHDC, (HBRUSH)CreateSolidBrush(playerMapColor));
	showPlayerBlock(player.pos.x, player.pos.y);
	SelectObject(mainWindowHDC, oldBrush);

	if (usedCheats || monster.seeThePlayer)
	{
		oldBrush = (HBRUSH)SelectObject(mainWindowHDC, (HBRUSH)CreateSolidBrush(monsterMapColor));
		showPlayerBlock(monster.pos.x, monster.pos.y);
		SelectObject(mainWindowHDC, oldBrush);
	}
}

block getObject(int x, int y)
{
	if (monster.pos.x == x && monster.pos.y == y)
		return monsterUM;
	return gameMap[x][y];
}
block getObject(POINT gettedPoint)
{
	return getObject(gettedPoint.x, gettedPoint.y);
}
void showObject(block obj, short range, sidesEnum side)
{
	if (obj == wall)
		showWall(range, side);
	else if (obj == none)
		showNone(range, side);
	else if (obj == door)
		showDoor(range, side);
	else if (obj == monsterUM)
		showMonster(range, side);
}

void showGameCanvas()
{
	int rangeViewedPass = 0;
	POINT viewPoint = player.pos;

	while(getObject(viewPoint) == none && rangeViewedPass < 6) //����������� ���� �������� �����
	{
		updatePlayerMap(viewPoint);

		POINT viewPointLeft = moveDirectionLeft(viewPoint, player.viewDirection);
		POINT viewPointRight = moveDirectionRight(viewPoint, player.viewDirection);
		showObject(getObject(viewPointLeft), rangeViewedPass, leftSide);
		showObject(getObject(viewPointRight), rangeViewedPass, rightSide);

		viewPoint = moveDirectionForward(viewPoint, player.viewDirection);
		rangeViewedPass++;
	}
	showObject(getObject(viewPoint), rangeViewedPass, frontSide);

	showPlayerMap(); //��������� ����-�����
	showCompas(); //����������� �������
	showStatus();
}



void generateGame()
{
	generateMap();
	gameMap[mapSize.x - 2][mapSize.y - 1] = door;

	player.pos.x = mapSize.x - 2;
	player.pos.y = mapSize.y - 2;
	clearPlayerMap();

	monster.pos = { mapSize.x / 2, mapSize.y / 2 };
	for (int i = mapSize.y / 2; i != 0; i--)
	{
		if (gameMap[monster.pos.x][i] == none)
		{
			monster.pos.y = i;
			break;
		}
	}
	for (int i = mapSize.x / 2; i != 0; i--)
	{
		if (gameMap[i][monster.pos.y] == none)
		{
			monster.pos.x = i;
			break;
		}
	}
	monster.setFirstTarget();
}

std::atomic_bool callGameTick = true;
void Game_Tick()
{
	refreshCanvas();
	srand(time(NULL));
	Sleep(monster.speed);

	monster.alifeTick();
	refreshCanvas();
	if (monster.catchPlayer())
		goToPage(losePage);

	callGameTick = true;
}