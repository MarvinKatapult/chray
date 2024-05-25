
#include <raylib.h>

#include <ctype.h>

#include "tstring.hpp"
#include "tlog.hpp"

#define SQUARE_SIZE     100
#define BOARD_WIDTH     8
#define BOARD_HEIGHT    8
#define BOARD_SIZE      64

#define SIDEBAR_WIDTH   SQUARE_SIZE * 2 

#define SCREEN_WIDTH    ( BOARD_WIDTH * SQUARE_SIZE + SIDEBAR_WIDTH )
#define SCREEN_HEIGHT   BOARD_HEIGHT * SQUARE_SIZE

#define PIECE_COUNT    6

Texture piece_pngs[2][PIECE_COUNT];
Vector2 myBoardLayout[BOARD_HEIGHT][BOARD_WIDTH];

Vector2 mySelectedPiece = { -1, -1 };

void loadTextures() {

    Image images[2][PIECE_COUNT] = {
        LoadImage( "assets/white-pawn.png" ),
        LoadImage( "assets/white-bishop.png" ),
        LoadImage( "assets/white-knight.png" ),
        LoadImage( "assets/white-rook.png" ),
        LoadImage( "assets/white-king.png" ),
        LoadImage( "assets/white-queen.png" ),

        LoadImage( "assets/black-pawn.png" ),
        LoadImage( "assets/black-bishop.png" ),
        LoadImage( "assets/black-knight.png" ),
        LoadImage( "assets/black-rook.png" ),
        LoadImage( "assets/black-king.png" ),
        LoadImage( "assets/black-queen.png" ),
    };

    for ( int i = 0; i < 2; i++ ) {
        for ( int j = 0; j < PIECE_COUNT; j++ ) {
            ImageResize( &images[i][j], SQUARE_SIZE, SQUARE_SIZE );
            piece_pngs[i][j] = LoadTextureFromImage( images[i][j] );
        }
    }
}

void drawBoard() {
    for ( int y = 0; y < BOARD_HEIGHT; y++ ) {
        for ( int x = 0; x < BOARD_WIDTH; x++ ) {
            DrawRectangle( x * SQUARE_SIZE, y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, ( x + y ) % 2 == 0 ? WHITE : DARKBLUE );
            if ( myBoardLayout[y][x].x != -1 && myBoardLayout[y][x].y != -1 ) {
                DrawTexture( piece_pngs[(int)myBoardLayout[y][x].x][(int)myBoardLayout[y][x].y], x * SQUARE_SIZE, y * SQUARE_SIZE, WHITE );
            }
        }
    }
}

void drawSideBar() {
    for ( int i = 0; i < PIECE_COUNT; i++ ) {
        DrawTexture( piece_pngs[0][i], SCREEN_WIDTH - SQUARE_SIZE * 2, SQUARE_SIZE * i, WHITE );
        DrawTexture( piece_pngs[1][i], SCREEN_WIDTH - SQUARE_SIZE, SQUARE_SIZE * i, WHITE );
    } 

    DrawRectangleLines( SCREEN_WIDTH - SIDEBAR_WIDTH + mySelectedPiece.x * SQUARE_SIZE, mySelectedPiece.y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, RED );

    DrawText( "Press f to",        SCREEN_WIDTH - SIDEBAR_WIDTH + 12, SCREEN_HEIGHT - SQUARE_SIZE, 20, BLACK );
    DrawText( "copy to clipboard", SCREEN_WIDTH - SIDEBAR_WIDTH + 12, SCREEN_HEIGHT - SQUARE_SIZE + 24, 20, BLACK );
}

bool checkForSidebarPress() {
    int picked_color = -1;
    int picked_piece = -1;

    if ( GetMouseX() > SCREEN_WIDTH - SIDEBAR_WIDTH ) {
        picked_color = GetMouseX() < SCREEN_WIDTH - SIDEBAR_WIDTH / 2 ? 0 : 1;
        for ( int i = 0; i < PIECE_COUNT; i++ ) {
            if ( GetMouseY() < ( i + 1 ) * SQUARE_SIZE) {
                picked_piece = i;
                break;
            }
        }
    }

    if ( picked_color != -1 && picked_piece != -1 ) {
        mySelectedPiece.x = (float)picked_color; 
        mySelectedPiece.y = (float)picked_piece;
        return true;
    }

    return false;
}

