#include<SDL3/SDL.h>
#include<SDL3/SDL_main.h>
#include<chrono>
#include<thread>
#include<iostream>
#include<vector>
#include<algorithm>

void update(){}
void draw(){}

int main(){
    SDL_Window *window;                    // Declare a window pointer
    bool done = false;
    SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL3

    std::chrono::nanoseconds start;
    std::chrono::nanoseconds end;
    std::chrono::nanoseconds duration;
    std::chrono::nanoseconds frametime(16666667);

    std::vector<SDL_Keycode> keys;

    window = SDL_CreateWindow(
        "An SDL3 window",                  // title
        640,                               // width
        480,                               // height
        SDL_WINDOW_OPENGL                  // flags
    );

    // Check that the window was successfully created
    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    while (!done) {
        start = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());

        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            } else if (event.type == SDL_EVENT_KEY_DOWN){
                if (std::find(keys.begin(),keys.end(),event.key.key) == keys.end()){
                    keys.push_back(event.key.key);
                }
            } else if (event.type == SDL_EVENT_KEY_UP){
                if (std::find(keys.begin(),keys.end(),event.key.key) != keys.end()){
                    keys.erase(keys.begin()+std::distance(keys.begin(),std::find(keys.begin(),keys.end(),event.key.key)));
                }
            }
        }
        if(std::find(keys.begin(),keys.end(),SDLK_ESCAPE) != keys.end()){
            done = true;
        }

        
        // Do game logic, present a frame, etc.

        end = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
        duration = start - end;
        if (frametime>duration){
            std::this_thread::sleep_for(frametime-duration);
        }
    }

    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();
    return 0;
}