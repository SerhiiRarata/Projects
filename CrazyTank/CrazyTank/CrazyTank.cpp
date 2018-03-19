// CrazyTank.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <Windows.h>

using namespace std;
const int width = 70;
const int height = 20;

char board[height][width];

struct Bullet{
	int iPos, jPos;
	int fireAngle;
	int isActive;
	int hostTankid;
};

struct Tank{
	int id;
	int angle;
	int life;
	int target_id;
	int iCoord, jCoord;
	int TankForm[3][3];
	Bullet bullets[100];
};

void gotoxy(int x, int y);
void textcolor(int color);
void initBattleBoard();
void drawBattleBoard();
void drawMapToScreen();
void clearBoard();
void resetMap();
void initPlayerTank(Tank *pTank, int id);
void drawPlayerTank(Tank tank);
void delPlayerTank(Tank tank);
void rotateTank90(Tank *pTank);
void goUp(Tank *pTank);
void goDown(Tank *pTank);
void goLeft(Tank *pTank);
void goRight(Tank *pTank);
void initBulletArr(Tank *pTank);
void initBullet(Bullet *pBullet, Tank tank);
void drawBullet(Bullet b, int hostId);
void delBullet(Bullet b);
void moveBullet(Bullet *pBullet, int id);
void moveActiveBullet(Tank *t);
void delAfterVaCham(Bullet *b);



int _tmain(int argc, _TCHAR* argv[])
{
	Tank player;

	initPlayerTank(&player, 0);
	drawPlayerTank(player);
	initBulletArr(&player);
	drawBattleBoard();

	void* handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO structCursorInfo;
	GetConsoleCursorInfo(handle,&structCursorInfo);
	structCursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo( handle, &structCursorInfo );
	
	while (1){
		if (player.id == 0)
			textcolor(12);
		else
			textcolor(2);
		
		drawMapToScreen();
		
		
		if(_kbhit){
			switch(_getch()){
			case char(72):
				goUp(&player);
				break;
			case char(80):
				goDown(&player);
				break;
			case char(75):
				goLeft(&player);
				break;
			case char(77):
				goRight(&player);
				break;
			case ' ':
				for (int i = 0; i < 100; i++){
					if (player.bullets[i].isActive == 0){
						initBullet((&player.bullets[i]), player);
						player.bullets[i].isActive = 1;
						break;
					}
				}
				break;
			}
		}
		
		moveActiveBullet(&player);
		drawMapToScreen();
		clearBoard();
		gotoxy(1, 1);
	}

	system("PAUSE");
	return 0;
}

void gotoxy(int x, int y){
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = x; scrn.Y = y;
	SetConsoleCursorPosition(hOuput,scrn);
}

void textcolor(int color){
	HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hcon,color);
}

void initBattleBoard(){
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			board[i][j] = ' ';
		}
	}
}

void drawBattleBoard(){
	for(int i = 0; i <= height + 1; i++)
	{
		for (int j = 0; j <= width + 1; j++)
		{
			if ((i == 0 || i == height + 1) && j >= 0 && j <= width + 1)
				cout << "#";
			else if ((j == 0 || j == width + 1) && i > 0 && i < height + 1)
				cout << "#";
			else
				cout << " ";
		}
		cout << "\n";
	}
}

void drawMapToScreen(){
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			gotoxy(j + 1, i + 1);
			cout << board[i][j];
		}
	}
}

void clearBoard(){
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			gotoxy(j + 1, i + 1);
			cout << "\0";
		}
	}
}

void resetMap(){
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
			board[i][j] = '\0';
	}
}

void initPlayerTank(Tank *pTank, int id){
	pTank -> id = id; 
	pTank -> life = 3;
	pTank -> angle = 0;
	pTank -> iCoord = height - 3;
	pTank -> jCoord = width / 2;
	
	pTank -> TankForm[0][0] = pTank -> TankForm[0][1] = 'x';
	pTank -> TankForm[0][2] = ' ';
	pTank -> TankForm[1][0] = pTank -> TankForm[1][1] = 'x';
	pTank -> TankForm[1][2] = 'x';
	pTank -> TankForm[2][0] = pTank -> TankForm[2][1] = 'x';
	pTank -> TankForm[2][2] = ' ';
}

void drawPlayerTank(Tank tank){
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			board[tank.iCoord + i][tank.jCoord + j] = tank.TankForm[i][j];
		}
	}
}

void delPlayerTank(Tank tank)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
			board[tank.iCoord + i][tank.jCoord + j] = ' ';
	}
}

void rotateTank90(Tank *pTank)
{
	char temp[3][3];

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			temp[3 - j - 1][i] = pTank->TankForm[i][j];
		}
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			pTank->TankForm[i][j] = temp[i][j];
		}
	}

	if (pTank->angle == 270)
		pTank->angle = 0;
	else
	{
		pTank->angle += 90;
	}

	delPlayerTank(*pTank);
	drawPlayerTank(*pTank);
}

