#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <time.h>
#include "../include/tetris.h"

void panic(const char *message) {
        fprintf(stderr, "%s", message);
        exit(-1);
}

void generate_new_next_piece(GameState* game_state) {
        u32 piece = random() % 6;
        switch (piece) {
        case 0: game_state->next_piece = SquareShape; break;
        case 1: game_state->next_piece = LShape; break;
        case 2: game_state->next_piece = ZShape; break;
        case 3: game_state->next_piece = IShape; break;
        case 4: game_state->next_piece = ReverseZShape; break;
        case 5: game_state->next_piece = TShape; break;
        }

        u32 color = random() % 4;
        switch (color) {
        case 0: game_state->next_color = Red; break;
        case 1: game_state->next_color = Blue; break;
        case 2: game_state->next_color = Green; break;
        case 3: game_state->next_color = Yellow; break;
        }
}

void init_gamestate(GameState* game_state) {
        game_state->score = 0;
        // TODO: array initializer
        for (u32 i = 0; i < BOARD_WIDTH; ++i) {
                for (u32 j = 0; j < BOARD_HEIGHT; ++j) {
                        game_state->board[i][j] = Empty;
                }
        } 
        generate_new_next_piece(game_state);
        game_state->current_piece = game_state->next_piece;
        game_state->current_color = game_state->next_color;
        generate_new_next_piece(game_state);
        game_state->current_location.x = BOARD_WIDTH / 2;
        game_state->current_location.y = 3;
        game_state->current_rotation = 0;
}

u32 color_to_color_pair(Color color) {
        switch (color) {
        case Empty: return EMPTY_PAIR;
        case Red: return RED_PAIR;
        case Blue: return BLUE_PAIR;
        case Green: return GREEN_PAIR;
        case Yellow: return YELLOW_PAIR;
        default: panic("invalid color"); return -1;
        }
}

GameOutcome evaluate_gamestate(GameState *game_state) {
        for ( u32 j = 0; j < BOARD_WIDTH; ++j ) {
                if (game_state->board[j][3] != Empty) {
                        return Lose;
                }
        }
        return Playing;
}

PlayerInput get_player_input_once() {
        int ch = getch();
        PlayerInput out = NoInput;
        switch (ch) {
        case KEY_DOWN:
                out |= MoveDown;
                break;
        case KEY_LEFT:
                out |= MoveLeft;
                break;
        case KEY_RIGHT:
                out |= MoveRight;
                break;
        case 'a':
                out |= RotateLeft;
                break;
        case 'e':
                out |= RotateRight;
                break;
        case KEY_EXIT:
                out |= QuitRequested;
                break;
        case 'p':
                out |= PauseRequested;
                break;
        case ' ':
                out |= DropPiece;
                break;
        }

        return out;
}

PlayerInput get_player_input() {
        PlayerInput out = NoInput;
        PlayerInput next;

        while ( (next = get_player_input_once()) ) {
                out |= next;
        }
        return out;
}

void pause() { }

