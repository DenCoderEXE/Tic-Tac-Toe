// Denis Danilov

//#include <PDCurses-master/curses.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>

#define size 3
#define start_row 1
#define start_col 1
#define delta_x 2
#define delta_y 4

bool menu(WINDOW* win);

typedef struct Game
{
    char board[size][size];
    char player1;
    char player2;
    short cursor_x;
    short cursor_y;
    bool p2p_mode;
    bool gameStopped;
    bool xMove;
} Game;

Game game;

void setCursor(void) {
    move((game.cursor_x * delta_x) + start_row, (game.cursor_y * delta_y) + start_col);
}

void resetCursor(void) { // Move the cursor to the starting position
    game.cursor_x = 1;
    game.cursor_y = 1;
    setCursor();
}

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
    mvaddch(6, 0, ch);
    addstr("'s win!\n");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(1));
    addstr("Press any key to restart:\n");
    curs_set(0);
    refresh();
}

void printDraw(void) {
    attrset(A_BOLD);
    attrset(COLOR_PAIR(2));
    mvaddstr(6, 0, "Draw!\n");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(2));
    addstr("Press any key to restart:\n");
    curs_set(0);
    refresh();
}

bool end(void) {
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

void updateBoard(void) {
    mvaddch((game.cursor_x * delta_x) + start_row, (game.cursor_y * delta_y) + start_col, game.board[game.cursor_x][game.cursor_y]); // Add new symbol on the position
    setCursor(); // Move the cursor to this position
    refresh();
}

void printBoard(void) {
    clear();
    attrset(A_BOLD);
    mvaddstr(0, 0, "Tic-Tac-Toe\n");
    mvaddstr(1, 0, "   |   |   \n---|---|---\n   |   |   \n---|---|---\n   |   |   "); // Initial board
    if (game.xMove)
    {
        attrset(A_UNDERLINE);
        mvaddch(3, 13, game.player1); // Information on who is moving now
        attroff(A_UNDERLINE);
    }
    else
    {
        attrset(A_UNDERLINE);
        mvaddch(3, 13, game.player2); // Information on who is moving now
        attroff(A_UNDERLINE);
    }
    attroff(A_BOLD);
    refresh();
}

void resetBoard(void) {
    for (short row = 0; row < size; row++)
    {
        for (short col = 0; col < size; col++)
        {
            game.board[row][col] = ' ';
        }
    }
}

void printMenu(void) {
    clear();
    addstr("Tic-Tac-Toe\n");
    if (game.gameStopped)
    {
        addstr("  Resume \n");
        addstr(" New game \n");
    }
    else
    {
        addstr(" New game \n");
    }
    addstr(" Settings \n");
    addstr("   Quit \n");
    refresh();
}

void highlightActiveOption(short option) {
    if (game.gameStopped)
    {
        switch (option)
        {
        case 0:
            mvaddch(1, 1, '<');
            mvaddch(1, 1 + 7, '>');

            mvaddch(2, 0, ' ');
            mvaddch(2, 0 + 9, ' ');

            mvaddch(3, 0, ' ');
            mvaddch(3, 0 + 9, ' ');

            mvaddch(4, 2, ' ');
            mvaddch(4, 2 + 5, ' ');
            break;

        case 1:
            mvaddch(2, 0, '<');
            mvaddch(2, 0 + 9, '>');

            mvaddch(1, 1, ' ');
            mvaddch(1, 1 + 7, ' ');

            mvaddch(3, 0, ' ');
            mvaddch(3, 0 + 9, ' ');

            mvaddch(4, 2, ' ');
            mvaddch(4, 2 + 5, ' ');
            break;

        case 2:
            mvaddch(3, 0, '<');
            mvaddch(3, 0 + 9, '>');

            mvaddch(1, 1, ' ');
            mvaddch(1, 1 + 7, ' ');

            mvaddch(2, 0, ' ');
            mvaddch(2, 0 + 9, ' ');

            mvaddch(4, 2, ' ');
            mvaddch(4, 2 + 5, ' ');
            break;

        case 3:
            mvaddch(4, 2, '<');
            mvaddch(4, 2 + 5, '>');

            mvaddch(1, 1, ' ');
            mvaddch(1, 1 + 7, ' ');

            mvaddch(2, 0, ' ');
            mvaddch(2, 0 + 9, ' ');

            mvaddch(3, 0, ' ');
            mvaddch(3, 0 + 9, ' ');
            break;

        default:
            break;
        }
    }
    else
    {
        switch (option)
        {
        case 0:
            mvaddch(1, 0, '<');
            mvaddch(1, 0 + 9, '>');

            mvaddch(2, 0, ' ');
            mvaddch(2, 0 + 9, ' ');

            mvaddch(3, 2, ' ');
            mvaddch(3, 2 + 5, ' ');
            break;

        case 1:
            mvaddch(2, 0, '<');
            mvaddch(2, 0 + 9, '>');

            mvaddch(1, 0, ' ');
            mvaddch(1, 0 + 9, ' ');

            mvaddch(3, 2, ' ');
            mvaddch(3, 2 + 5, ' ');
            break;

        case 2:
            mvaddch(3, 2, '<');
            mvaddch(3, 2 + 5, '>');

            mvaddch(1, 0, ' ');
            mvaddch(1, 0 + 9, ' ');

            mvaddch(2, 0, ' ');
            mvaddch(2, 0 + 9, ' ');
            break;

        default:
            break;
        }
    }
    refresh();
}

void resumeGame(void) {
    if (!game.gameStopped)
    {
        return;
    }
    clear();
    printBoard();
    for (short row = 0; row < size; row++)
    {
        for (short col = 0; col < size; col++)
        {
            mvaddch((row * delta_x) + start_row, (col * delta_y) + start_col, game.board[row][col]);
        }
    }
    setCursor();
    curs_set(2);
}

void p2p(WINDOW* win) {
    while (true)
    {
        if (!end()) // End-of-game check
        {
            short pressed = wgetch(win);
            switch (pressed)
            {
            case 27: // ESC key
                if (menu(win))
                {
                    endwin();
                    exit(0);
                }
                break;

            case KEY_RIGHT:
                if (game.cursor_y < 2)
                {
                    game.cursor_y++;
                }
                setCursor(); // Moving the cursor to the next position
                break;

            case KEY_LEFT:
                if (game.cursor_y > 0)
                {
                    game.cursor_y--;
                }
                setCursor(); // Moving the cursor to the next position
                break;

            case KEY_UP:
                if (game.cursor_x > 0)
                {
                    game.cursor_x--;
                }
                setCursor(); // Moving the cursor to the next position
                break;

            case KEY_DOWN:
                if (game.cursor_x < 2)
                {
                    game.cursor_x++;
                }
                setCursor(); // Moving the cursor to the next position
                break;

            case 10: // ENTER key
                if (game.board[game.cursor_x][game.cursor_y] == ' ') // Check if the position is empty
                {
                    if (game.xMove)
                    {
                        game.board[game.cursor_x][game.cursor_y] = game.player1;
                        game.xMove = false;
                        attrset(A_UNDERLINE);
                        mvaddch(3, 13, game.player2); // Information on who is moving now
                        attroff(A_UNDERLINE);
                        updateBoard();
                    }
                    else
                    {
                        game.board[game.cursor_x][game.cursor_y] = game.player2;
                        game.xMove = true;
                        attrset(A_UNDERLINE);
                        mvaddch(3, 13, game.player1); // Information on who is moving now
                        attroff(A_UNDERLINE);
                        updateBoard();
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
                if (menu(win))
                {
                    endwin();
                    exit(0);
                }
                // endwin(); // End of the program
                // return 0;
                break;

            default:
                curs_set(2);
                clear();
                resetBoard();
                printBoard();
                resetCursor();
                break;
            }
        }
    }
}

void pve(WINDOW* win) {

}

void newGame(WINDOW* win) {
    game.cursor_x = 1, game.cursor_y = 1; // Cursor position
    game.xMove = true; // Moving player information
    game.gameStopped = true;

    resetBoard();
    printBoard();
    resetCursor();

    if (game.p2p_mode)
    {
        p2p(win);
    }
    else
    {
        pve(win);
    }
}

void highlightActiveSettingsOption(short option, WINDOW* win) {
    curs_set(0);
    mvaddch(1, 16, ' ');
    mvaddch(1, 20, ' ');
    mvaddch(2, 16, ' ');
    mvaddch(2, 20, ' ');
    mvaddch(3, 7, ' ');
    mvaddch(3, 12, ' ');
    if (option < 2)
    {
        mvaddch(option + 1, 16, '<');
        mvaddch(option + 1, 20, '>');
    }
    else
    {
        mvaddch(option + 1, 7, '<');
        mvaddch(option + 1, 12, '>');
    }
}

void settingsChange(short option) {
    curs_set(0);
    switch (option)
    {
    case 0:
        if (game.player1 == 'X')
        {
            game.player1 = 'O';
            game.player2 = 'X';
            game.gameStopped = false;
            mvaddch(1, 18, 'O');
            refresh();
        }
        else
        {
            game.player1 = 'X';
            game.player2 = 'O';
            game.gameStopped = false;
            mvaddch(1, 18, 'X');
            refresh();
        }
        break;

    case 1:
        if (game.p2p_mode)
        {
            game.p2p_mode = false;
            game.gameStopped = false;
            mvaddch(2, 18, 'N');
            refresh();
        }
        else
        {
            game.p2p_mode = true;
            game.gameStopped = false;
            mvaddch(2, 18, 'Y');
            refresh();
        }
        break;

    default:
        break;
    }
}

bool settings(WINDOW* win) {
    char tmp_player1 = game.player1;
    bool tmp_pvp_mode = game.p2p_mode;
    curs_set(0);
    clear();
    addstr("      Settings:\n");
    addstr("1.Player");
    addstr("\t<[");
    addch(game.player1);
    addstr("]>\n");
    addstr("PvP mode");
    addstr("\t [Y] \n");
    addstr("\tBack\t");
    refresh();
    short option = 0;
    while (true)
    {
        short pressed = wgetch(win);
        switch (pressed)
        {
        case KEY_DOWN:
            if (option == 2)
            {
                option = 0;
            }
            else
            {
                option++;
            }
            highlightActiveSettingsOption(option, win);
            break;

        case KEY_UP:
            if (option == 0)
            {
                option = 2;
            }
            else
            {
                option--;
            }
            highlightActiveSettingsOption(option, win);
            break;

        case KEY_LEFT:
            settingsChange(option);
            break;

        case KEY_RIGHT:
            settingsChange(option);
            break;

        case 10: // Enter
            if (option == 2)
            {
                goto exit_settings;
            }
            break;

        case 27:
            goto exit_settings;
            break;

        default:
            break;
        }
    }
exit_settings:
    if (tmp_player1 != game.player1 || tmp_pvp_mode != game.p2p_mode)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool menu(WINDOW* win) {
restart:
    curs_set(0);
    printMenu();
    highlightActiveOption(0);
    if (game.gameStopped)
    {
        short option = 0;
        while (true)
        {
            short pressed = wgetch(win);
            switch (pressed)
            {
            case KEY_DOWN:
                if (option == 3)
                {
                    option = 0;
                }
                else
                {
                    option++;
                }
                highlightActiveOption(option);
                break;

            case KEY_UP:
                if (option == 0)
                {
                    option = 3;
                }
                else
                {
                    option--;
                }
                highlightActiveOption(option);
                break;

            case 27: // ESC
                resumeGame();
                return false;
                break;

            case 10: // Enter
                switch (option)
                {
                case 0: // Resume
                    resumeGame();
                    return false;
                    break;

                case 1: // New game
                    curs_set(2);
                    newGame(win);
                    break;

                case 2: // Settings
                    if (settings(win)) // If need to restart menu
                    {
                        goto restart;
                    }
                    printMenu();
                    highlightActiveOption(option);
                    if (game.gameStopped)
                    {
                        highlightActiveOption(3);
                    }
                    highlightActiveOption(2);
                    break;

                case 3: // Exit
                    return true;
                    break;

                default:
                    break;
                }
                break;

            default:
                break;
            }
        }
    }
    else
    {
        short option = 0;
        while (true)
        {
            short pressed = wgetch(win);
            switch (pressed)
            {
            case KEY_DOWN:
                if (option == 2)
                {
                    option = 0;
                }
                else
                {
                    option++;
                }
                highlightActiveOption(option);
                break;

            case KEY_UP:
                if (option == 0)
                {
                    option = 2;
                }
                else
                {
                    option--;
                }
                highlightActiveOption(option);
                break;

            case 10: // Enter
                switch (option)
                {
                case 0: // New game
                    curs_set(2);
                    newGame(win);
                    break;

                case 1: // Settings
                    settings(win);
                    printMenu();
                    if (game.gameStopped)
                    {
                        highlightActiveOption(2);
                    }
                    highlightActiveOption(1);
                    break;

                case 2: // Exit
                    return true;
                    break;

                default:
                    break;
                }
                break;

            default:
                break;
            }
        }
    }
    return false;
}

void main(void) {
    WINDOW* win = initscr(); // Screen initialization
    noecho(); //Switch off echo
    cbreak(); //Disable inline buffering
    curs_set(2); // Set blinking curser
    keypad(win, true);
    if (can_change_color()) // Color initialization
    {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_GREEN);
        init_pair(2, COLOR_WHITE, COLOR_RED);
        init_pair(3, COLOR_WHITE, COLOR_CYAN);
    }

    game.player1 = 'X';
    game.player2 = 'O';
    game.gameStopped = false;
    game.p2p_mode = true;

    if (menu(win))
    {
        endwin();
        exit(0);
    }
}
