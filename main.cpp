#include<iostream>
#include<vector>
#include<conio.h>
#include<windows.h>
using namespace std;

#define ROW_COUNT 20
#define COL_COUNT 20
#define MAX_SNAKE_LENGTH (ROW_COUNT*COL_COUNT)

enum eDirection {
    STOP=0,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    PAUSE
};

class Coord
{
    public:
    int x;
    int y;
        Coord(int x, int y)
        {
            this->x = x;
            this->y = y;
        }

        bool operator==(Coord rhs)
        {
            return ((this->x == rhs.x) && (this->y == rhs.y));
        }
};

class Snake
{
    Coord head;
    Coord body;
    Coord tail;
    int ts;     // timestamp used to set the order of appearance of the snake body/tail
    vector <vector <int> > isSnakeBody;     // to keep track of the board positions where the snake body/tail is present

    public:
        Snake():head(COL_COUNT/2, ROW_COUNT/2),body((COL_COUNT/2) + 1, ROW_COUNT/2),tail((COL_COUNT/2) + 2, ROW_COUNT/2)
        {
            ts = 0;
            isSnakeBody.resize(ROW_COUNT, vector<int>(COL_COUNT, 0));
            setSnakeAtPos(tail, true);
            setSnakeAtPos(body, true);
        }

        /*
        // display() function for debugging
        void display()
        {
            cout << endl;

            for(int i = 0 ; i < ROW_COUNT ; i++)
            {
                for(int j = 0 ; j < COL_COUNT ; j++)
                {
                    cout << isSnakeBody[i][j] << " ";
                }
                cout << endl;
            }

            cout << "\nhead: " << isSnakeBody[head.y][head.x];
            cout << "\nbody: " << isSnakeBody[body.y][body.x];
            cout << "\ntail: " << isSnakeBody[tail.y][tail.x];
            cout << "\nts: " << ts << endl;
        }
        */

        Coord getHead()
        {
            return head;
        }

        void setHead(Coord head)
        {
            this->head = head;
        }

        Coord getTail()
        {
            return tail;
        }

        void setTail(Coord tail)
        {
            this->tail = tail;
        }

        bool isSnakeBodyAtPos(Coord pos)
        {
            return (isSnakeBody[pos.y][pos.x] > 0);
        }

        void setSnakeAtPos(Coord pos, bool snakeBody)
        {
            if(snakeBody)
                isSnakeBody[pos.y][pos.x] = ts = (ts % MAX_SNAKE_LENGTH) + 1;
            else
                isSnakeBody[pos.y][pos.x] = 0;
        }

        void wrapAroundCoord(Coord& pos)
        {
            if(pos.x < 0)
                pos.x = COL_COUNT - 1;
            else if(pos.x > COL_COUNT - 1)
                pos.x = 0;
            if(pos.y < 0)
                pos.y = ROW_COUNT - 1;
            else if(pos.y > ROW_COUNT - 1)
                pos.y = 0;
        }

        int getSnakeAtPos(Coord pos, int xdel = 0, int ydel = 0)
        {
            pos.x += xdel;
            pos.y += ydel;

            wrapAroundCoord(pos);

            return isSnakeBody[pos.y][pos.x];
        }

        bool isHeadCrashed(Coord& newHead, char ch)
        {
            Coord headBeforeWrap = newHead;
            wrapAroundCoord(newHead);
            bool hitBody = isSnakeBodyAtPos(newHead);   // check for both 'E' and 'H' modes

            if(ch == 'E' || hitBody)
            {
                return hitBody;
            }

            // check only for 'H' mode
            bool hitWall = (headBeforeWrap.x < 0) || (headBeforeWrap.x > COL_COUNT-1) || (headBeforeWrap.y < 0) || (headBeforeWrap.y > ROW_COUNT-1);

            return hitWall;
        }

        bool updateHeadAndCheckCrash(eDirection dira, char ch)
        {
            Coord oldHead = getHead();
            setSnakeAtPos(oldHead,true);

            Coord newHead = oldHead;

            switch (dira)
            {
                case LEFT:  newHead.x--;
                            break;
                case RIGHT: newHead.x++;
                            break;
                case UP:    newHead.y--;
                            break;
                case DOWN:  newHead.y++;
                            break;
                default:    break;
            }

            bool crashed = isHeadCrashed(newHead,ch);
            setHead(newHead);
            return crashed;
        }