PointCloud get_points_for_current_piece(GameState *game_state) {
        PointCloud out;
        switch (game_state->current_piece) {
        case SquareShape:
                out.points[0].x = game_state->current_location.x;
                out.points[0].y = game_state->current_location.y - 1;
                out.points[1].x = game_state->current_location.x + 1;
                out.points[1].y = game_state->current_location.y - 1;
                out.points[2].x = game_state->current_location.x;
                out.points[2].y = game_state->current_location.y;
                out.points[3].x = game_state->current_location.x + 1;
                out.points[3].y = game_state->current_location.y;
                break;
        case LShape:
                switch (game_state->current_rotation) {
                case 0:
                        // X
                        // X X X
                        out.points[0].x = game_state->current_location.x - 1;
                        out.points[0].y = game_state->current_location.y - 1;
                        out.points[1].x = game_state->current_location.x - 1;
                        out.points[1].y = game_state->current_location.y;
                        out.points[2].x = game_state->current_location.x;
                        out.points[2].y = game_state->current_location.y;
                        out.points[3].x = game_state->current_location.x + 1;
                        out.points[3].y = game_state->current_location.y;
                        break;
                case 1: case -3:
                        // X X
                        // X
                        // X
                        out.points[0].x = game_state->current_location.x;
                        out.points[0].y = game_state->current_location.y - 2;
                        out.points[1].x = game_state->current_location.x + 1;
                        out.points[1].y = game_state->current_location.y - 2;
                        out.points[2].x = game_state->current_location.x;
                        out.points[2].y = game_state->current_location.y - 1;
                        out.points[3].x = game_state->current_location.x;
                        out.points[3].y = game_state->current_location.y;
                        break;
                case 2: case -2:
                        // X X X
                        //     X
                        out.points[0].x = game_state->current_location.x - 1;
                        out.points[0].y = game_state->current_location.y - 1;
                        out.points[1].x = game_state->current_location.x;
                        out.points[1].y = game_state->current_location.y - 1;
                        out.points[2].x = game_state->current_location.x + 1;
                        out.points[2].y = game_state->current_location.y - 1;
                        out.points[3].x = game_state->current_location.x + 1;
                        out.points[3].y = game_state->current_location.y;
                        break;
                case 3: case -1:
                        //   X
                        //   X
                        // X X
                        out.points[0].x = game_state->current_location.x + 1;
                        out.points[0].y = game_state->current_location.y - 2;
                        out.points[1].x = game_state->current_location.x + 1;
                        out.points[1].y = game_state->current_location.y - 1;
                        out.points[2].x = game_state->current_location.x;
                        out.points[2].y = game_state->current_location.y;
                        out.points[3].x = game_state->current_location.x + 1;
                        out.points[3].y = game_state->current_location.y;
                        break;
                default:
                        panic("invalid rotation");
                }
                break;
        case ZShape:
                switch (game_state->current_rotation) {
                case 0: case 2: case -2:
                        // X X
                        //   X X
                        out.points[0].x = game_state->current_location.x - 1;
                        out.points[0].y = game_state->current_location.y - 1;
                        out.points[1].x = game_state->current_location.x;
                        out.points[1].y = game_state->current_location.y - 1;
                        out.points[2].x = game_state->current_location.x;
                        out.points[2].y = game_state->current_location.y;
                        out.points[3].x = game_state->current_location.x + 1;
                        out.points[3].y = game_state->current_location.y;
                        break;
                case 1: case -1: case 3: case -3:
                        //   X
                        // X X
                        // X
                        out.points[0].x = game_state->current_location.x + 1;
                        out.points[0].y = game_state->current_location.y - 2;
                        out.points[1].x = game_state->current_location.x;
                        out.points[1].y = game_state->current_location.y - 1;
                        out.points[2].x = game_state->current_location.x + 1;
                        out.points[2].y = game_state->current_location.y - 1;
                        out.points[3].x = game_state->current_location.x;
                        out.points[3].y = game_state->current_location.y;
                        break;
                default:
                        panic("invalid rotation");
                }
                break;
        case IShape:
                switch (game_state->current_rotation) {
                case 0: case 2: case -2:
                        // X X X X
                        out.points[0].x = game_state->current_location.x - 1;
                        out.points[0].y = game_state->current_location.y;
                        out.points[1].x = game_state->current_location.x;
                        out.points[1].y = game_state->current_location.y;
                        out.points[2].x = game_state->current_location.x + 1;
                        out.points[2].y = game_state->current_location.y;
                        out.points[3].x = game_state->current_location.x + 2;
                        out.points[3].y = game_state->current_location.y;
                        break;
                case 1: case -1: case 3: case -3:
                        // X
                        // X
                        // X
                        // X
                        out.points[0].x = game_state->current_location.x;
                        out.points[0].y = game_state->current_location.y - 3;
                        out.points[1].x = game_state->current_location.x;
                        out.points[1].y = game_state->current_location.y - 2;
                        out.points[2].x = game_state->current_location.x;
                        out.points[2].y = game_state->current_location.y - 1;
                        out.points[3].x = game_state->current_location.x;
                        out.points[3].y = game_state->current_location.y;
                        break;
                default:
                        panic("invalid rotation");
                }
                break;
        case ReverseZShape:
                switch (game_state->current_rotation) {
                case 0: case 2: case -2:
                        //   X X
                        // X X
                        out.points[0].x = game_state->current_location.x;
                        out.points[0].y = game_state->current_location.y - 1;
                        out.points[1].x = game_state->current_location.x + 1;
                        out.points[1].y = game_state->current_location.y - 1;
                        out.points[2].x = game_state->current_location.x - 1;
                        out.points[2].y = game_state->current_location.y;
                        out.points[3].x = game_state->current_location.x;
                        out.points[3].y = game_state->current_location.y;
                        break;
                case 1: case -1: case 3: case -3:
                        // X
                        // X X
                        //   X
                        out.points[0].x = game_state->current_location.x;
                        out.points[0].y = game_state->current_location.y - 2;
                        out.points[1].x = game_state->current_location.x;
                        out.points[1].y = game_state->current_location.y - 1;
                        out.points[2].x = game_state->current_location.x + 1;
                        out.points[2].y = game_state->current_location.y - 1;
                        out.points[3].x = game_state->current_location.x + 1;
                        out.points[3].y = game_state->current_location.y;
                        break;
                default:
                        panic("invalid rotation");
                }
                break;
        case TShape:
                switch (game_state->current_rotation) {
                case 0:
                        //   X
                        // X X X
                        out.points[0].x = game_state->current_location.x;
                        out.points[0].y = game_state->current_location.y - 1;
                        out.points[1].x = game_state->current_location.x - 1;
                        out.points[1].y = game_state->current_location.y;
                        out.points[2].x = game_state->current_location.x;
                        out.points[2].y = game_state->current_location.y;
                        out.points[3].x = game_state->current_location.x + 1;
                        out.points[3].y = game_state->current_location.y;
                        break;
                case 1: case -3:
                        // X
                        // X X
                        // X
                        out.points[0].x = game_state->current_location.x;
                        out.points[0].y = game_state->current_location.y - 2;
                        out.points[1].x = game_state->current_location.x;
                        out.points[1].y = game_state->current_location.y - 1;
                        out.points[2].x = game_state->current_location.x + 1;
                        out.points[2].y = game_state->current_location.y - 1;
                        out.points[3].x = game_state->current_location.x;
                        out.points[3].y = game_state->current_location.y;
                        break;
                case 2: case -2:
                        // X X X
                        //   X
                        out.points[0].x = game_state->current_location.x - 1;
                        out.points[0].y = game_state->current_location.y - 1;
                        out.points[1].x = game_state->current_location.x;
                        out.points[1].y = game_state->current_location.y - 1;
                        out.points[2].x = game_state->current_location.x + 1;
                        out.points[2].y = game_state->current_location.y - 1;
                        out.points[3].x = game_state->current_location.x;
                        out.points[3].y = game_state->current_location.y;
                        break;
                case 3: case -1:
                        //   X
                        // X X
                        //   X
                        out.points[0].x = game_state->current_location.x + 1;
                        out.points[0].y = game_state->current_location.y - 2;
                        out.points[1].x = game_state->current_location.x;
                        out.points[1].y = game_state->current_location.y - 1;
                        out.points[2].x = game_state->current_location.x + 1;
                        out.points[2].y = game_state->current_location.y - 1;
                        out.points[3].x = game_state->current_location.x + 1;
                        out.points[3].y = game_state->current_location.y;
                        break;
                default:
                        panic("invalid rotation");
                }
                break;
        default:
                printf("%X", game_state->current_piece);
                panic("Invalid shape");
        }
        return out;
}