bool checkForBoardPress( int p_button_press ) {
    // Calculating clicked Square
    Vector2 clicked_square = { -1.f, -1.f };
    for ( int i = 0; i < BOARD_WIDTH; i++ ) {
        if ( GetMouseX() <= (i + 1) * SQUARE_SIZE && GetMouseX() >= i * SQUARE_SIZE ) {
            clicked_square.x = i;
            break;
        }
    }
    for ( int i = 0; i < BOARD_HEIGHT; i++ ) {
        if ( GetMouseY() <= (i + 1) * SQUARE_SIZE && GetMouseY() >= i * SQUARE_SIZE ) {
            clicked_square.y = i;
            break;
        }
    }
    if ( clicked_square.x == -1.f || clicked_square.y == -1.f ) return false;

    // Right Click
    if ( p_button_press == MOUSE_BUTTON_RIGHT ) {
        myBoardLayout[(int)clicked_square.y][(int)clicked_square.x].x = -1;
        myBoardLayout[(int)clicked_square.y][(int)clicked_square.x].y = -1;
        return true;
    }

    // Left Click
    else if ( p_button_press == MOUSE_BUTTON_LEFT ) {
        if ( mySelectedPiece.x == -1 || mySelectedPiece.y == -1 ) return false;

        myBoardLayout[(int)clicked_square.y][(int)clicked_square.x] = mySelectedPiece; 
        return true;
    }

    return false;
}

char getCharForIndex( int p_color, int p_index ) {
    char ret;
    switch ( p_index ) {
        case 0:
            ret = 'p';
            break;
        case 1:
            ret = 'b';
            break;
        case 2:
            ret = 'n';
            break;
        case 3:
            ret = 'r';
            break;
        case 4:
            ret = 'k';
            break;
        case 5:
            ret = 'q';
            break;
        default:
            ret = '\0';
    }

    return p_color == 0 ? toupper( ret ) : ret;
}

TString generateFenstring() {
    TString layout;
    for ( int y = 0; y < BOARD_HEIGHT; y++ ) {
        int empty_fields = 0;
        for ( int x = 0; x < BOARD_WIDTH; x++ ) {
            const int color = myBoardLayout[y][x].x;
            const int piece_index = myBoardLayout[y][x].y;
            if ( piece_index == -1 || color == -1 ) {
                empty_fields++;
            } else {
                if ( empty_fields > 0 ) {
                    layout += empty_fields;
                    empty_fields = 0;
                }

                layout += getCharForIndex( color, piece_index );
            }

            if ( x == BOARD_WIDTH - 1 && empty_fields != 0 ) layout += empty_fields;
        }
        if ( y != BOARD_HEIGHT - 1 ) layout += "/";
    }

    TLog::log( "%s\n", layout.ascii() );
    return layout;
}

void saveFenstring() {
    SetClipboardText( generateFenstring().ascii() ); 
}

void handleUserInput() {
    
    if ( IsMouseButtonDown( MOUSE_BUTTON_LEFT ) ) {
        if ( checkForSidebarPress() ) return;
        if ( checkForBoardPress( MOUSE_BUTTON_LEFT ) ) return;
    }

    if ( IsMouseButtonDown( MOUSE_BUTTON_RIGHT ) ) {
        if ( checkForBoardPress( MOUSE_BUTTON_RIGHT ) ) return;
    }

    if ( IsKeyPressed( KEY_F ) ) {
        saveFenstring();
    }
}

int main() {

    for ( int y = 0; y < BOARD_HEIGHT; y++ ) {
        for ( int x = 0; x < BOARD_WIDTH; x++ ) {
            myBoardLayout[y][x].x = -1;
            myBoardLayout[y][x].y = -1;
        } 
    }

    InitWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "Chray - DEV" );  
    SetTargetFPS( 60 );

    loadTextures();

    while ( !WindowShouldClose() ) {

        handleUserInput();

        BeginDrawing();
            ClearBackground( WHITE );
            drawBoard();
            drawSideBar();
        EndDrawing();
    }

    return 0;
}
