//Samuel Santiago
//Tommy King
//Project 2 Checkpoint 2
//q to quit
//directional keys are w,a,s,d not the arrows
//manual included at the bottom of the code

// This is better than texting
#include <iostream>
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>
#if defined(WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include "curses.h"
#include <vector>
#include <cstring>

using namespace std;

// wait funciton to slow down refresh loop
void wait()
{
    
#if defined(WIN32)
    Sleep(100); // speed
#else
    usleep(10000); // speed
#endif
    
}
// another wait fuciton for the die function that helps to display the die screen for a couple seconds before the game is quit without being able to move
void die_wait()
{
    
#if defined(WIN32)
    Sleep(1000000); // speed
#else
    usleep(10000000); // speed
#endif
    
}

// Displays our lovely ASCII art upon the untimely death of the worm, then refreshes the screen and waits before quitting
void die()
{
    //
    //YOU DIED
    //
    move(1 , 1);
    addstr(" __   _____  _   _   ____ ___ _____ ____ ");
    move(2 , 1);
    addstr(" \\ \\ / / _ \\| | | | |  _ \\_ _| ____|  _ \\ ");
    move(3 , 1);
    addstr("  \\ V / | | | | | | | | | | ||  _| | | | |");
    move(4 , 1);
    addstr("   | || |_| | |_| | | |_| | || |___| |_| |");
    move(5 , 1);
    addstr("   |_| \\___/ \\___/  |____/___|_____|____/");
    refresh();
    die_wait();
    
}

// The function for refreshing the border each time through the game loop
void DrawBorder(int score, time_t startTime)
{
    border(ACS_VLINE, ACS_VLINE,ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER); //using ncurses built in border characters and function
    
    move(0, 4); //moving to place Score
    addstr(" Score:   ");
    
    //Timer
    stringstream ss; // declares a stringstream for the outputed words to go in
    ss = stringstream();
    int playTime; //for the timer
    playTime = difftime(time(nullptr), startTime); // calculates the elapsed time
    ss = stringstream(); // new stringstream for timer
    ss << ' ' << setw(2) << setfill('0') << playTime / 3600 << ':' << setw(2) << playTime / 60 << ':' << setw(2) << playTime % 60 << ' '; //hrs minutes and seconds
    move(0 , COLS - 20); //place timer in proper positon
    addstr(ss.str().c_str()); // add timer
    
    //Score
    ss = stringstream(); //re-initializes the stringstream so it can also be used for the score
    move(0, 12);// position for the score
    ss << ' ' << setw(3) << score << ' '; // prints out the score
    addstr(ss.str().c_str());
    
}
// POS struct for positions on the game board
struct POS
{
    int l, c; // line and column
    POS(); // default method
    POS(int line, int col); // method with specific line and column
    bool operator==(const POS & other); // used for determining equivalence between POSes
    
};

POS::POS() // makes a randomly determined POS, used for food
{
    l = rand() % (LINES - 2) + 1;
    c = rand() % (COLS - 2) + 1;
}

POS::POS(int line, int col) // a specified POS by line and column number
{
    l = line;
    c = col;
}

bool POS::operator==(const POS & other) // defines the operator == to check to see if both l and c values are the same between two POSes
{
    return(l == other.l) && (c == other.c);
}

// Our Food struct, includes a position, a value, a default method and a render method
struct Food
{
    POS p;
    int v;
    Food();
    void Render();
};

// generates a food of random value and random position using POS()
Food::Food()
{
    v = rand() % 5 + 5;
    p = POS();
}

// Renders the Food on the screen
void Food::Render()
{
    move(p.l, p.c);
    stringstream ss;
    ss << v;
    addstr(ss.str().c_str());
    
}
// Our Worm Struct, includes a vector of segments of positions, a render method, an initialize method, also several bool checks to see if the worm lands on anything. Also has the move method
struct Worm
{
    Worm();
    vector<POS> segments;
    void Render();
    void Initialize();
    
    bool landed_on_food(POS food);
    bool landed_on_border();
    bool landed_on_self();
    
    void move_worm(char c);
    
};

// I guess there needs to be a default method but this doesn't do anything in our case
Worm::Worm()
{
    
}

// initializes the worm in the middle of the screen and to the left of the head, 6 segments long by pushing back new POSes into the segments vector
void Worm::Initialize()
{
    POS p = POS(LINES/2, COLS/2);
    segments.push_back(p);
    const int WORM_LENGTH = 6;
    for (int i = 0; i < WORM_LENGTH; i++)
    {
        p.c = p.c - 1;
        segments.push_back(p);
    }
    
}

// checks to see if the head of the worm lands on a piece of food
bool Worm::landed_on_food(POS food)
{
    bool rv = false;
    if(segments[0] == food)
    {
        rv = true;
    }
    return rv;
}