void handle_input(GameState *game_state, PlayerInput player_input) {
        PointCloud pc;
        u32 old_rotation;
        u32 not_collided;
        u32 drop_counter = 0;

        switch (player_input) {
        case MoveLeft:
                pc = get_points_for_current_piece(game_state);
                for ( u32 i = 0; i < 4; ++i ) {
                        if (pc.points[i].x == 0) { return; }
                        if (game_state->board[pc.points[i].x - 1][pc.points[i].y] != Empty) { return; }
                }
                game_state->current_location.x -= 1;
                break;
        case MoveRight:
                pc = get_points_for_current_piece(game_state);
                for ( u32 i = 0; i < 4; ++i ) {
                        if (pc.points[i].x + 1 == BOARD_WIDTH) { return; }
                        if (game_state->board[pc.points[i].x + 1][pc.points[i].y] != Empty) { return; }
                }
                game_state->current_location.x += 1;
                break;
        case RotateLeft:
                old_rotation = game_state->current_rotation;
                game_state->current_rotation -= 1;
                game_state->current_rotation %= 4;
                pc = get_points_for_current_piece(game_state);
                for ( u32 i = 0; i < 4; ++i ) {
                        if (pc.points[i].x >= BOARD_WIDTH || pc.points[i].y >= BOARD_HEIGHT) {
                                game_state->current_rotation = old_rotation;
                                return;
                        }
                        if (game_state->board[pc.points[i].x][pc.points[i].y] != Empty) {
                                game_state->current_rotation = old_rotation;
                                return;
                        }
                }
                break;
        case RotateRight:
                old_rotation = game_state->current_rotation;
                game_state->current_rotation += 1;
                game_state->current_rotation %= 4;
                pc = get_points_for_current_piece(game_state);
                for ( u32 i = 0; i < 4; ++i ) {
                        mvprintw(0,0,"%d %d", pc.points[i].x, pc.points[i].y);
                        if (pc.points[i].x >= BOARD_WIDTH || pc.points[i].y >= BOARD_HEIGHT) {
                                game_state->current_rotation = old_rotation;
                                return;
                        }
                        if (game_state->board[pc.points[i].x][pc.points[i].y] != Empty) {
                                game_state->current_rotation = old_rotation;
                                return;
                        }
                }
                break;
        case MoveDown:
                // TODO: fast fall
                break;
        case DropPiece:
                printf("DROPPING PIECE");
                not_collided = TRUE;
                while (not_collided) {
                        drop_counter += 1;
                        game_state->current_location.y += 1;
                        pc = get_points_for_current_piece(game_state);
                        for (u32 i = 0; i < 4; ++i) {
                                not_collided = not_collided && (pc.points[i].y + 1 != BOARD_HEIGHT);
                                not_collided = not_collided && (game_state->board[pc.points[i].x][pc.points[i].y] == Empty);
                        }
                }
                game_state->current_location.y -= 1;
                game_state->score += drop_counter / 8;
                break;
        default:
                break;
        }
}

