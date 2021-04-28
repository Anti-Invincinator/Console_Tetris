
#include<iostream>
#include<thread>
#include<vector>

using namespace std;

#include<Windows.h>
#include<stdio.h>

/*


==============================================================CONSOLE TETRIS=================================================================================


Edit the console window properties :

	Window Size :  
				* Height : 80
				* Width  : 27
	
	Font Size : 36
!!!IMPORTANT or else the output won't be right.

CONTROLS :

 1.	Left and Right Cursor Keys for Horizontal Movement 
 2.	Down Cursor Key to fasten the piece's drop rate 
 3.	'Z' key to rotate the piece
*/


wstring tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char* pField = nullptr;

int nScreenWidth = 80;  //Console Screen Size X
int nScreenHeight = 30; //Console Screen Size Y

int Rotate(int px, int py, int r)  //(indexX, indexY, rotation)
{
	switch (r % 4)
	{
	case 0 : return py * 4 + px;                  
	case 1 : return 12 + py - (px * 4);			  
	case 2 : return 15 - (py * 4) - px;			  	
	case 3 : return 3 - py + (px * 4);			  
	}

	return 0;
}

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++)
		{
			//Get index into piece
			int pi = Rotate(px, py, nRotation);

			//Getting index into field
			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			//Collision Detection
			if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
			{
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
				{
					if (tetromino[nTetromino][pi] == L'X' && pField[fi] != 0)
						return false; //fail on first hit
				}
			}
		}

	return true;
}

int main()
{
	//Setting Up the Screen
	wchar_t* screen = new wchar_t[nScreenWidth*nScreenHeight];
	for (long int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	//Setting Up Console Output Window
	SMALL_RECT rect;
	COORD coord;

	coord.X = nScreenWidth;
	coord.Y = nScreenHeight;

	rect.Top = 0;
	rect.Left = 0;
	rect.Bottom = coord.Y - 1;
	rect.Right = coord.X - 1;

	HANDLE hwnd = GetStdHandle(STD_OUTPUT_HANDLE); // get handle
	SetConsoleScreenBufferSize(hwnd, coord);       // set buffer size
	SetConsoleWindowInfo(hwnd, TRUE, &rect);       // set window size

	//The Assets
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"....");

	tetromino[2].append(L".X..");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"....");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L"....");

	tetromino[4].append(L"..X.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"....");

	tetromino[5].append(L"....");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..X.");

	tetromino[6].append(L"....");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L".X..");
	tetromino[6].append(L".X..");


	//Setting Up The Game
	pField = new unsigned char[nFieldWidth * nFieldHeight];
	for (int x = 0; x < nFieldWidth; x++) //Board Boundary
		for (int y = 0; y < nFieldHeight; y++)
			pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;


	//Gameplay Loic
	bool bGameOver = false;

	int nCurrentPiece = rand() % 7;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2 - 2;
	int nCurrentY = 0;

	//Input Keys
	bool bKey[4]; 
	bool bRotateHold = false;

	int nSpeed = 20;
	int nSpeedCounter = 0;
	bool bForceDown = false;
	int nPieceCount = 0;
	int nScore = 0;

	vector<int> vLines;

	//Gameplay Loop
	while (!bGameOver)
	{
		//GAME TIMING ====================================================================================================================================================
		this_thread::sleep_for(50ms);  //One Single Game Tick
		nSpeedCounter++;
		bForceDown = (nSpeedCounter == nSpeed);


		//INPUT ==========================================================================================================================================================
		for (int k = 0; k < 4; k++)								 //R   L   D  Z
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

		//GAME LOGIC =====================================================================================================================================================

		//Moving The Current Piece
		nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0; //Moving Left
		nCurrentX -= (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0; //Moving Right
		nCurrentY += (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0; //Moving Down

		if (bKey[3])
		{
			nCurrentRotation += (!bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
			bRotateHold = true;
		}
		else
		{
			bRotateHold = false;
		}
		
		if (bForceDown)
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY++;
			else
			{
				//Lock the current piece in field
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;


				//Increasing the difficulty
				if (nPieceCount % 10 == 0)
					if (nSpeed >= 10) nSpeed--;


				//Check for finished lines
				for (int py = 0; py < 4; py++)
					if (nCurrentY + py < nFieldHeight - 1)
					{
						bool bLine = true;
						for (int px = 1; px < nFieldWidth -1; px++)
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

						if (bLine)
						{
							//Remove Line, set it to =
							for (int px = 1; px < nFieldWidth - 1; px++)
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;

							vLines.push_back(nCurrentY + py);
						}
					}

				//Calculating Score
				nScore += 25;
				if (!vLines.empty()) nScore += (1 << vLines.size()) * 100;

				//Choose Next piece
				nCurrentX = nFieldWidth / 2 - 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;

				//if piece does not fit
				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}

			nSpeedCounter = 0;
		}

		//RENDER OUTPUT ==================================================================================================================================================

		//Draw Field
		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
				  screen[(y + 2) * nScreenWidth + (x + 2)] = L" 0000000=#"[pField[y * nFieldWidth + x]];

		//Draw Current Piece
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
					screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;

		//Draw Score
		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);

		if (!vLines.empty())
		{
			//Draw Lines
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms); //Delay

			for (auto &v : vLines)
				for (int px = 1; px < nFieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--)
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
					pField[px] = 0;
				}

			vLines.clear();  //Emptying Finished Lines
		}

		//Display Frame
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}

	//Game Over Screen
	CloseHandle(hConsole);
	cout << "GAME OVER !! Score: " << nScore << endl;
	system("pause");


	return 0;
}
