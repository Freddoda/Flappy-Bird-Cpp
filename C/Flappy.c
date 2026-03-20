#include<SDL3/SDL.h>
#include<SDL3/SDL_main.h>
#include<SDL3_ttf/SDL_ttf.h>
#include<stdlib.h>
#include<time.h>

struct timespec timespec_subtract(struct timespec* a, struct timespec* b){
    struct timespec result = {0};
    result.tv_sec = a->tv_sec-b->tv_sec;
    result.tv_nsec = a->tv_nsec-b->tv_nsec;
    if (result.tv_nsec<0){
        result.tv_sec--;
        result.tv_nsec+=1000000000;
    }
    return result;
}

void update(const bool* keys){
    if (keys[SDL_SCANCODE_W]){
        printf("w");
    }
}

void draw(SDL_Renderer* renderer){

}

int main(int argc, char* args[]){
    SDL_Window* window;                    // Declare a window pointer
    SDL_Renderer* renderer;
    TTF_TextEngine* textEngine;
    TTF_Init();
    bool done = false;
    SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL3
    const bool *keys = SDL_GetKeyboardState(NULL);

    //framerate stuff
    struct timespec inter = { .tv_sec = 0, .tv_nsec = 016666667};
    struct timespec start;
    struct timespec end;
    struct timespec span;
    struct timespec wait;
    //probably didn't have to have this many seperate variables

    window = SDL_CreateWindow(
        "Flappy Bird",                  // title
        640,                               // width
        940,                               // height
        SDL_WINDOW_OPENGL                  // flags
    );

    renderer = SDL_CreateRenderer(window,NULL);
    textEngine = TTF_CreateRendererTextEngine(renderer);

    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    while (!done) {
        timespec_get(&start,TIME_UTC);

        SDL_Event event;
         while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
         }

        update(keys);
        draw(renderer);
        SDL_UpdateWindowSurface(window);

        timespec_get(&end,TIME_UTC);

        span = timespec_subtract(&end,&start);
        //printf("%d\n",(int)span.tv_nsec);
        if (!(span.tv_sec>inter.tv_sec || span.tv_nsec>inter.tv_nsec)){
            wait = timespec_subtract(&inter,&span);
            nanosleep(&wait,NULL);
        }
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    TTF_DestroyRendererTextEngine(textEngine);
    
    SDL_Quit();
    return 0;
}