void step_game(GameState *game_state) {
        u32 will_collide = FALSE;
        u32 multiplier = 0;

        for ( u32 j = 4; j < BOARD_HEIGHT; ++j ) {
                u32 all_filled = TRUE;
                for ( u32 i = 0; i < BOARD_WIDTH; ++i ) {
                        all_filled = all_filled && (game_state->board[i][j] != Empty);
                }
                if (all_filled) {
                        multiplier += 1;
                        for ( u32 shift_times = j; shift_times > 4; --shift_times ) {
                                for ( u32 i = 0; i < BOARD_WIDTH; ++i ) {
                                        game_state->board[i][shift_times] = game_state->board[i][shift_times - 1];
                                }
                        }
                }
        }

        if (multiplier) {
                game_state->score += 10 * (multiplier * multiplier);
                // update lines cleared here
                return;
        }

        PointCloud pc = get_points_for_current_piece(game_state);

        for ( u32 i = 0; i < 4; ++i ) {
                if (pc.points[i].y + 1 >= BOARD_HEIGHT || game_state->board[pc.points[i].x][pc.points[i].y + 1] != Empty) {
                        will_collide = TRUE;
                }
        }

        if (will_collide) {
                for ( u32 i = 0; i < 4; ++i ) {
                        game_state->board[pc.points[i].x][pc.points[i].y] = game_state->current_color;
                }
                game_state->current_piece = game_state->next_piece;
                game_state->current_color = game_state->next_color;
                game_state->current_location.x = BOARD_WIDTH / 2;
                game_state->current_location.y = 3;
                generate_new_next_piece(game_state);
        }
        
        game_state->current_location.y += 1;
}

