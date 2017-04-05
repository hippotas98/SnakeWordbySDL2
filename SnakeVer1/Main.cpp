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
TTF_Font *NotoSan;
SDL_Rect Snake_Box = { 20,100,500,400 };
SDL_Rect Word_Box = { 550,80,250,80 };
SDL_Rect Score_Box = { 550,200,250,120 };
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
bool eatfruit = true;
int winner = 2;
//const char alphabet[29] = { 'A','B','C','D','S','F','G','H','J','K','L','Q','W','R','T','Y','U','I','O','P','Z','X','V','N','M','E '};
string alphabet = "QWERTYUIOPLKJHGFDSAZXCVBNM";
string chosenWord;
string guessWord;
//char word;
bool makeword = true;
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

//Guess Word 

const string WORD_LIST[] = {
	"angle", "ant", "apple", "arch", "arm", "army",
	"baby", "bag", "ball", "band", "basin", "basket", "bath", "bed", "bee", "bell", "berry",
	"bird", "blade", "board", "boat", "bone", "book", "boot", "bottle", "box", "boy",
	"brain", "brake", "branch", "brick", "bridge", "brush", "bucket", "bulb", "button",
	"cake", "camera", "card",  "cart", "carriage", "cat", "chain", "cheese", "chest",
	"chin", "church", "circle", "clock", "cloud", "coat", "collar", "comb", "cord",
	"cow", "cup", "curtain", "cushion",
	"dog", "door", "drain", "drawer", "dress", "drop", "ear", "egg", "engine", "eye",
	"face", "farm", "feather", "finger", "fish", "flag", "floor", "fly",
	"foot", "fork", "fowl", "frame",
	"garden", "girl", "glove", "goat", "gun",
	"hair", "hammer", "hand", "hat", "head", "heart", "hook", "horn", "horse",
	"hospital", "house",
	"island", "jewel",
	"kettle", "key", "knee", "knife", "knot",
	"leaf", "leg", "library", "line", "lip", "lock",
	"map", "match", "monkey", "moon", "mouth", "muscle",
	"nail", "neck", "needle", "nerve", "net", "nose", "nut",
	"office", "orange", "oven", "parcel", "pen", "pencil", "picture", "pig", "pin",
	"pipe", "plane", "plate", "plow", "pocket", "pot", "potato", "prison", "pump",
	"rail", "rat", "receipt", "ring", "rod", "roof", "root",
	"sail", "school", "scissors", "screw", "seed", "sheep", "shelf", "ship", "shirt",
	"shoe", "skin", "skirt", "snake", "sock", "spade", "sponge", "spoon", "spring",
	"square", "stamp", "star", "station", "stem", "stick", "stocking", "stomach",
	"store", "street", "sun",
	"table", "tail", "thread", "throat", "thumb", "ticket", "toe", "tongue", "tooth",
	"town", "train", "tray", "tree", "trousers",
	"umbrella",
	"wall", "watch", "wheel", "whip", "whistle", "window", "wire", "wing", "worm" };

const int WORD_COUNT = sizeof(WORD_LIST) / sizeof(string);


string ChosenWord() {
	int chosen = rand()% WORD_COUNT;
	return WORD_LIST[chosen];
}

void MakeGuess() {
	guessWord = chosenWord;
	char *temp = "-";
	for (int i = 0; i < chosenWord.length(); ++i)
	{
		guessWord[i] = *temp;
	}
}

bool Contains(char word) {
	for (int i = 0; i < chosenWord.length(); ++i) {
		if (word == chosenWord[i])
		{
			return true;
		}
	}
	return false;
}

void Update(char word) {
	for (int i = 0; i < chosenWord.length(); ++i) {
		if (word == chosenWord[i])
		{
			guessWord[i] = word;
		}
	}
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
	if (Snake[0].x + Snake[0].width > Snake_Box.x + Snake_Box.w ) Snake[0].x = Snake_Box.x;
	else if (Snake[0].x < Snake_Box.x) Snake[0].x = Snake_Box.x + Snake_Box.w - Snake[0].width;
	if (Snake[0].y  + Snake[0].height > Snake_Box.y + Snake_Box.h ) Snake[0].y = Snake_Box.y;
	else if (Snake[0].y < Snake_Box.y) Snake[0].y = Snake_Box.y + Snake_Box.h  - Snake[0].height;
}

void IncreaseSnakeLength()
{
	Snake[snakelength].x = Snake[snakelength - 1].ox;
	Snake[snakelength].y = Snake[snakelength - 1].oy;
	snakelength += 1;
}

