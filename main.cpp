#include <ncurses.h>
#include <map>
#include <list>

#include <chrono>
#include <thread>


namespace essencials {
    int x = 15;
    int y = 15;
    
    int numOfFoods = 5;
    bool debug = false;

    bool running = true;
    int lastRand = 0;
    
    int rand(int max) {
        lastRand = std::rand() % max;
        return lastRand;
    }
}

namespace player {
    std::list<std::list<int>> body = { { static_cast<int>(floor(essencials::x/2)), static_cast<int>(floor(essencials::y/2)) } };
    char direction = 'r';

    void changeDirection(char d) {
        direction = d;
    }

    void appendTail() {
        body.push_back({-1,-1});
    }
}

class Food {
    public:
        int x;
        int y;
        void Relocate();
};

namespace grid {
    int fps = 16;

    int points = 0;

    std::map<int,Food> foods;

    void addFood() {
        Food a;
        a.Relocate();
        foods[foods.size()] = a;
    }

    void step() {
        std::list<int> last = player::body.front();
        std::list<int> newHead = { 
            last.front()+=( (player::direction == 'r') ? 1 : ((player::direction == 'l') ? -1 : 0) ), 
            last.back()+= ( (player::direction == 'd') ? 1 : ((player::direction == 'u') ? -1 : 0) )
        };
        if (newHead.front() > essencials::x-1) {
            newHead.front() = 0;
        } else if (newHead.front() < 0) {
            newHead.front() = essencials::x-1;
        }
        if (newHead.back() > essencials::y-1) {
            newHead.back() = 0;
        } else if (newHead.back() < 0) {
            newHead.back() = essencials::y-1;
        }
        
        
        player::body.push_front(newHead);
        player::body.pop_back();
        for (int i = 0; i < foods.size()-1; i++) {
            if (player::body.front().front() == foods[i].x && player::body.front().back() == foods[i].y) {
                foods[i].Relocate();
                player::appendTail();
                points += 1;
            }
        }
    }
    
    void render() {
        if (points >= essencials::x*essencials::y-1) {
            mvprintw(0,0, "You won! Congratations!"); 
            mvprintw(1,0, "You ate %d apples, on a grid of %dx%d",points,essencials::x,essencials::y); 
            mvprintw(2,0, "Made by Ash");
            mvprintw(3,0, "press q or ctrl+C to quit");
        } else {
            for (int j = 0; j < 2; j++) {
                mvprintw(0,(essencials::x*2+1)*j, "+");
                for (int i = 0; i < essencials::x*2; i++) {
                    move(j*essencials::y+1*j,i+1);
                    printw("-");
                }
            }
            for (int j = 0; j < 2; j++) {
                mvprintw(essencials::y+1,(essencials::x*2+1)*j, "+");
                for (int i = 0; i < essencials::x; i++) {
                    move(i+1,j*essencials::y*2+1*j);
                    printw("|");
                }
            }
            
            mvprintw(1,50, "%d / %d", points, essencials::x*essencials::y-1);
            mvprintw(0,50, "press q or ctrl+C to quit");
            
            attron(COLOR_PAIR(2));
            for (std::list<int> i : player::body) {
                mvprintw(i.back()+1, i.front()*2+1,"##");
            }
            attron(COLOR_PAIR(3));
            mvprintw(player::body.front().back()+1,player::body.front().front()*2+1, "##");
            attron(COLOR_PAIR(1));
            for (int i = 0; i < foods.size()-1; i++) {
                mvprintw(foods[i].y+1, foods[i].x*2+1, "()");
            }
            attron(COLOR_PAIR(4));
        }
    }
}
void Food::Relocate() {
    bool obstacle = false;
    int nx;
    int ny;
    int loops = 0;
    do {
        loops += 1;
        obstacle = false;
        nx = essencials::rand(essencials::x);
        ny = essencials::rand(essencials::y);
        for (int i = 0; i < grid::foods.size()-1; i++) {
            if (grid::foods[i].x == nx && grid::foods[i].y == ny) {
                obstacle = true;
                break;
            }
        }
        for (std::list<int> i : player::body) {
            if (i.front() == nx && i.back() == ny) {
                obstacle = true;
                break;
            }
        }
        if (essencials::debug) {
            mvprintw(8,50, "%d", nx);
            mvprintw(8,55, "%d", ny);
            mvprintw(9,50, "%d", obstacle);
            refresh();
        }
        if (loops > 1000) {
            mvprintw(0,0, "if you're stuck here uhh yeah i made a while loop to "); 
            mvprintw(1,0, "randomize the apple's locations... so it might have either");
            mvprintw(2,0, "softlocked or you're really lucky... %d loops have passed...", loops);
            mvprintw(3,0, "press q or ctrl+C to quit");
            if (getch() == 113) {
                essencials::running = false;
                break;
            }
            refresh();
        }
    } while (obstacle && player::body.size() < essencials::x*essencials::y-essencials::numOfFoods);
    if ((player::body.size() < essencials::x*essencials::y-essencials::numOfFoods)) {
        x = nx;
        y = ny;
    } else {
        x = -1;
        y = -1;
    }
}

int main() {
    srand(time(nullptr));
    using namespace std::chrono_literals;
    using namespace std::this_thread;
    int key;
    std::chrono::system_clock::time_point time;
    for (int i = 0; i < essencials::numOfFoods; i++) {
        grid::addFood();
    }

    
    initscr();
    noecho();
    timeout(0);
    keypad(stdscr, TRUE);
    curs_set(0);

    start_color();
    init_pair(1, COLOR_RED, COLOR_RED);
    init_pair(2, COLOR_GREEN, COLOR_GREEN);
    init_pair(3, COLOR_CYAN, COLOR_CYAN);
    init_pair(4, COLOR_WHITE, COLOR_BLACK);
    
    do {
        time = std::chrono::system_clock::now();
        key = getch();
        clear();
        switch (key) {
            case 113:
                essencials::running = false;
                break;
            case 259:
                player::direction = 'u';
                break;
            case 258:
                player::direction = 'd';
                break;
            case 261:
                player::direction = 'r';
                break;
            case 260:
                player::direction = 'l';
                break;
            default:
                break;
        }
        grid::step();
        if (essencials::debug) {
            mvprintw(3,50,"key ID: %d", key);
            mvprintw(4,50, "player:");
            mvprintw(4,60, "%d", player::body.front().front());
            mvprintw(4,63, "%d", player::body.front().back());
            mvprintw(5,50, "apple:");
            mvprintw(5,60, "%d", grid::foods[0].x);
            mvprintw(5,63, "%d", grid::foods[0].y);
            mvprintw(6,50, "time: %d", time);
        }
        grid::render();
        refresh();
        sleep_for(150ms-(std::chrono::system_clock::now()-time));
    } while (essencials::running);

    keypad(stdscr, FALSE);
    endwin();
    return 0;
}