void goUp(Tank *pTank)
{
	if (pTank->iCoord == 0)
		return;
	if (pTank->angle != 90)
	{
		while (pTank->angle != 90)
			rotateTank90(pTank);
		return;
	}
	else
	{
	delPlayerTank(*pTank);
	pTank->iCoord--;
	drawPlayerTank(*pTank);
	}
}

void goDown(Tank *pTank)
{
	if (pTank->iCoord == height - 3)
		return;
	if(pTank->angle != 270)
	{
		while (pTank->angle != 270)
			rotateTank90(pTank);
		return;
	}
	else
	{
	delPlayerTank(*pTank);
	pTank->iCoord++;
	drawPlayerTank(*pTank);
	}
}

void goLeft(Tank *pTank)
{
	if (pTank->jCoord == 0)
		return;
	if (pTank->angle != 180)
	{
		while (pTank->angle != 180)
			rotateTank90(pTank);
		return;
	}
	else
	{
	delPlayerTank(*pTank);
	pTank->jCoord--;
	drawPlayerTank(*pTank);
	}
}
void goRight(Tank *pTank)
{
	if (pTank->jCoord == width - 3)
		return;
	if (pTank->angle != 0)
	{
		while (pTank->angle != 0)
			rotateTank90(pTank);
		return;
	}
	else
	{
	delPlayerTank(*pTank);
	pTank->jCoord++;
	drawPlayerTank(*pTank);
	}
}

void initBulletArr(Tank *pTank)
{
	for (int i = 0; i < 100; i++)
	{
		pTank->bullets[i].isActive = 0;
	}
}

void initBullet(Bullet *pBullet, Tank tank)
{
	switch (tank.angle)
	{
	case 0:
		pBullet->iPos = tank.iCoord + 1;
		pBullet->jPos = tank.jCoord + 2;
		break;
	case 90:
		pBullet->iPos = tank.iCoord;
		pBullet->jPos = tank.jCoord + 1;
		break;
	case 180:
		pBullet->iPos = tank.iCoord + 1;
		pBullet->jPos = tank.jCoord ;
		break;
	case 270:
		pBullet->iPos = tank.iCoord + 2;
		pBullet->jPos = tank.jCoord + 1;
		break;
	}

	pBullet->fireAngle = tank.angle;
}

void drawBullet(Bullet b, int hostId)
{
	if (hostId == 0)
		board[b.iPos][b.jPos] = 'x';
	else if (hostId == 1)
		board[b.iPos][b.jPos] = '*';
	else if (hostId == 2)
		board[b.iPos][b.jPos] = 'u';
}

void delBullet(Bullet b)
{
	board[b.iPos][b.jPos] = '\0';
}


void moveBullet(Bullet *pBullet, int id)
{
	delBullet(*pBullet);
	switch (pBullet->fireAngle)
	{
	case 0:
		if (pBullet->jPos == width - 3)
			return;
		pBullet->jPos++;
		break;
	case 90:
		if (pBullet->iPos == 0)
			return;
		pBullet->iPos--;
		break;
	case 180:
		if (pBullet->jPos == 0)
			return;
		pBullet->jPos--;
		break;
	case 270:
		if (pBullet->iPos == height - 3)
			return;
		pBullet->iPos++;
		break;
	}
	
	drawBullet(*pBullet, id);
}

void moveActiveBullet(Tank *t)
{
	for (int i = 0; i < 100; i++)
	{
		if (t->bullets[i].isActive==1 &&(t->bullets[i].iPos <= 0 || t->bullets[i].iPos >= height - 3 || t->bullets[i].jPos <= 0 || 
			t->bullets[i].jPos >= width - 3))
		{
			delBullet(t->bullets[i]);
			t->bullets[i].isActive = 0;
			t->bullets[i].iPos = 0;
			t->bullets[i].jPos = 0;
			continue;
		}
		else if (t->bullets[i].isActive == 1)
		{
			moveBullet(&t->bullets[i], t->id);
		}
	}
}

void delAfterVaCham(Bullet *b)
{
	if (b->fireAngle == 0)
	{
		if (board[b->iPos][b->jPos + 1] != 'o')
		{
			return;
		}
	}

	else if (b->fireAngle == 90)
	{
		if (board[b->iPos - 1][b->jPos] != 'o')
		{
			return;
		}
	}

	else if (b->fireAngle == 180)
	{
		if (board[b->iPos][b->jPos - 1] != 'o')
		{
			return;
		}
	}

	else if (b->fireAngle == 270)
	{
		if (board[b->iPos + 1][b->jPos] != 'o')
		{
			return;
		}
	}

	b->isActive = 0;
	delBullet(*b);
	b->iPos = 0;
	b->jPos = 0;
}

