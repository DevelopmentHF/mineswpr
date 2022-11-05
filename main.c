#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <assert.h>


/*============================================================================*/

#define CELLSIZE (40)       // size of one cell in px
#define GRID_HEIGHT (21)    // number of cells y-dir
#define GRID_WIDTH (21)     // number of cells x-dir
#define WINDOW_HEIGHT (CELLSIZE*GRID_HEIGHT + 1)   // size of window y-dir
#define WINDOW_WIDTH (CELLSIZE*GRID_WIDTH + 1)    // size of window x-dir
#define NUM_MINES (35)      // number of mines on the board

/*============================================================================*/
typedef struct {
    SDL_Surface* mine;
    SDL_Surface* hitmine;
    SDL_Surface* def;
    SDL_Surface* flag;
    SDL_Surface *zero, *one, *two, *three, *four, *five, *six, *seven, *eight;
} surfaces_t;

typedef struct {
    SDL_Texture* mine;
    SDL_Texture* hitmine;
    SDL_Texture* def;
    SDL_Texture* flag;
    SDL_Texture *zero, *one, *two, *three, *four, *five, *six, *seven, *eight;
} textures_t;

typedef struct {
    SDL_bool clicked;
    SDL_bool isflag;
    SDL_bool ismine;
    int numtouching;
} state_t;

typedef struct {
    SDL_Rect pos;
    state_t state;
} cell_t;

typedef struct {
    SDL_bool active;
    SDL_bool hover;
    SDL_bool rightclick;
} mouse_t;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* render;
    surfaces_t surfaces;
    textures_t textures;
    cell_t board[GRID_HEIGHT][GRID_WIDTH];
    cell_t current;
    SDL_bool hasquit;
} game_t;

/*============================================================================*/
void load_surfaces(game_t* game);
void generate_mines(game_t* game);
void init_cell_details(game_t* game);
void generate_touching_details(game_t* game);
void init_tx(game_t* game);
void draw_cells(game_t* game, mouse_t* mouse);
/*================================================*/