// checks to see if the head of the worm lands on any of the four border lines around the edge
bool Worm::landed_on_border()
{
    bool rv = false;
    for (int i = 0; i < COLS; i++)
    {
        
        if (segments[0] == POS(0,i))
        {
            rv = true;
            break;
        }
        else if (segments[0] == POS(i,0))
        {
            rv = true;
            break;
        }
        else if (segments[0] == POS(i,COLS - 1))
        {
            rv = true;
            break;
        }
        else if (segments[0] == POS(LINES - 1, i))
        {
            rv = true;
            break;
        }
        
    }
    return rv;
}

// checks to see if the head of the worm lands on any other segment of the worm
bool Worm::landed_on_self()
{
    bool rv = false;
    for (int i = 1; i < segments.size(); i++)
    {
        if (segments[0] == segments[i])
        {
            rv = true;
            break;
        }
    }
    return rv;
}

// our move method
void Worm::move_worm(char d)
{
    POS old_position = segments[0]; // sets the inital postion of the head
    int dl, dc = 0;
    
    for (size_t i = segments.size() - 1 ; i > 0; i--) // puts each segment where the one before it is
    {
        segments[i] = segments[i-1];
    }
    
    switch(d) // increments the delta direction based on which key is pressed
    {
            case 'w':
                dl--;
                break;
            case 's':
                dl++;
                break;
            case 'a':
                dc--;
                break;
            case 'd':
                dc++;
                break;
    }
    
    segments[0].l = old_position.l + dl; // assigns the new head position
    segments[0].c = old_position.c + dc;
    
    
}

// takes input from the user, q quits the game, the directional keys w,a,s and d call the move method in each direciton
bool ProcessKeyboard(char c, Worm & w)
{
    bool rv = false;
    switch(c)
    {
        case 'q':
            rv = true;
            break;
        case 'w':
            w.move_worm('w');
            break;
        case 'a':
            w.move_worm('a');
            break;
        case 's':
            w.move_worm('s');
            break;
        case 'd':
            w.move_worm('d');
            break;
    }
    return rv;
    
}
// Renders the worm on the screen by moving to and addch-ing each member of the segments vector
void Worm::Render()
{
    
    for (size_t i = 0; i < segments.size(); i++)
    {
        move(segments.at(i).l, segments.at(i).c);
        addch((i == 0) ? '@' : 'o');
    }
}

int main()
{
    initscr();
    cbreak();// non blocking input
    noecho(); //turns off output on screen
    nodelay(stdscr, 1); // non blocking input
    curs_set(0); //curser
    
    time_t startTime = time(nullptr); //current time
    int score = 0;// initializes the score to 0
    srand((unsigned int)time(nullptr)); //seeds the rand function
    
    Food f; // declares the first food
    Worm w; // our good ole worm
    w.Initialize(); // sets the inital position of the worm
    
    while (true)
    {
        char c = getch();
        if (ProcessKeyboard(c, w))
        {
            break;
        }
        
        clear();// clears the old screen each time through the loop
        DrawBorder(score, startTime); // draws the updated border each time with new time and possible score
        f.Render(); // draws the food on the screen
        w.Render(); // draws the worm on the screen
        //w.move_worm(c);
    
        if(w.landed_on_border() || w.landed_on_self()) // if the worm hits either the border or itself, the die function is called which has a built in wait period which is followed by a break, quitting the game
        {
            die();
            break;
        }
        
        if(w.landed_on_food(f.p)) // checks to see if the worm lands on a food
        {
            score = score + f.v; // adds the value of the food to the score
            
            POS p; // declares a temp POS to use for push back purposes
            for (int i = 1; i < f.v + 1; i++) // goes through a loop the number of times specified by the value of the food that was landed on, pushing back that number of segments onto the worm
            {
                p.l = w.segments[w.segments.size()-1].l;
                p.c = w.segments[w.segments.size()-1].c - 1; //push backs the number of segments onto the worm as the value of the food
                w.segments.push_back(p);
            }
            Food g; // declares a new food to be displayed
            f.p = g.p; // changes the position of f so it will show up in a new location the next time through the loop
            f.v = g.v; // changes the value of f
            continue; // continues
        }
        wait(); // slows down

    }
    endwin(); // quits game
}

/*
 Welcome to your brand new worm game!
 
 For those of you who have never played games before, you're in for a treat
 
 
 Movement is simple in this game, use the characters 'w' 'a' 's' and 'd' to go up, left, down, and right in that order.
 
 feeling extremely frustrated? Press 'q' to quit and keep the change!
 
 View LIVE the amount of time you've wasted away at the computer playing our game with the fantastic 'timer' feature located in the top right corner of the screen!
 
 Theoretically the worm should grow each time you eat some food, however our top notch development team prefers an air of mystery in our games, who knows what's going to happen as you move around the screen, embrace the unknown!
 
 See your score in the top LEFT corner of the screen!
*/


