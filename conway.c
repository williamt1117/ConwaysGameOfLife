#include<stdio.h>
#include<stdlib.h>
#include<ncurses.h>
#include<locale.h>
#include<time.h>

#define BOARDSIZEX 40
#define BOARDSIZEY 30
#define SPAWNPERCENT 20.0

#define PROGRAMRUNTIME 15 //measured in seconds
#define FPS 6

#define COLORMODE 0


typedef struct Canvas
{
    int sizeX;
    int sizeY;
    int value[BOARDSIZEX][BOARDSIZEY]; //age representation of cell
}Canvas;

void InitializeBoard(Canvas* myCanvas, int xsize, int ysize)
{
    myCanvas->sizeX = xsize;
    myCanvas->sizeY = ysize;
    for (int x = 0; x < xsize; x++)
    {
        for (int y = 0; y < ysize; y++)
        {
            myCanvas->value[x][y] = 0;
        }
    }
}

void PrintCharacter(int val)
{
    if (val == 0)
    {
        attron(COLOR_PAIR(val));
        printw("░░");
        attroff(COLOR_PAIR(val));
    }
    else
    {
        if (COLORMODE)
        {
            int new = val;
            if (val > 6)
                val == 6;
            attron(COLOR_PAIR(new));
            printw("▓▓");
            attroff(COLOR_PAIR(new));
        }
        else
        {
            printw("▓▓");
        }
    }
}

int CountNeighbors(Canvas* canvas, int x, int y)
{
    int neighborCount = 0;
    for (int i = x - 1; i <= x + 1; i++)
    {
        for (int j = y - 1; j <= y + 1; j++)
        {
            //If in bounds and on, add to array
            if (i >= 0 && i < BOARDSIZEX && j >= 0 && j < BOARDSIZEY)
            {
                if (canvas->value[i][j] >= 1)
                    if (x != i || y != j)
                        neighborCount++;
            }
        }
    }

    return neighborCount;
}

void PrintCanvas(Canvas* canvas)
{
    move(0, 0);
    for (int a = 0; a < canvas->sizeY; a++) 
    {
        for (int b = 0; b < canvas->sizeX; b++)
        {
            int x = b;
            int y = canvas->sizeY - 1 - a;
            PrintCharacter(canvas->value[x][y]);
        }
        printw("\n");
    }

}

void IterateBoard (Canvas* canvas)
{
    int neighborCount[BOARDSIZEX][BOARDSIZEY];

    for (int x = 0; x < BOARDSIZEX; x++)
    {
        for (int y = 0; y < BOARDSIZEY; y++)
        {
            neighborCount[x][y] = CountNeighbors(canvas, x, y);
        }
    }

    for (int x = 0; x < BOARDSIZEX; x++)
    {
        for (int y = 0; y < BOARDSIZEY; y++)
        {
            if (canvas->value[x][y] >= 1)
            {
                if (neighborCount[x][y] <= 1)
                {
                    canvas->value[x][y] = 0;
                }
                else if (neighborCount[x][y] <= 3)
                {
                    if (canvas->value[x][y] < 6)
                        canvas->value[x][y]++;
                }
                else if (neighborCount[x][y] <= 8)
                {
                    canvas->value[x][y] = 0;
                }
            }
            else
            {
                if (neighborCount[x][y] == 3)
                {
                    canvas->value[x][y]++;
                }
            }
        }
    }
}

void SetGliderSW(Canvas* canvas, int x, int y)
{
    canvas->value[x][y] = 1;
    canvas->value[x][y+1] = 1;
    canvas->value[x][y+2] = 1;
    canvas->value[x+1][y] = 1;
    canvas->value[x+2][y+1] = 1;
}

void RandomizeBoard(Canvas* canvas, float spawnpercent)
{
    for (int x = 0; x < BOARDSIZEX; x++)
    {
        for (int y = 0; y < BOARDSIZEY; y++)
        {
            canvas->value[x][y] = ((double)rand() / (double)RAND_MAX) <= spawnpercent;
        }
    }
}

int main()
{
    WINDOW* myWindow;
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    start_color();
    cbreak();
    timeout(10);
    srand(time(NULL));

    init_pair(0, COLOR_BLACK, COLOR_WHITE);
    init_pair(1, COLOR_BLACK, COLOR_RED);
    init_pair(2, COLOR_BLACK, COLOR_YELLOW);
    init_pair(3, COLOR_BLACK, COLOR_GREEN);
    init_pair(4, COLOR_BLACK, COLOR_BLUE);
    init_pair(5, COLOR_BLACK, COLOR_CYAN);
    init_pair(6, COLOR_BLACK, COLOR_MAGENTA);


    Canvas canvas;
    InitializeBoard(&canvas, BOARDSIZEX, BOARDSIZEY);

    RandomizeBoard(&canvas, SPAWNPERCENT / 100.0);
    //SetGliderSW(&canvas, BOARDSIZEX / 2, BOARDSIZEY / 2);

    int maxFrames = FPS * PROGRAMRUNTIME;
    int currentFrame = 0;

    while(currentFrame < maxFrames)
    {
        PrintCanvas(&canvas);
        refresh();
        IterateBoard(&canvas);
        napms(1000 / FPS);
        currentFrame++;
    }
    endwin();

    return 0;
}