void draw_game(GameState *game_state) {
        clear();
        const u32 SCREEN_OFFSET = 15;
        const u32 VERT_OFFSET = 3;
        u32 color = Empty;
        Shape piece_swap;
        u32 rotation_swap;
        Point location_swap;

        // print score
        mvprintw(VERT_OFFSET - 1, 0, "Score: %d", game_state->score);

        for (u32 i = 0; i < BOARD_WIDTH; ++i) {
                for (u32 j = 4; j < BOARD_HEIGHT; ++j) {
                        switch (game_state->board[i][j]) {
                        case Empty:
                                attron(COLOR_PAIR(EMPTY_PAIR));
                                mvaddch(j + VERT_OFFSET - (BOARD_HEIGHT - PLAYABLE_BOARD_HEIGHT),
                                        (i * 2) + SCREEN_OFFSET, ' ');
                                attroff(COLOR_PAIR(EMPTY_PAIR));
                                break;
                        case Blue: case Green: case Yellow: case Red:
                                color = color_to_color_pair(game_state->board[i][j]);
                                attron(COLOR_PAIR(color));
                                mvaddch(j + VERT_OFFSET - (BOARD_HEIGHT - PLAYABLE_BOARD_HEIGHT),
                                        (i * 2) + SCREEN_OFFSET, 'X');
                                attroff(COLOR_PAIR(color));
                                break;
                        default:
                                panic("Unknown color");
                        }
                }
        }

        // DRAW FALLING PIECE
        PointCloud pc = get_points_for_current_piece(game_state);
        color = color_to_color_pair(game_state->current_color);
        attron(COLOR_PAIR(color));
        for ( u32 i = 0; i < 4; ++i ) {
                mvaddch(pc.points[i].y + VERT_OFFSET - (BOARD_HEIGHT - PLAYABLE_BOARD_HEIGHT),
                        (pc.points[i].x * 2) + SCREEN_OFFSET, 'O');
        }
        attroff(COLOR_PAIR(color));
        mvhline(PLAYABLE_BOARD_HEIGHT + VERT_OFFSET, SCREEN_OFFSET, 0, BOARD_WIDTH * 2);
        mvhline(VERT_OFFSET - 1, SCREEN_OFFSET, 0, BOARD_WIDTH * 2);
        mvvline(VERT_OFFSET, SCREEN_OFFSET - 1, 0, PLAYABLE_BOARD_HEIGHT);
        mvaddch(VERT_OFFSET - 1, SCREEN_OFFSET - 1, ACS_ULCORNER);
        mvvline(VERT_OFFSET, SCREEN_OFFSET + BOARD_WIDTH * 2, 0, PLAYABLE_BOARD_HEIGHT);
        mvaddch(VERT_OFFSET - 1, SCREEN_OFFSET + BOARD_WIDTH * 2, ACS_URCORNER);
        mvaddch(VERT_OFFSET + PLAYABLE_BOARD_HEIGHT, SCREEN_OFFSET + BOARD_WIDTH * 2, ACS_LRCORNER);
        mvaddch(VERT_OFFSET + PLAYABLE_BOARD_HEIGHT, SCREEN_OFFSET - 1, ACS_LLCORNER);

        // DRAW NEXT PIIECE
        const u32 NEXT_PIECE_DISPLAY_OFFSET = SCREEN_OFFSET + BOARD_WIDTH * 2 + 5;

        // HACK, fix this by parameterizing get_points fn over the shape
        piece_swap = game_state->current_piece;
        rotation_swap = game_state->current_rotation;
        location_swap = game_state->current_location;
        game_state->current_piece = game_state->next_piece;
        game_state->current_rotation = 0;
        game_state->current_location.x = 4;
        game_state->current_location.y = 4;
        pc = get_points_for_current_piece(game_state);
        game_state->current_piece = piece_swap;
        game_state->current_rotation = rotation_swap;
        game_state->current_location = location_swap;
        color = color_to_color_pair(game_state->next_color);
        attron(COLOR_PAIR(color));
        for ( u32 i = 0; i < 4; ++i ) {
                mvaddch(pc.points[i].y + VERT_OFFSET * 2 - (BOARD_HEIGHT - PLAYABLE_BOARD_HEIGHT),
                        (pc.points[i].x * 2) + NEXT_PIECE_DISPLAY_OFFSET, 'O');
        }
        attroff(COLOR_PAIR(color));
        //  bad code: TODO: abstract drawing boxes or use ncurses windows properly
        mvhline(VERT_OFFSET, NEXT_PIECE_DISPLAY_OFFSET + 4, 0, 4 * 2 + 1);
        mvhline(VERT_OFFSET + 5, NEXT_PIECE_DISPLAY_OFFSET + 4, 0, 4 * 2 + 1);
        mvvline(VERT_OFFSET + 1, NEXT_PIECE_DISPLAY_OFFSET + 4 - 1, 0, 4);
        mvaddch(VERT_OFFSET, NEXT_PIECE_DISPLAY_OFFSET + 4 - 1, ACS_ULCORNER);
        mvvline(VERT_OFFSET + 1,  NEXT_PIECE_DISPLAY_OFFSET + 4 + 4 * 2 + 1, 0, 4);
        mvaddch(VERT_OFFSET, NEXT_PIECE_DISPLAY_OFFSET + 4 + 4 * 2 + 1, ACS_URCORNER);
        mvaddch(VERT_OFFSET + 4 + 1, NEXT_PIECE_DISPLAY_OFFSET + 4 + 4 * 2 + 1, ACS_LRCORNER);
        mvaddch(VERT_OFFSET + 4 + 1, NEXT_PIECE_DISPLAY_OFFSET + 4 - 1, ACS_LLCORNER);

        refresh();
}

