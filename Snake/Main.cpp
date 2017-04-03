#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "SDL_image.h"
#include "cstdlib"
#include "ctime"
#include "cmath"
#include "string"
#include "sstream"
#include "iostream"
using namespace std;
SDL_Window *windows;
SDL_Renderer *renderer;
TTF_Font *times;
SDL_Rect Snake_Box = { 20,100,500,400 };
SDL_Rect Word_Box = { 550,80,250,80 };
SDL_Rect Score_Box = { 550,200,250,120 };
SDL_Rect Score_t = { 600,210,100,40 };
SDL_Rect Score_int = { 660,260,40,40 };
SDL_Rect snakeRect[100];
SDL_Rect Fruit_Rect[3];
SDL_Surface *Fruit_Sur[5];
SDL_Texture *Fruit_Text[5];
SDL_Color white = { 255,255,255 };
typedef struct {
	int x, y;
	int ox, oy;
	const int width = 12;
	const int height = 12;
}snake;
snake Snake[100];
typedef struct
{
	int x, y;
	bool state;
}fruit;
typedef struct
{
	int x, y;
}dir;
dir Dir[4];
fruit Fruit[5];
char let1;
char let2;
const int speed = 12;
int snakelength = 10;
int fruitnumber = 2;
int score = 0;
int tick = 30;
int direction = 1;
int odir = 3;
bool running = true;
bool stop = false;
bool eatfruit = true;;
//const char alphabet[29] = { 'A','B','C','D','S','F','G','H','J','K','L','Q','W','R','T','Y','U','I','O','P','Z','X','V','N','M','E '};
string alphabet = "QWERTYUIOPLKJHGFDSAZXCVBNM";

void RenderText(SDL_Surface *sur, SDL_Rect *Rect) {
	SDL_Texture *text = NULL;
	text = SDL_CreateTextureFromSurface(renderer, sur);
	//if (text == NULL)
	//{
	//	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", SDL_GetError(), windows);
	//}
	SDL_RenderCopy(renderer, text, NULL, Rect);
	SDL_DestroyTexture(text);
	SDL_FreeSurface(sur);
}

char GetRandomChar() {
	srand(unsigned int(time(NULL)));
	int a = rand() % 26;
	return alphabet[a];
}

void Quit();

bool EatSnake() {
	for (int i = 1; i < snakelength; ++i) {
		if (Snake[0].x == Snake[i].x && Snake[0].y == Snake[i].y)
		{
			return false;
		}
	}
	return true;
}

void snakeMove(int dir) {
	Snake[0].ox = Snake[0].x;
	Snake[0].oy = Snake[0].y;
	Snake[0].x += Dir[dir].x;
	Snake[0].y += Dir[dir].y;
	for (int i = 1; i < snakelength; ++i) {
		Snake[i].ox = Snake[i].x;
		Snake[i].oy = Snake[i].y;
		Snake[i].x = Snake[i - 1].ox;
		Snake[i].y = Snake[i - 1].oy;
	}
	SDL_Delay(tick);
} //done

bool SnakeCanMove() {
	if (abs(odir - direction) == 2) return false;
	return true;
}

void snakeWall() {
	if (Snake[0].x + Snake[0].width - 1> Snake_Box.x + Snake_Box.w ) Snake[0].x = Snake_Box.x;
	else if (Snake[0].x < Snake_Box.x) Snake[0].x = Snake_Box.x + Snake_Box.w - Snake[0].width;
	if (Snake[0].y  + Snake[0].height - 1> Snake_Box.y + Snake_Box.h ) Snake[0].y = Snake_Box.y;
	else if (Snake[0].y < Snake_Box.y) Snake[0].y = Snake_Box.y + Snake_Box.h  - Snake[0].height;
}

void SnakeEatFruit()
{
	for (int i = 0; i < fruitnumber; ++i) {
		if (Snake[0].x >= Fruit_Rect[i].x && Snake[0].x +Snake[0].width <= Fruit[i].x + Fruit_Rect[i].w && 
			Snake[0].y >= Fruit_Rect[i].y && Snake[0].y + Snake[0].height <= Fruit[i].y + Fruit_Rect[i].h)
		/*if(Snake[0].x == Fruit_Rect[i].x && Snake[0].y == Fruit_Rect[i].y)*/
		{
			Snake[snakelength].x = Snake[snakelength - 1].ox;
			Snake[snakelength].y = Snake[snakelength - 1].oy;
			snakelength += 1;
			score += 1;
			eatfruit = true;
		}
	}
}

void SnakeMoveKeyboard()
{
	const Uint8 *state = SDL_GetKeyboardState(NULL);
		if(SnakeCanMove()==1)
		odir = direction;
		if (state[SDL_SCANCODE_UP])
		{
			direction = 0;
			if(SnakeCanMove()==true)
			snakeMove(direction);
		}
		else if (state[SDL_SCANCODE_DOWN])
		{
			direction = 2;
			if (SnakeCanMove()==true)
			snakeMove(direction);
		}
		else if (state[SDL_SCANCODE_RIGHT])
		{
			direction = 1;
			if (SnakeCanMove()==true)
			snakeMove(direction);
		}
		else if (state[SDL_SCANCODE_LEFT])
		{
			direction = 3;
			if (SnakeCanMove()==true)
			snakeMove(direction);
		}
		
} //done 

