#pragma once
#include <windows.h>
#include <time.h>

typedef enum block {
	none = 0,
	wall,
	monsterUM, //UM - UnMapped, ����������, �� �� ��������� �� gameMap, ��� ��� ����� ��������� �����
	door
};
constexpr POINT mapSize = { 25, 25 };
block gameMap[mapSize.x][mapSize.y];
constexpr POINT doorPos = { mapSize.x - 2, mapSize.y - 1 };

bool deadEnd(int x, int y) {
	int a = 0;

	if (x != 1) {
		if (gameMap[y][x - 2] == none)
			a += 1;
	}
	else a += 1;

	if (y != 1) {
		if (gameMap[y - 2][x] == none)
			a += 1;
	}
	else a += 1;

	if (x != mapSize.x - 2) {
		if (gameMap[y][x + 2] == none)
			a += 1;
	}
	else a += 1;

	if (y != mapSize.y - 2) {
		if (gameMap[y + 2][x] == none)
			a += 1;
	}
	else a += 1;

	if (a == 4)
		return 1;
	else
		return 0;
}
void generateMap()
{
	int x, y, c, a;
	bool b;

	for (int i = 0; i < mapSize.y; i++) // ������ ����������� ������-��������
		for (int j = 0; j < mapSize.x; j++)
			gameMap[i][j] = wall;

	x = 3; y = 3; a = 0; // ����� ����������� ����� � �������
	while (a < 10000) { // ��, ��������, �������, ����� ���� ���, �� ����
		gameMap[y][x] = none; a++;
		while (1) { // ����������� ����, ������� ����������� ������ �������
			c = rand() % 4; // ���������, ��� ���� ���������
			switch (c) {  // �� ��� ������ � ����� ����������� �� ������
			case 0: if (y != 1)
				if (gameMap[y - 2][x] == wall) { // �����
					gameMap[y - 1][x] = none;
					gameMap[y - 2][x] = none;
					y -= 2;
				}
			case 1: if (y != mapSize.y - 2)
				if (gameMap[y + 2][x] == wall) { // ����
					gameMap[y + 1][x] = none;
					gameMap[y + 2][x] = none;
					y += 2;
				}
			case 2: if (x != 1)
				if (gameMap[y][x - 2] == wall) { // ������
					gameMap[y][x - 1] = none;
					gameMap[y][x - 2] = none;
					x -= 2;
				}
			case 3: if (x != mapSize.x - 2)
				if (gameMap[y][x + 2] == wall) { // �������
					gameMap[y][x + 1] = none;
					gameMap[y][x + 2] = none;
					x += 2;
				}
			}
			if (deadEnd(x, y))
				break;
		}

		if (deadEnd(x, y)) // ����������� ����� �� ������
			do {
				x = 2 * (rand() % ((mapSize.x - 1) / 2)) + 1;
				y = 2 * (rand() % ((mapSize.y - 1) / 2)) + 1;
			} while (gameMap[y][x] != none);
	} // �� ���� � ���.

	unsigned int holesCounter = rand() % 5 + 5;
	while(holesCounter > 0)
	{
		POINT randomPoint = { rand() % (mapSize.x - 2) + 1, rand() % (mapSize.y - 2) + 1 };
		if (gameMap[randomPoint.x][randomPoint.y] == wall && 
			((gameMap[randomPoint.x - 1][randomPoint.y] == wall && gameMap[randomPoint.x + 1][randomPoint.y] == wall) ||
				(gameMap[randomPoint.x][randomPoint.y - 1] == wall && gameMap[randomPoint.x][randomPoint.y + 1] == wall)
			))
		{
			gameMap[randomPoint.x][randomPoint.y] = none;
			holesCounter--;
		}
	}
}