void init_ncurses() {
        setlocale(LC_ALL, "");
        initscr();
        cbreak();
        noecho();
        nonl();
        nodelay(stdscr, TRUE);
        intrflush(stdscr, FALSE);
        keypad(stdscr, TRUE);
        // hide cursor
        curs_set(0);
        // coloring
        start_color();
        init_pair(EMPTY_PAIR, COLOR_BLACK, COLOR_BLACK);
        init_pair(RED_PAIR, COLOR_RED, COLOR_BLACK);
        init_pair(BLUE_PAIR, COLOR_BLUE, COLOR_BLACK);
        init_pair(GREEN_PAIR, COLOR_GREEN, COLOR_BLACK);
        init_pair(YELLOW_PAIR, COLOR_YELLOW, COLOR_BLACK);
}

int main() {
        GameState game_state;
        init_gamestate(&game_state);
        init_ncurses();
        GameOutcome outcome;
        srandom(time(NULL));
        while ( (outcome = evaluate_gamestate(&game_state)) == Playing) {
                PlayerInput player_input = get_player_input();
                if( player_input & QuitRequested ) { break; }
                else if( player_input & PauseRequested ) { pause(); } 
                else if( !(player_input & NoInput) ) {
                        handle_input(&game_state, player_input); }

                step_game(&game_state);
                draw_game(&game_state);
                // instead of sleeping check input in a busy loop
                usleep(1000 * 250);
        }

        if (outcome == Lose) {
                // print lose message
        }

        return EXIT_SUCCESS;
}