void MakeFruit() {
	if (eatfruit == true) {
		for (int i = 0; i < fruitnumber; ++i) {
			int randomx = rand() % 40 + 1;
			int randomy = rand() % 32 + 1;
			Fruit[i].x = randomx * 12 + Snake_Box.x;
			Fruit[i].y = randomy * 12 + Snake_Box.y;
			char temp = GetRandomChar();
			if (i == 0) let1 = temp;
			else if (i == 1) let2 = temp;
			SDL_Delay(30);
		}
		for (int i = 0; i < fruitnumber; ++i) {
			Fruit_Rect[i].x = Fruit[i].x;
			Fruit_Rect[i].y = Fruit[i].y;
			Fruit_Rect[i].w = 12;
			Fruit_Rect[i].h = 12;
		}
		eatfruit = false;
	}

}

void DrawFruit() {
	SDL_Surface *temp = NULL;
	stringstream letter1, letter2;
	letter1 << let1;
	letter2 << let2;
	for (int i = 0; i < fruitnumber; ++i) 
	{
			if (i == 0) {
				temp = TTF_RenderText_Solid(times, letter2.str().c_str(), white);
			}
			else if (i == 1) {
				temp = TTF_RenderText_Solid(times, letter1.str().c_str(), white);			
			}
			else temp = TTF_RenderText_Solid(times, "*", white);
			RenderText(temp, &Fruit_Rect[i]);
			SDL_RenderDrawRect(renderer, &Fruit_Rect[i]);
	}	
}

void DrawScore() {
	stringstream sscore;
	sscore << score;
	SDL_Surface *score_sur = TTF_RenderText_Solid(times, sscore.str().c_str(), white);
	RenderText(score_sur, &Score_int);
	SDL_Surface *score_text = TTF_RenderText_Solid(times, "SCORE: ", white);
	RenderText(score_text, &Score_t);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(renderer, &Score_Box);

}

void PrintResut() {
	SDL_Surface *Result = TTF_RenderText_Solid(times, "You lose. Try Again", white);
	SDL_Rect Rect = { Snake_Box.x + Snake_Box.w / 2 - 200,Snake_Box.y + Snake_Box.h / 2 - 20,400,40 };
	RenderText(Result, &Rect);
	SDL_RenderPresent(renderer);
}

void LoadGame()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_VideoInit(NULL);
	TTF_Init();
	if (TTF_Init() == -1)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", SDL_GetError(), windows);
	}
	IMG_Init(IMG_INIT_PNG);
	windows = SDL_CreateWindow("Snake Word", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(windows, -1, SDL_RENDERER_ACCELERATED);
	if (windows == NULL || renderer == NULL)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", SDL_GetError(), windows);
	}
	Snake[0].x = Snake_Box.x ;
	Snake[0].y = Snake_Box.y ;
	for (int i = 1; i < snakelength; ++i)
	{
		Snake[i].x = Snake_Box.x + (speed)*i;
		Snake[i].y = Snake_Box.y ;
	}
	Dir[0].x = 0;
	Dir[0].y = -(speed);

	Dir[1].x = (speed);
	Dir[1].y = 0;

	Dir[2].x = 0;
	Dir[2].y = (speed);

	Dir[3].x = -(speed);
	Dir[3].y = 0;

	times = TTF_OpenFont("times.ttf", 30);
}

void DrawScreen()
{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawRect(renderer, &Snake_Box);
		SDL_RenderDrawRect(renderer, &Word_Box);
		if (stop == false) {

			
			for (int i = 0; i < snakelength; ++i)
			{
				if (i != 0)
				{
					SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
				}
				else {
					SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
				}
				snakeRect[i].x = Snake[i].x;
				snakeRect[i].y = Snake[i].y;
				snakeRect[i].w = Snake[i].width;
				snakeRect[i].h = Snake[i].height;
				SDL_RenderFillRect(renderer, &snakeRect[i]);
			}
			DrawFruit();
			DrawScore();
			SDL_RenderPresent(renderer);
		}
		else
		{
			PrintResut();
		}
}

void Logic()
{
	MakeFruit();
	SnakeMoveKeyboard();
	snakeWall();
	if (EatSnake() == false) {
		stop = true;
	}
	SnakeEatFruit();
}

void Quit()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(windows);
	TTF_CloseFont(times);
	SDL_VideoQuit();
	TTF_Quit();
	SDL_Quit();
}

int main(int argv, char **args)
{

	SDL_Event occur;
	LoadGame();
	while (running == true)
	{
		SDL_PollEvent(&occur);
		if (occur.type == SDL_QUIT)
		{
			Quit();
		}
		if (occur.type == SDL_KEYDOWN) {
			if (occur.key.keysym.sym == SDLK_SPACE) {
				Quit();
			}
		}
		Logic();
		DrawScreen();
	}
	//system("pause");
	return 0;
}