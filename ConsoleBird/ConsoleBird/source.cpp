#include <windows.h>
#include <iostream>
#include <cmath>
#include <conio.h>
#include <ctime>

using namespace std;

#define PI 3.14
HANDLE hStdin;
DWORD fdwMode;
HWND myconsole;
HDC dc;
HPEN draw_pen;
HPEN black_pen;
HBRUSH hbr;
int height, widght;
bool run;
bool lose, lose_drawed;
int scores;

int ppt = 3;
const double lenght_between_pipes = 0.34;
const int pipe_count = 5;

//pipe structure
struct pipe
{
	int x;
	double y;
	bool scored;

	int h = height * y;
	int w = widght * 0.095;
	int ha = height * 0.0625;
	int wa = w * 0.95;
	int hole_height = height * 0.19;

	pipe()
	{
		x = INT_MAX;
		y = 0.5;
		scored = false;

		h = height * y;
		w = widght * 0.095;
		ha = height * 0.0625;
		wa = w * 0.95;
		hole_height = height * 0.19;
	}

	void draw(int x, HPEN pen)
	{
		SelectObject(dc, pen);

		hole_height = height * 0.19;

		h = height * y;
		w = widght * 0.095;
		ha = height * 0.0625;
		wa = w * 0.95;

		MoveToEx(dc, x, 0, NULL);
		LineTo(dc, x, h - ha);
		LineTo(dc, x + wa, h - ha);
		LineTo(dc, x + wa, 0);
		MoveToEx(dc, x, h - ha, NULL);
		LineTo(dc, x - (w - wa), h - ha);
		LineTo(dc, x - (w - wa), h);
		LineTo(dc, x + w, h);
		LineTo(dc, x + w, h - ha);
		LineTo(dc, x + wa, h - ha);
		MoveToEx(dc, x - (w - wa), h + hole_height, NULL);
		LineTo(dc, x - (w - wa), h + hole_height + ha);
		LineTo(dc, x + w, h + hole_height + ha);
		LineTo(dc, x + w, h + hole_height);
		LineTo(dc, x - (w - wa), h + hole_height);
		MoveToEx(dc, x, h + hole_height + ha, NULL);
		LineTo(dc, x, height);
		MoveToEx(dc, x + wa, h + hole_height + ha, NULL);
		LineTo(dc, x + wa, height);
	}
}
pipes[pipe_count];

struct bird
{

	const double sz = 0.05;
	int x, y, ox, oy, d;
	double t = 0, u = 0, m, a = 0, g = 10.0 / 100.0;

	bird()
	{
		ox = x = y = 200;
		m = 2;
	}

	void draw()
	{
		SelectObject(dc, hbr);
		SelectObject(dc, black_pen);
		Ellipse(dc, x, oy, x + height * sz, oy + d * 0.85);
		Ellipse(dc, x + d * 0.4, oy + d*0.5, x + d, oy + d * 0.85);
		Ellipse(dc, x + d * 0.5, oy, x + d, oy + d * 0.5);
		Ellipse(dc, x, oy + d * 0.2, x + d * 0.45, oy + d * 0.5);
		MoveToEx(dc, x + d, oy + d * 0.68, NULL);
		LineTo(dc, x + d - d * 0.45, oy + d * 0.68);

		SelectObject(dc, draw_pen);
		Ellipse(dc, x, y, x + height * sz, y + d * 0.85);
		Ellipse(dc, x + d * 0.4, y + d*0.5, x + d, y + d * 0.85);
		Ellipse(dc, x + d * 0.5, y, x + d, y + d * 0.5);
		Ellipse(dc, x, y + d * 0.2, x + d * 0.45, y + d * 0.5);
		MoveToEx(dc, x + d, y + d * 0.68, NULL);
		LineTo(dc, x + d - d * 0.45, y + d * 0.68);
		MoveToEx(dc, x + d * 0.85, y + d * 0.18, NULL);
		LineTo(dc, x + d * 0.85, y + d * 0.32);


	}

	void update()
	{
		d = height * sz;
		u += g * t;
		oy = y;
		y = y + u * height * 0.001;
		t++;
	}
} flappy;

