// Denis Danilov

#include <PDCurses-master/curses.h>
//#include <ncurses.h>
#include <stdbool.h>

#define size 3
#define start_row 2
#define start_col 1
#define delta_x 2
#define delta_y 4

typedef struct Game
{
    char board[size][size];
} Game;

Game game;

void highlightWinner(short x1, short y1, short x2, short y2, short x3, short y3) {
    attrset(A_BLINK);
    attrset(COLOR_PAIR(3));
    mvaddch((x1 * delta_x) + start_row, (y1 * delta_y) + start_col, game.board[x1][y1]);
    mvaddch((x2 * delta_x) + start_row, (y2 * delta_y) + start_col, game.board[x2][y2]);
    mvaddch((x3 * delta_x) + start_row, (y3 * delta_y) + start_col, game.board[x3][y3]);
    attroff(A_BLINK);
    attroff(COLOR_PAIR(3));
    refresh();
}

void printWinner(char ch) {
    attrset(A_BOLD);
    attrset(COLOR_PAIR(1));
    mvaddch(8, 0, ch);
    addstr("'s win!\n");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(1));
    addstr("Press any key to restart:\n");
    refresh();
}

void printDraw() {
    attrset(A_BOLD);
    attrset(COLOR_PAIR(2));
    mvaddstr(8, 0, "Draw!\n");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(1));
    addstr("Press any key to restart:\n");
    refresh();
}

bool end() {
    for (short row = 0; row < size; row++) // Checking for a combination in a row
    {
        if ((game.board[row][0] == game.board[row][1] && game.board[row][1] == game.board[row][2]) && game.board[row][0] != ' ')
        {
            highlightWinner(row, 0, row, 1, row, 2);
            printWinner(game.board[row][0]);
            return true;
        }
    }
    for (short col = 0; col < size; col++) // Checking for a combination in a column
    {
        if ((game.board[0][col] == game.board[1][col] && game.board[1][col] == game.board[2][col]) && game.board[0][col] != ' ')
        {
            highlightWinner(0, col, 1, col, 2, col);
            printWinner(game.board[0][col]);
            return true;
        }
    }
    if (game.board[0][0] == game.board[1][1] && game.board[1][1] == game.board[2][2] && game.board[1][1] != ' ') // Checking for a combination in a primary diagonal
    {
        highlightWinner(0, 0, 1, 1, 2, 2);
        printWinner(game.board[1][1]);
        return true;
    }
    else if (game.board[0][2] == game.board[1][1] && game.board[1][1] == game.board[2][0] && game.board[1][1] != ' ') // Checking for a combination in a secondary diagonal
    {
        highlightWinner(0, 2, 1, 1, 2, 0);
        printWinner(game.board[1][1]);
        return true;
    }

    short n = 0;
    for (short row = 0; row < size; row++) // Check if there are no empty positions left
    {
        for (short col = 0; col < size; col++)
        {
            if (game.board[row][col] == ' ')
            {
                n++;
            }
        }
    }
    if (n == 0)
    {
        printDraw();
        return true;
    }
    return false;
}

void updateBoard(short x, short y) {
    mvaddch((x * delta_x) + start_row, (y * 4) + start_col, game.board[x][y]); // Add new symbol on the position
    move((x * delta_x) + start_row, (y * 4) + start_col); // Move the cursor to this position
    refresh();
}

void printBoard() {
    attrset(A_BOLD);
    mvaddstr(0, 0, "Tic-tac-toe\n");
    mvaddstr(2, 0, "   |   |   \n---|---|---\n   |   |   \n---|---|---\n   |   |   "); // Initial board
    attrset(A_UNDERLINE);
    mvaddch(4, 13, 'X'); // Information on who is moving now
    attroff(A_UNDERLINE);
    attroff(A_BOLD);
    refresh();
}

void resetBoard() {
    for (short row = 0; row < size; row++)
    {
        for (short col = 0; col < size; col++)
        {
            game.board[row][col] = ' ';
        }
    }
}

int main(void) {
    short x = 1, y = 1; // Cursor position
    bool X_move = true; // Moving player information
    WINDOW* win = initscr(); // Screen initialization
    noecho();
    cbreak();
    keypad(win, true);
    if (can_change_color()) // Color initialization
    {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_GREEN);
        init_pair(2, COLOR_WHITE, COLOR_RED);
        init_pair(3, COLOR_WHITE, COLOR_CYAN);
    }
    
    resetBoard();
    printBoard();
    move((1 * delta_x) + start_row, (1 * delta_y) + start_col); // Move the cursor to the starting position

    while (true)
    {
        if (!end()) // End-of-game check
        {
            short pressed = wgetch(win);
            switch (pressed)
            {
            case 27: // ESC key
                endwin();
                return 0;
                break;

            case KEY_RIGHT:
                if (y < 2)
                {
                    y++;
                }
                move((x * delta_x) + start_row, (y * delta_y) + start_col); // Moving the cursor to the next position
                break;
                
            case KEY_LEFT:
                if (y > 0)
                {
                    y--;
                }
                move((x * delta_x) + start_row, (y * delta_y) + start_col); // Moving the cursor to the next position
                break;

            case KEY_UP:
                if (x > 0)
                {
                    x--;
                }
                move((x * delta_x) + start_row, (y * delta_y) + start_col); // Moving the cursor to the next position
                break;

            case KEY_DOWN:
                if (x < 2)
                {
                    x++;
                }
                move((x * delta_x) + start_row, (y * delta_y) + start_col); // Moving the cursor to the next position
                break;

            case 10: // ENTER key
                if (game.board[x][y] == ' ') // Check if the position is empty
                {
                    if (X_move)
                    {
                        game.board[x][y] = 'X';
                        X_move = false;
                        attrset(A_UNDERLINE);
                        mvaddch(4, 13, 'O'); // Information on who is moving now
                        attroff(A_UNDERLINE);
                        updateBoard(x, y);
                    }
                    else
                    {
                        game.board[x][y] = 'O';
                        X_move = true;
                        attrset(A_UNDERLINE);
                        mvaddch(4, 13, 'X'); // Information on who is moving now
                        attroff(A_UNDERLINE);
                        updateBoard(x, y);
                    }
                }
                else
                {
                    beep(); // Illegal move signal
                }
                break;
            
            default:
                break;
            }
        }
        else
        {
            short pressed = wgetch(win);
            switch (pressed)
            {
            case 27: // ESC key
                endwin(); // End of the program
                return 0;
                break;
            
            default:
                clear();
                resetBoard();
                printBoard();
                move((1 * 2) + start_row, (1 * 4) + start_col); // Move the cursor to the starting position
                x = 1, y = 1; // Reset coordinates
                break;
            }
        }
    }
}