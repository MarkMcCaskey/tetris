#include <stdint.h>

typedef uint32_t u32;
typedef signed char i8;

const u32 BOARD_WIDTH = 10;
const u32 PLAYABLE_BOARD_HEIGHT = 20;
const u32 BOARD_HEIGHT = PLAYABLE_BOARD_HEIGHT + 4;

// the colors of the blocks, stored in the board
enum Color {
        Empty = 0,
        Red = 0x111,
        Blue = 0x333,
        Green = 0x777,
        Yellow = 0xEEE,
};
typedef enum Color Color;

// Color pair ids
#define EMPTY_PAIR 1
#define RED_PAIR 2
#define BLUE_PAIR 3
#define GREEN_PAIR 4
#define YELLOW_PAIR 5

enum Shape {
        // X X
        // X X
        SquareShape = 0x123,
        // X
        // X X X
        LShape = 0x456,
        // X X
        //   X X
        ZShape = 0x789,
        // X X X X
        IShape = 0xABC,
        //   X X
        // X X
        ReverseZShape = 0xDEF,
        //   X
        // X X X
        TShape = 0xF0F,
        
};
typedef enum Shape Shape;

struct Point {
        u32 x;
        u32 y;
};
typedef struct Point Point;

struct PointCloud {
        Point points[4];
};
typedef struct PointCloud PointCloud;

// tetris is 10x40?
struct GameState {
        u32 score;
        Shape next_piece;
        Color next_color;
        Shape current_piece;
        Color current_color;
        i8 current_rotation;
        // where y is the lowest y point
        // and x is the centermost position on the lowest line with a bias to the left
        Point current_location;
        Color board[BOARD_WIDTH][BOARD_HEIGHT];
};
typedef struct GameState GameState;

enum GameOutcome {
        Playing = 0x010,
        Win = 0x020,
        Lose = 0x040,
};
typedef enum GameOutcome GameOutcome;

enum PlayerInput {
        NoInput = 0x0,
        MoveLeft = 0x1,
        MoveRight = 0x2,
        MoveDown = 0x4,
        PauseRequested = 0x8,
        QuitRequested = 0x10,
        RotateRight = 0x20,
        RotateLeft = 0x40,
        DropPiece = 0x80,
};
typedef enum PlayerInput PlayerInput;