int main()
{
	srand(time(NULL));

	//Get a console handle
	myconsole = GetConsoleWindow();
	//Get a handle to device context
	dc = GetDC(myconsole);
	run = true;
	lose = lose_drawed = false;
	scores = 0;
	cout << scores << '\b';
	SetCaretPos(0, 0);

	//inits pens and brush
	draw_pen = CreatePen(PS_SOLID, 1, RGB(192, 192, 192));
	black_pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	hbr = CreateSolidBrush(RGB(0, 0, 0));

	//console input mode
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(hStdin, fdwMode);

	//get the height and widght
	BITMAP structBitmapHeader1;
	memset(&structBitmapHeader1, 0, sizeof(BITMAP));
	HGDIOBJ hBitmap1 = GetCurrentObject(dc, OBJ_BITMAP);
	GetObject(hBitmap1, sizeof(BITMAP), &structBitmapHeader1);
	height = structBitmapHeader1.bmHeight;
	widght = structBitmapHeader1.bmWidth;

	//setting the first pipe
	pipes[0].x = widght;
	pipes[0].y = (rand() % 29 + 15) / 100.0;

	bool pressed = false;
	bool released = true;
	while (run)
	{
		//Update info about height and widght, if user change the size of console
		BITMAP structBitmapHeader;
		memset(&structBitmapHeader, 0, sizeof(BITMAP));
		HGDIOBJ hBitmap = GetCurrentObject(dc, OBJ_BITMAP);
		GetObject(hBitmap, sizeof(BITMAP), &structBitmapHeader);
		height = structBitmapHeader.bmHeight;
		widght = structBitmapHeader.bmWidth;


		//draw bird
		if (!lose)
		{
			flappy.draw();
			flappy.update();
		}
		//working with pipes
		for (int i = 0; i < pipe_count && !lose; i++)
		{
			//checking, if we can start new pipe
			if (pipes[i].x == INT_MAX && (widght - pipes[i > 0 ? i - 1 : pipe_count - 1].x > widght * lenght_between_pipes))
			{
				pipes[i].x = widght;
				pipes[i].y = (rand() % 35 + 15) / 100.0;
			}

			//unuse pipe, which got out of windows
			if (pipes[i].x < -widght * 0.1)
			{
				pipes[i].x = INT_MAX;
				pipes[i].scored = false;
			}

			//drawing pipes
			if (pipes[i].x != INT_MAX)
			{
				pipes[i].x -= ppt;

				pipes[i].draw(pipes[i].x + ppt, black_pen); //erase old pipe
				pipes[i].draw(pipes[i].x, draw_pen); //draw new pipe
			}

			//checking the pos of flappy
			if (pipes[i].x != INT_MAX && pipes[i].x < flappy.x + flappy.d && pipes[i].x + pipes[i].w > flappy.x && (flappy.y < pipes[i].h || flappy.y + flappy.d * 0.85 > pipes[i].h + pipes[i].hole_height))
				lose = true;

			//dont hit the ground!
			if (flappy.y + flappy.d * 0.85 > height)
				lose = true;

			//sky is bordered, you have no freedom!
			if (flappy.y < 0)
				flappy.y = 0;
			

			if (pipes[i].x != INT_MAX && !pipes[i].scored && pipes[i].x + pipes[i].x + pipes[i].w < flappy.x)
			{
				pipes[i].scored = true;
				scores++;
				ppt += 1;
				cout << scores;
				int t = scores;
				while (t && cout << '\b')
					t /= 10;
			}

		}
		released = true;
		//keyboard actions
		if (_kbhit())
		{
			switch (_getch())
			{
			case 27: //esc
				run = false;
			default:
				//you can drive your bird with any key
				if (!pressed)
				{
					flappy.t = 0;
					flappy.u = -11;
					pressed = true;
				}
				released = false;


				if (lose)
				{
					lose = false;
					lose_drawed = false;


					int t = scores;
					while (t && cout << '\b')
						t /= 10;
					int t1 = scores;
					while (t1 && cout << ' ')
						t1 /= 10;
					int t2 = scores;
					while (t2 && cout << '\b')
						t2 /= 10;

					for (int i = 0; i < 40; i++)
						cout << "\b";

					cout << "                                 ";

					for (int i = 0; i < 40; i++)
						cout << "\b";

					scores = 0;
					flappy.x = flappy.y = 200;
					flappy.u = 0;

					for (int i = 0; i < pipe_count; i++)
					{
						pipes[i].draw(pipes[i].x, black_pen);
						pipes[i].x = INT_MAX;
					}
					pipes[0].x = widght;
					ppt = 3;
				}
				break;
			}
		}

		if (lose && !lose_drawed)
		{
			lose_drawed = true;

			//SendMessage(myconsole, EM_SETSEL, 10, 10);
			cout << "GAME OVER"
				<< '.'
				<< " YOUR SCORE: " << scores;
		}

		if (released)
			pressed = false;
		// 1000/25 = 40; it deletes some blinks
		Sleep((BYTE)40);
	}

	cout << "o_O" << '\n' << "I won't let you go!";
	_getch();

	return 0;
}