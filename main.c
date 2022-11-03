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
    SDL_Rect pos;        // position details of the cursor
} cursor_t;

typedef struct {
    SDL_Texture* texture;
    SDL_Rect pos;
} mine_t;

typedef struct {
    SDL_Color bg_colour;
    SDL_Color line_colour;
    SDL_Color ghost_colour;
    SDL_Color cursor_colour;
} grid_t;

typedef struct {
    SDL_bool active;
    SDL_bool hover;
} mouse_t;

typedef struct {
    SDL_Rect pos;
    SDL_bool clicked;
} cell_t;

typedef struct {
    cell_t grid[21][21];
} gridcells_t;
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

    /* Place the grid cursor in the middle of the screen */
    cursor_t cursor;
    // cursor.pos.x =  (GRID_WIDTH - 1) / 2 * CELLSIZE;
    // cursor.pos.y =  (GRID_HEIGHT - 1) / 2 * CELLSIZE;
    cursor.pos.w = CELLSIZE;
    cursor.pos.h = CELLSIZE;

    /* Cell below the cursors current position */
    cursor_t ghost_cursor;
    ghost_cursor.pos.x = cursor.pos.x;
    ghost_cursor.pos.y = cursor.pos.y;
    ghost_cursor.pos.w = CELLSIZE;
    ghost_cursor.pos.h = CELLSIZE;

    /* Initialise grid details */
    grid_t grid;
    // Barely Black
    grid.bg_colour.r = 22;
    grid.bg_colour.g = 22;
    grid.bg_colour.b = 22;
    grid.bg_colour.a = 255;

    // Dark grey
    grid.line_colour.r = 44;
    grid.line_colour.g = 44;
    grid.line_colour.b = 44;
    grid.line_colour.a = 255;

    // other
    grid.ghost_colour.r = 28;
    grid.ghost_colour.g = 200;
    grid.ghost_colour.b = 121;
    grid.ghost_colour.a = 255;

    // White
    grid.cursor_colour.r = 255;
    grid.cursor_colour.g = 255;
    grid.cursor_colour.b = 255;
    grid.cursor_colour.a = 255;

    gridcells_t gridcells;
        for (int i=0; i<GRID_HEIGHT; i++) {
            for (int j=0; j<GRID_WIDTH; j++) {
                gridcells.grid[i][j].pos.x = i*CELLSIZE;
                gridcells.grid[i][j].pos.y = j*CELLSIZE;
                gridcells.grid[i][j].pos.w = CELLSIZE;
                gridcells.grid[i][j].pos.h = CELLSIZE;
                gridcells.grid[i][j].clicked = SDL_FALSE;
            }
        }

    /* Init loop details */
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
                cursor.pos.x = (event.motion.x / CELLSIZE) * CELLSIZE;
                cursor.pos.y = (event.motion.y / CELLSIZE) * CELLSIZE;
                break;
            case SDL_MOUSEMOTION:
                ghost_cursor.pos.x = (event.motion.x / CELLSIZE) * CELLSIZE;
                ghost_cursor.pos.y = (event.motion.y / CELLSIZE) * CELLSIZE;
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
        SDL_SetRenderDrawColor(render, grid.bg_colour.r, grid.bg_colour.g,
                                grid.bg_colour.b, grid.bg_colour.a);
        SDL_RenderClear(render);

        /* Set grid colour details */
        SDL_SetRenderDrawColor(render, grid.line_colour.r, grid.line_colour.g,
                                grid.line_colour.b, grid.line_colour.a);

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
                if (cursor.pos.x == gridcells.grid[i][j].pos.x && cursor.pos.y == gridcells.grid[i][j].pos.y) {
                    gridcells.grid[i][j].clicked = SDL_TRUE;
                }

                if (gridcells.grid[i][j].clicked == SDL_TRUE) {
                    SDL_SetRenderDrawColor(render, grid.ghost_colour.r,
                                   grid.ghost_colour.g,
                                   grid.ghost_colour.b,
                                   grid.ghost_colour.a);
                    SDL_RenderFillRect(render, &gridcells.grid[i][j].pos);
                }
            }
        }

        /* Draw ghost cursor */
        if (mouse.active && mouse.hover) {
            SDL_SetRenderDrawColor(render, grid.ghost_colour.r,
                                   grid.ghost_colour.g,
                                   grid.ghost_colour.b,
                                   grid.ghost_colour.a);
            SDL_RenderFillRect(render, &ghost_cursor.pos);
        }

        // Draw last clicked grid cursor.
        SDL_SetRenderDrawColor(render, grid.ghost_colour.r,
                               grid.ghost_colour.g, grid.ghost_colour.b,
                               grid.ghost_colour.a);
        SDL_RenderFillRect(render, &cursor.pos);

        SDL_RenderPresent(render);
    }

    /* Clean up*/
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}