void SnakeEatFruit()
{
	if (eatfruit == false) {
		for (int i = 0; i < fruitnumber; ++i) {
			if (Snake[0].x >= Fruit_Rect[i].x && Snake[0].x + Snake[0].width <= Fruit[i].x + Fruit_Rect[i].w &&
				Snake[0].y >= Fruit_Rect[i].y && Snake[0].y + Snake[0].height <= Fruit[i].y + Fruit_Rect[i].h)
				/*if(Snake[0].x == Fruit_Rect[i].x && Snake[0].y == Fruit_Rect[i].y)*/
			{
				if (i == 0) {
					if (let1 == let2)
					{
						int random = rand() % 2;
						if (random == 0)
						{
							IncreaseSnakeLength();
							score += 1;
						}
						else
						{
							snakelength -= 1;
						}
					}
					else
					{
						snakelength -= 1;
					}
				}
				else
				{
					IncreaseSnakeLength();
					score += 1;
					Update(let2);
				}
				eatfruit = true;
			}
		}
	}
}

int Count() {
	int count = 0;
	for (int i = 0; i < chosenWord.length(); ++i)
	{
		char *temp = "-";
		if (guessWord[i] == *temp)
		{
			count++;
		}
	}
	return count;
}

void LoseGame()
{
	if (snakelength <= 3)
	{
		winner = 0;
	}
	else if (Count() == 0)
	{
		winner = 1;
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

void MakeFruitTwo() {
	if (eatfruit == true) {
			int randomx = rand() % 40;
			int randomy = rand() % 32;
			Fruit[0].x = randomx * 12 + Snake_Box.x;
			Fruit[0].y = randomy * 12 + Snake_Box.y;
			char temp = GetRandomChar();
			let1 = temp;
		}
		for (int i = 0; i < fruitnumber; ++i) {
			Fruit_Rect[i].x = Fruit[i].x;
			Fruit_Rect[i].y = Fruit[i].y;
			Fruit_Rect[i].w = 12;
			Fruit_Rect[i].h = 12;
		}
		eatfruit = false;
	}

void MakeFruitOne()
{
	if (eatfruit == true) {
		int randomx = rand() % 40;
		int randomy = rand() % 32;
		Fruit[1].x = randomx * 12 + Snake_Box.x;
		Fruit[1].y = randomy * 12 + Snake_Box.y;
		char temp = chosenWord[rand() % chosenWord.length()];
		let2 = temp;
		SDL_Delay(1000);
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
				temp = TTF_RenderText_Solid(NotoSan, letter2.str().c_str(), white);
			}
			else if (i == 1) {
				temp = TTF_RenderText_Solid(NotoSan, letter1.str().c_str(), white);			
			}
			else temp = TTF_RenderText_Solid(NotoSan, "*", white);
			RenderText(temp, &Fruit_Rect[i]);
			SDL_RenderDrawRect(renderer, &Fruit_Rect[i]);
	}	
}

void DrawScore() {
	SDL_Rect Score_t = { 600,210,100,40 };
	SDL_Rect Score_int = { 660,260,40,40 };
	stringstream sscore;
	sscore << score;
	SDL_Surface *score_sur = TTF_RenderText_Solid(NotoSan, sscore.str().c_str(), white);
	RenderText(score_sur, &Score_int);
	SDL_Surface *score_text = TTF_RenderText_Solid(NotoSan, "SCORE ", white);
	RenderText(score_text, &Score_t);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(renderer, &Score_Box);

}

void DrawWord() {
	int width = 30 * chosenWord.length();
	SDL_Rect Word = { Word_Box.x + Word_Box.w/2  - width/2 , Word_Box.y + 10,width , 60 };
	stringstream word_stream;
	word_stream << guessWord;
	SDL_Surface *Word_Sur = TTF_RenderText_Solid(NotoSan, word_stream.str().c_str(), white);
	RenderText(Word_Sur, &Word);
}

void PrintResut() {
	SDL_Surface *Result = TTF_RenderText_Solid(NotoSan, "You lose. Try Again", white);
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

	times = TTF_OpenFont("times.ttf", 15);
	NotoSan = TTF_OpenFont("NotoSans.ttf", 15);

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
			DrawWord();
			SDL_RenderPresent(renderer);
		}
		else
		{
			PrintResut();
		}
}

void Logic()
{
	if (makeword == true) {
		chosenWord = ChosenWord();
		MakeGuess();
		makeword = false;
		SDL_Delay(500);
	}
	MakeFruitOne();
	MakeFruitTwo();
	SnakeMoveKeyboard();
	snakeWall();
	if (EatSnake() == false) {
		stop = true;
	}
	SnakeEatFruit();
	LoseGame();
	if (winner == 0)
	{
		stop = true;
	}
	else if (winner == 1)
	{
		score += 100;
		makeword = true;
		winner = 2;
	}
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