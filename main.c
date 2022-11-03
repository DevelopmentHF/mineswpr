#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>

/*============================================================================*/

#define CELLSIZE (40)       // size of one cell in px
#define GRID_HEIGHT (21)    // number of cells y-dir
#define GRID_WIDTH (21)     // number of cells x-dir
#define WINDOW_HEIGHT (CELLSIZE*GRID_HEIGHT + 1)   // size of window y-dir
#define WINDOW_WIDTH (CELLSIZE*GRID_WIDTH + 1)    // size of window x-dir

/*============================================================================*/
typedef struct {
    SDL_Rect pos;
    SDL_bool clicked;
    SDL_bool ismine;
} cell_t;

typedef struct {
    SDL_Color bg;
    SDL_Color ghost;
    SDL_Color line;
    SDL_Color clicked;
} colours_t;

typedef struct {
    colours_t colours;
    cell_t board[GRID_HEIGHT][GRID_WIDTH];
} grid_t;

typedef struct {
    SDL_bool active;
    SDL_bool hover;
} mouse_t;

/*============================================================================*/

int
main(int argc, char* argv[]) {

    /* Initialise SDL */
    if (SDL_Init(SDL_INIT_VIDEO)) {
        printf("Error init: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    /* Create a window */
    SDL_Window* window = SDL_CreateWindow("Minesweeper",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        WINDOW_WIDTH,
                                        WINDOW_HEIGHT,
                                        0);

    /* Check if the window was created successfully */
    if (!window) {
        printf("Error window init: %s\n", SDL_GetError());
        /* Exit out */
        SDL_Quit();
        return EXIT_FAILURE;
    }

    /* Create renderer */
    SDL_Renderer* render = SDL_CreateRenderer(
                                window, -1, SDL_RENDERER_ACCELERATED);

    /* Check renderer was created successfully*/
    if (!render) {
        printf("Error render init: %s\n", SDL_GetError());
        /* Exit out */
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    /* Player cell details */
    cell_t player;
    player.pos.w = CELLSIZE;
    player.pos.h = CELLSIZE;

    /* Cell below the cursors current position */
    cell_t ghost;
    ghost.pos.x = player.pos.x;
    ghost.pos.y = player.pos.y;
    ghost.pos.w = CELLSIZE;
    ghost.pos.h = CELLSIZE;

    /* Initialise grid details */
    grid_t grid;
    // Barely Black - Background Colour
    grid.colours.bg.r = 22;
    grid.colours.bg.g = 22;
    grid.colours.bg.b = 22;
    grid.colours.bg.a = 255;

    // Dark grey    - Line and Ghost colouring
    grid.colours.line.r = 44;
    grid.colours.line.g = 44;
    grid.colours.line.b = 44;
    grid.colours.line.a = 255;
    grid.colours.ghost.r = 44;
    grid.colours.ghost.g = 44;
    grid.colours.ghost.b = 44;
    grid.colours.ghost.a = 255;

    // Green        - Successful click colouring
    grid.colours.clicked.r = 28;
    grid.colours.clicked.g = 200;
    grid.colours.clicked.b = 121;
    grid.colours.clicked.a = 255;

    /* Initialise the board details */
    for (int i=0; i<GRID_HEIGHT; i++) {
        for (int j=0; j<GRID_WIDTH; j++) {
            grid.board[i][j].pos.x = i*CELLSIZE;
            grid.board[i][j].pos.y = j*CELLSIZE;
            grid.board[i][j].pos.w = CELLSIZE;
            grid.board[i][j].pos.h = CELLSIZE;
            grid.board[i][j].clicked = SDL_FALSE;
        }
    }

    /* Initialise mouse details */
    mouse_t mouse;
    mouse.active = SDL_FALSE;
    mouse.hover = SDL_FALSE;

    SDL_bool hasquit = SDL_FALSE;
    /* Animation Loop */
    while (!hasquit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                player.pos.x = (event.motion.x / CELLSIZE) * CELLSIZE;
                player.pos.y = (event.motion.y / CELLSIZE) * CELLSIZE;
                break;
            case SDL_MOUSEMOTION:
                ghost.pos.x = (event.motion.x / CELLSIZE) * CELLSIZE;
                ghost.pos.y = (event.motion.y / CELLSIZE) * CELLSIZE;
                if (!mouse.active)
                    mouse.active = SDL_TRUE;
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER && !mouse.hover)
                    mouse.hover = SDL_TRUE;
                else if (event.window.event == SDL_WINDOWEVENT_LEAVE && mouse.hover)
                    mouse.hover = SDL_FALSE;
                break;
            case SDL_QUIT:
                hasquit = SDL_TRUE;
                break;
            }
        }

        /* Draw the background */
        SDL_SetRenderDrawColor(render, grid.colours.bg.r, grid.colours.bg.g,
                                grid.colours.bg.b, grid.colours.bg.a);
        SDL_RenderClear(render);

        /* Set grid colour details */
        SDL_SetRenderDrawColor(render, grid.colours.line.r, grid.colours.line.g,
                                grid.colours.line.b, grid.colours.line.a);

        /* Draw vertical lines */
        for (int x = 0; x <= GRID_WIDTH * CELLSIZE;
            x += CELLSIZE) {
            SDL_RenderDrawLine(render, x, 0, x, WINDOW_HEIGHT);
        }

        /* Draw horizontal lines */
        for (int y = 0; y <= GRID_HEIGHT * CELLSIZE;
                y += CELLSIZE) {
            SDL_RenderDrawLine(render, 0, y, WINDOW_WIDTH, y);
        }


        for (int i=0; i<GRID_HEIGHT; i++) {
            for (int j=0; j<GRID_WIDTH; j++) {

                if (player.pos.x == grid.board[i][j].pos.x &&
                        player.pos.y == grid.board[i][j].pos.y) {
                    grid.board[i][j].clicked = SDL_TRUE;
                }

                if (grid.board[i][j].clicked == SDL_TRUE) {
                    SDL_SetRenderDrawColor(render, grid.colours.clicked.r,
                                   grid.colours.clicked.g,
                                   grid.colours.clicked.b,
                                   grid.colours.clicked.a);
                    SDL_RenderFillRect(render, &grid.board[i][j].pos);
                }
            }
        }

        /* Draw ghost cursor */
        if (mouse.active && mouse.hover) {
            SDL_SetRenderDrawColor(render, grid.colours.ghost.r,
                                   grid.colours.ghost.g,
                                   grid.colours.ghost.b,
                                   grid.colours.ghost.a);
            SDL_RenderFillRect(render, &ghost.pos);
        }

        // Draw last clicked grid cursor to overwrite ghost
        SDL_SetRenderDrawColor(render, grid.colours.clicked.r,
                               grid.colours.clicked.g, grid.colours.clicked.b,
                               grid.colours.clicked.a);
        SDL_RenderFillRect(render, &player.pos);

        SDL_RenderPresent(render);
    }

    /* Clean up*/
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}