// CrazyTank.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <ctime>

using namespace std;
const int width = 50;
const int height = 20;

HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);

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
void drawBattleBoard();
void drawMapToScreen();
void clearBoard();
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
	const clock_t begin_time = clock();
	Tank player;
	Tank enemy;
	Tank enemy_1;
	int score = 0;

	initPlayerTank(&player, 0);
	initPlayerTank(&enemy, 1);
	initPlayerTank(&enemy_1, 2);
	drawPlayerTank(player);
	drawPlayerTank(enemy);
	drawPlayerTank(enemy_1);
	initBulletArr(&player);
	drawBattleBoard();
	drawMapToScreen();

	void* handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO structCursorInfo;
	GetConsoleCursorInfo(handle,&structCursorInfo);
	structCursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo( handle, &structCursorInfo );
	
	while (1){
		{
			gotoxy(55, 9);
			cout << "Life: " << player.life << endl;
			gotoxy(55, 10);
			cout << "Score: " << score << endl;
			gotoxy(55, 11);
			cout << "Game time: " << float( clock () - begin_time ) /  CLOCKS_PER_SEC;
		}
		
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
		drawPlayerTank(enemy);
		drawPlayerTank(enemy_1);
		drawMapToScreen();	
		clearBoard();
		gotoxy(1, 1);
	}

	return 0;
}

void gotoxy(int x, int y){
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = x; scrn.Y = y;
	SetConsoleCursorPosition(hOuput,scrn);
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
	for(int i = height - 1; i >= height - 2; i--){ 
		for(int j = width / 2 - 2; j <= width / 2 + 2; j++){
			if (i == height - 1 && j >= width / 2 - 1 && j <= width / 2 + 1)
				board[i][j] = char(177);
			else
				board[i][j] = char(254);
			SetConsoleTextAttribute(hcon,2);
			
		}
	}

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

void initPlayerTank(Tank *pTank, int id){
	pTank -> id = id; 
	pTank -> life = 3;
	pTank -> angle = 0;

	switch(pTank -> id){
	case 0:
		pTank -> iCoord = height - 5;
		pTank -> jCoord = width / 2;
		break;
	case 1:
		srand(time(NULL));
		pTank -> iCoord = rand() % (height - 10);
		pTank -> jCoord = rand() % (width - 4);
		break;
	case 2:
		bool isExist = true;
		while(isExist == true){
			int i = rand() % (height - 10);
			int j = rand() % (width - 4);
			pTank -> iCoord = i;
			pTank -> jCoord = j;
			for (int k = i - 2; k < i + 5; k++){
				for (int m = j - 2; m < j + 5; m++){
					if (board[k][m] != '0'){
						isExist = false;
					}
				}
			}
		}
		
		break;
	}

	if (id == 0){
		pTank -> TankForm[0][0] = pTank -> TankForm[0][1] = 'x';
		pTank -> TankForm[0][2] = ' ';
		pTank -> TankForm[1][0] = pTank -> TankForm[1][1] = 'x';
		pTank -> TankForm[1][2] = 'x';
		pTank -> TankForm[2][0] = pTank -> TankForm[2][1] = 'x';
		pTank -> TankForm[2][2] = ' ';
	}
	else {
		pTank -> TankForm[0][0] = pTank -> TankForm[0][1] = 'o';
		pTank -> TankForm[0][2] = ' ';
		pTank -> TankForm[1][0] = pTank -> TankForm[1][1] = 'o';
		pTank -> TankForm[1][2] = 'o';
		pTank -> TankForm[2][0] = pTank -> TankForm[2][1] = 'o';
		pTank -> TankForm[2][2] = ' ';
	}
}
	


	/*srand(time(NULL));
	while (count < tankNumber) {
		int i = rand() % (height - 10);
		int j = rand() % (width - 4);
		for(int i; i < ; i--){ 
			for(int j; j <= width / 2 + 2; j++){
				if(board[i][j] != 'o') {
					board[i][j] = 0;
					count++;*/


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
	for (int j = 0; j < width; j++){
		if((board[pTank -> iCoord - 1][j] == char(254) || board[pTank -> iCoord - 1][j] == 'o') 
			&& pTank -> jCoord >= j - 2 && pTank -> jCoord <= j)
			return;
	}

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
	for (int j = 0; j < width; j++){
		if((board[pTank -> iCoord + 3][j] == char(254) || board[pTank -> iCoord + 3][j] == 'o')
			&& pTank -> jCoord >= j - 2 && pTank -> jCoord <= j)
			return;
	}

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

	for (int i = 0; i < height; i++){
		if((board[i][pTank -> jCoord - 1] == char(254) || board[i][pTank -> jCoord - 1] == 'o') 
			&& pTank -> iCoord >= i - 2 && pTank -> iCoord <= i)
			return;
	}

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
	for (int i = 0; i < height; i++){
		if((board[i][pTank -> jCoord + 3] == char(254) || board[i][pTank -> jCoord + 3] == 'o') 
			&& pTank -> iCoord >= i - 2 && pTank -> iCoord <= i)
			return;
	}

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
		board[b.iPos][b.jPos] = '*';
	else 
		board[b.iPos][b.jPos] = 'o';
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