        void updateTail()
        {
            Coord currTail = getTail();
            int prevTailVal = (getSnakeAtPos(currTail) % MAX_SNAKE_LENGTH) + 1;
            setSnakeAtPos(currTail,false);

            if(getSnakeAtPos(currTail,-1,0) == prevTailVal)
                currTail.x--;
            else if(getSnakeAtPos(currTail,1,0) == prevTailVal)
                currTail.x++;
            else if(getSnakeAtPos(currTail,0,-1) == prevTailVal)
                currTail.y--;
            else if(getSnakeAtPos(currTail,0,1) == prevTailVal)
                currTail.y++;

            wrapAroundCoord(currTail);
            setTail(currTail);
        }
};

class Fruit
{
    Coord fruitPos;
    public:
        Fruit():fruitPos(COL_COUNT/4, ROW_COUNT/4)
        {
            // no more initializations needed here
        }

        void generateFruit(Snake snake)
        {
            do
            {
                fruitPos.x = rand() % COL_COUNT;
                fruitPos.y = rand() % ROW_COUNT;
            }while((fruitPos == snake.getHead()) || (snake.isSnakeBodyAtPos(fruitPos)));
        }

        bool isFruitAtPos(Coord pos)
        {
            return (fruitPos == pos);
        }
};

class Game
{
    Snake snake;
    Fruit fruit;
    eDirection dira;
    eDirection prev;
    bool gameOver;
    int score;
    char ch;

    public:
        Game()
        {
            dira = prev = STOP;
            gameOver = false;
            score = 0;
            ch = 'E';
        }

        void drawFrame()
        {
            if(dira == PAUSE)
                return;

            system("cls");
            cout << "\n\t\t      SNAKE GAME\n";
            cout << "\t\t";

            for(int i = 0 ; i < COL_COUNT - 8 ; i++)
            {
                cout << "##";
            }
            cout << endl;
            for(int i = 0 ; i < ROW_COUNT ; i++)
            {
                for(int j = 0 ; j < COL_COUNT ; j++)
                {
                    if(j == 0)
                    {
                        cout << "\t\t##";
                    }

                    Coord pos(j,i);

                    if(snake.getHead() == pos)
                        cout << "@";
                    else if(fruit.isFruitAtPos(pos))
                        cout << "X";
                    else
                    {
                        if(snake.isSnakeBodyAtPos(pos))
                            cout << "*";
                        else
                            cout << " ";
                    }

                    if (j == COL_COUNT - 1)
                        cout << "##";
                }
                cout << endl;
            }
            cout << "\t\t";
            for(int i = 0 ; i < COL_COUNT - 8 ; i++)
            {
                cout << "##";
            }
            cout << endl;

            cout << "\t\t       Score: " << score << endl;
            //snake.display();
        }

        void getUserInput()
        {
            if(_kbhit())
            {
                switch(_getch())
                {
                    case 'a':   if(prev != RIGHT && dira != PAUSE)
                                    dira=LEFT;
                                break;
                    case 'd':   if(prev != LEFT && dira != PAUSE)
                                    dira=RIGHT;
                                break;
                    case 'w':   if(prev != DOWN && dira != PAUSE)
                                    dira=UP;
                                break;
                    case 's':   if(prev != UP && dira != PAUSE)
                                    dira=DOWN;
                                break;
                    case 'p':   dira = (dira == PAUSE) ? prev : PAUSE;
                                break;
                    case 'x':   gameOver=true;
                                break;
                }
            }
            if(dira != PAUSE && dira != STOP)
                prev = dira;
        }

        void logic()
        {
            if(dira == STOP || dira == PAUSE)
                return;

            bool crashed = snake.updateHeadAndCheckCrash(dira, ch);

            if(crashed)
            {
                gameOver = true;
            }

            if(fruit.isFruitAtPos(snake.getHead()))
            {
                score += 10;
                fruit.generateFruit(snake);
            }
            else
            {
                snake.updateTail();
            }
        }

        void play()
        {
            cout << "\n*****************************************\n";
            cout << "\t\tSNAKE GAME";
            cout << "\n*****************************************\n\n";
            cout << "Enter game mode which you want to play:\n\n";
            cout << "1. Press E for Easy mode\n\n";
            cout << "2. Press H for Hard mode\n\n";
            cout << "3. Press Q to Quit\n\n";

            do
            {
                cout<<"Your Choice: ";
                cin>>ch;
                ch = toupper(ch);
                switch (ch)
                {
                    case 'E':
                    case 'H':
                        while(!gameOver)
                        {
                            drawFrame();
                            getUserInput();
                            logic();
                            _sleep(30);
                        }
                        cout << "GAME OVER!!!";
                        break;
                    case 'Q': break;
                    default:
                        cout << "Invalid choice! Please select again.\n";
                }
            }while(ch != 'Q' && !gameOver);
        }
};

int main()
{
    Game newGame;

    newGame.play();

	getch();
	return 0;
}