int
main(int argc, char** argv) {

    srand(time(NULL)); // For random mine generation
    game_t game;
    mouse_t mouse;
    /* Initialise SDL  */
    if (SDL_Init(SDL_INIT_VIDEO)) {
        printf("Error init: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    /* Create a window */
    game.window = SDL_CreateWindow("Minesweeper",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        WINDOW_WIDTH,
                                        WINDOW_HEIGHT,
                                        0);

    /* Check if the window was created successfully */
    if (!game.window) {
        printf("Error window init: %s\n", SDL_GetError());
        /* Exit out */
        SDL_Quit();
        return EXIT_FAILURE;
    }

    /* Create renderer */
    game.render = SDL_CreateRenderer(
                                game.window, -1, SDL_RENDERER_ACCELERATED);

    /* Check renderer was created successfully*/
    if (!game.render) {
        printf("Error render init: %s\n", SDL_GetError());
        /* Exit out */
        SDL_DestroyWindow(game.window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    /* Load in surfaces */
    load_surfaces(&game);

    /* Generate mine details */
    generate_mines(&game);

    /* Initialise cell details (position, states etc) */
    init_cell_details(&game);


    /* Load in textures */
    init_tx(&game);

    /* Initialise mouse details */
    mouse.active = SDL_FALSE;
    mouse.hover = SDL_FALSE;

    /* Animation loop */
    game.hasquit = SDL_FALSE;
    while(game.hasquit == SDL_FALSE) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                game.current.pos.x = (event.motion.x / CELLSIZE) * CELLSIZE;
                game.current.pos.y = (event.motion.y / CELLSIZE) * CELLSIZE;
                /* Check for flagging/unflagging a cell */
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    mouse.rightclick = SDL_TRUE;
                } else {
                    mouse.rightclick = SDL_FALSE;
                }
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER && !mouse.hover)
                    mouse.hover = SDL_TRUE;
                else if (event.window.event == SDL_WINDOWEVENT_LEAVE && mouse.hover)
                    mouse.hover = SDL_FALSE;
                break;
            case SDL_QUIT:
                game.hasquit = SDL_TRUE;
                break;
            }
        }
        /* Draw all cells */
        draw_cells(&game, &mouse);

        SDL_RenderPresent(game.render);
        SDL_Delay(1000/24);
    }

    SDL_DestroyRenderer(game.render);
    SDL_DestroyWindow(game.window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

void
load_surfaces(game_t* game) {
    game->surfaces.def = IMG_Load("resources/pngs/base_square_small.png");
    game->surfaces.zero = IMG_Load("resources/pngs/clicked_square.png");
    game->surfaces.mine = IMG_Load("resources/pngs/mine.png");
    game->surfaces.hitmine = IMG_Load("resources/pngs/hitmine.png");
    game->surfaces.flag = IMG_Load("resources/pngs/flag.png");
    game->surfaces.one = IMG_Load("resources/pngs/one.png");
    game->surfaces.two = IMG_Load("resources/pngs/two.png");
    game->surfaces.three = IMG_Load("resources/pngs/three.png");
    game->surfaces.four = IMG_Load("resources/pngs/four.png");
    game->surfaces.five = IMG_Load("resources/pngs/five.png");
    game->surfaces.six = IMG_Load("resources/pngs/six.png");
    game->surfaces.seven = IMG_Load("resources/pngs/seven.png");
    game->surfaces.eight = IMG_Load("resources/pngs/eight.png");

}

void
generate_mines(game_t* game) {
    int x,y;
    for (int i=0; i<NUM_MINES; i++) {
        y = (rand() % GRID_HEIGHT);      // Returns a pseudo-random integer between 0 and GH-1
        x = (rand() % GRID_WIDTH);    
        game->board[y][x].state.ismine = SDL_TRUE;
        printf("Mine at (%d, %d)\n", x,y);
    }
}

void
init_cell_details(game_t* game) {
    /* Loop through all cells */
    for (int i=0; i<GRID_HEIGHT; i++) {
        for (int j=0;j<GRID_WIDTH; j++) {
            game->board[i][j].pos.y = i*CELLSIZE;
            game->board[i][j].pos.x = j*CELLSIZE;
            game->board[i][j].pos.w = CELLSIZE;
            game->board[i][j].pos.h = CELLSIZE;
            game->board[i][j].state.clicked = SDL_FALSE;
            game->board[i][j].state.isflag = SDL_FALSE;
            game->board[i][j].state.numtouching = 0;
            if (game->board[i][j].state.ismine != SDL_TRUE) {
                game->board[i][j].state.ismine = SDL_FALSE;
            }
        }
    }
    generate_touching_details(game);
}

void
generate_touching_details(game_t* game) {
    /* Needs to check all 8 positions around each mine */
    /* Loop through all cells */
    for (int y=0; y<GRID_HEIGHT; y++) {
        for (int x=0;x<GRID_WIDTH; x++) {
            /* Check all cells around this one */
            /* Check cell above */
            if (y>0) {
                if (game->board[x][y-1].state.ismine == SDL_TRUE) {
                    game->board[x][y].state.numtouching++;
                }
            }
            /* Check cell below */
            if (y<GRID_HEIGHT-1) {
                if (game->board[x][y+1].state.ismine == SDL_TRUE) {
                    game->board[x][y].state.numtouching++;
                }
            }

            /* Check cell to the left */
            if (x>0) {
                if (game->board[x-1][y].state.ismine == SDL_TRUE) {
                    game->board[x][y].state.numtouching++;
                }
            }

            /* Check cell to the right */
            if (x<GRID_WIDTH-1) {
                if (game->board[x+1][y].state.ismine == SDL_TRUE) {
                    game->board[x][y].state.numtouching++;
                }
            }

            /* Check left upper diagonal cell */
            if (x>0 && y>0) {
                if (game->board[x-1][y-1].state.ismine == SDL_TRUE) {
                    game->board[x][y].state.numtouching++;
                }
            }

            /* Check right upper diagonal cell */
            if (x<GRID_WIDTH-1 && y>0) {
                if (game->board[x+1][y-1].state.ismine == SDL_TRUE) {
                    game->board[x][y].state.numtouching++;
                }
            }

            /* Check left bottom diagonal cell */
            if (x>0 && y<GRID_HEIGHT-1) {
                if (game->board[x-1][y+1].state.ismine == SDL_TRUE) {
                    game->board[x][y].state.numtouching++;
                }
            }

            /* Check right bottom diagonal cell */
            if (x<GRID_WIDTH-1 && y<GRID_HEIGHT-1) {
                if (game->board[x+1][y+1].state.ismine == SDL_TRUE) {
                    game->board[x][y].state.numtouching++;
                }
            }
        }
    }
}

void
init_tx(game_t* game) {
    game->textures.def = SDL_CreateTextureFromSurface(game->render, game->surfaces.def);
    game->textures.zero = SDL_CreateTextureFromSurface(game->render, game->surfaces.zero);
    game->textures.mine = SDL_CreateTextureFromSurface(game->render, game->surfaces.mine);
    game->textures.hitmine = SDL_CreateTextureFromSurface(game->render, game->surfaces.hitmine);
    game->textures.flag = SDL_CreateTextureFromSurface(game->render, game->surfaces.flag);
    game->textures.one = SDL_CreateTextureFromSurface(game->render, game->surfaces.one);
    game->textures.two = SDL_CreateTextureFromSurface(game->render, game->surfaces.two);
    game->textures.three = SDL_CreateTextureFromSurface(game->render, game->surfaces.three);
    game->textures.four = SDL_CreateTextureFromSurface(game->render, game->surfaces.four);
    game->textures.five = SDL_CreateTextureFromSurface(game->render, game->surfaces.five);
    game->textures.six = SDL_CreateTextureFromSurface(game->render, game->surfaces.six);
    game->textures.seven = SDL_CreateTextureFromSurface(game->render, game->surfaces.seven);
    game->textures.eight = SDL_CreateTextureFromSurface(game->render, game->surfaces.eight);

    SDL_FreeSurface(game->surfaces.def);
    SDL_FreeSurface(game->surfaces.zero);
    SDL_FreeSurface(game->surfaces.mine);
    SDL_FreeSurface(game->surfaces.hitmine);
    SDL_FreeSurface(game->surfaces.flag);
    SDL_FreeSurface(game->surfaces.one);
    SDL_FreeSurface(game->surfaces.two);
    SDL_FreeSurface(game->surfaces.three);
    SDL_FreeSurface(game->surfaces.four);
    SDL_FreeSurface(game->surfaces.five);
    SDL_FreeSurface(game->surfaces.six);
    SDL_FreeSurface(game->surfaces.seven);
    SDL_FreeSurface(game->surfaces.eight);
}

void
draw_cells(game_t* game, mouse_t* mouse) {
    for (int i=0; i<GRID_HEIGHT; i++) {
        for (int j=0; j<GRID_WIDTH; j++) {
            /* Set if the cell has been clicked */
            if (game->current.pos.x == game->board[i][j].pos.x && game->current.pos.y == game->board[i][j].pos.y) {
                game->board[i][j].state.clicked = SDL_TRUE;
                printf("click at (%d, %d)\n", j, i);
                /* Check if this click was a flag/unflag */
                if (mouse->rightclick == SDL_TRUE) {
                    game->board[i][j].state.isflag = !game->board[i][j].state.isflag;
                    mouse->rightclick = SDL_FALSE;
                }
            }

            /* Cell hasnt been clicked */
            if (game->board[i][j].state.clicked == SDL_FALSE) {
                /* Thus render def view */
                SDL_RenderCopy(game->render, game->textures.def, NULL, &game->board[i][j].pos);
            /* Cell has been flagged */
            } else if (game->board[i][j].state.isflag) {
                SDL_RenderCopy(game->render, game->textures.flag, NULL, &game->board[i][j].pos);
            /* Display a number */
            } else if (game->board[i][j].state.ismine == SDL_FALSE) {
                if (game->board[i][j].state.numtouching == 0) {
                    SDL_RenderCopy(game->render, game->textures.zero, NULL, &game->board[i][j].pos);
                } else if (game->board[i][j].state.numtouching == 1) {
                    SDL_RenderCopy(game->render, game->textures.one, NULL, &game->board[i][j].pos);
                } else if (game->board[i][j].state.numtouching == 2) {
                    SDL_RenderCopy(game->render, game->textures.two, NULL, &game->board[i][j].pos);
                } else if (game->board[i][j].state.numtouching == 3) {
                    SDL_RenderCopy(game->render, game->textures.three, NULL, &game->board[i][j].pos);
                } else if (game->board[i][j].state.numtouching == 4) {
                    SDL_RenderCopy(game->render, game->textures.four, NULL, &game->board[i][j].pos);
                } else if (game->board[i][j].state.numtouching == 5) {
                    SDL_RenderCopy(game->render, game->textures.five, NULL, &game->board[i][j].pos);
                } else if (game->board[i][j].state.numtouching == 6) {
                    SDL_RenderCopy(game->render, game->textures.six, NULL, &game->board[i][j].pos);
                } else if (game->board[i][j].state.numtouching == 7) {
                    SDL_RenderCopy(game->render, game->textures.seven, NULL, &game->board[i][j].pos);
                } else if (game->board[i][j].state.numtouching == 8) {
                    SDL_RenderCopy(game->render, game->textures.eight, NULL, &game->board[i][j].pos);
                }
            /* Display the mine hit */
            } else {
                SDL_RenderCopy(game->render, game->textures.hitmine, NULL, &game->board[i][j].pos);
            }
        }
    }
}