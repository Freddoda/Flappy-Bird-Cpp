#include<SDL3/SDL.h>
#include<SDL3/SDL_main.h>
#include<chrono>
#include<thread>
#include<iostream>
#include<vector>
#include<algorithm>
#include<array>
#include<random>

void renderCircle(SDL_Renderer* renderer, int x, int y, int rad, std::array<int,3> colour){
    SDL_SetRenderDrawColor(renderer, colour[0], colour[1], colour[2], 255);
    for (int x2 = -rad; x2<=rad; x2++){
        for (int y2 = -rad; y2<=rad; y2++){
            if ((x2 * x2 + y2 * y2)<(rad*rad)){
                SDL_RenderPoint(renderer,x+x2,y+y2);
            }
        }
    }
}

template <typename T>
bool listContains(std::vector<T> &list, T item){
    bool contains = false;
    int index = 0;
    while (!contains && index<list.size()){
        if (list[index]==item){
            contains=true;
        } else {
            index++;
        }
    }
    return contains;
}

struct Ground{
    int height = 40;
};

struct Pipes{
    int x;
    int y;
    int gap;
    int speed;
    int index;
    std::mt19937 generator;
    std::uniform_int_distribution<int> dist;

    Pipes(int speed, int gap, int index){
        x = 500 * index;
        this->gap = gap;
        this->index = index;
        this->speed = speed;
        std::random_device rd;
        std::mt19937 generator(rd());
        this->generator = generator;
        std::uniform_int_distribution<int> dist(gap+45,940-gap-45);
        this->dist=dist;
        y = this->dist(this->generator);
    }

    void reset(){
        x = 500 * index;
        y = dist(generator);
    }
};

struct Bird{
    int x;
    float y;
    int rad;
    float ysp;
    int jtimer;

    Bird(){
        this->x=200;
        this->y=400;
        this->rad=25;
        this->ysp=0;
        this->jtimer=0;
    }

    void draw(SDL_Renderer* renderer){
        renderCircle(renderer,x,y,rad,{255,255,0});
    }

    void jump(std::vector<SDL_Keycode> &keys){
        if (jtimer>0){
            jtimer--;
        } else if (listContains(keys,SDLK_W)){
            jtimer=25;
            ysp=-15;
        }

        if (ysp<10){
            ysp+=0.5;
        }
        y+=ysp;
    }

    void reset(){
        y=400;
        ysp=0;
        jtimer=0;
    }
};

void update(bool* playing, std::vector<SDL_Keycode> &keys, Bird *bird){
    if (*playing){
        bird->jump(keys);
    } else {
        if (listContains(keys,SDLK_W)){
            *playing = true;
        }
    }
}

void draw(SDL_Renderer *renderer, Bird *bird){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    bird->draw(renderer);

    SDL_RenderPresent(renderer);
}

int main(){
    SDL_Window* window;                    // Declare a window pointer
    SDL_Renderer* renderer;
    bool done = false;
    SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL3

    std::chrono::nanoseconds start;
    std::chrono::nanoseconds end;
    std::chrono::nanoseconds duration;
    std::chrono::nanoseconds frametime(16666667);

    std::vector<SDL_Keycode> keys;

    bool* playing = new bool(false);

    Bird* bird = new Bird();

    window = SDL_CreateWindow(
        "An SDL3 window",                  // title
        640,                               // width
        940,                               // height
        SDL_WINDOW_OPENGL                  // flags
    );

    renderer = SDL_CreateRenderer(window,NULL);

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
                if (!listContains(keys,event.key.key)){
                    keys.push_back(event.key.key);
                }
            } else if (event.type == SDL_EVENT_KEY_UP){
                if (listContains(keys,event.key.key)){
                    keys.erase(keys.begin()+std::distance(keys.begin(),std::find(keys.begin(),keys.end(),event.key.key)));
                }
            }
        }
        if(listContains(keys,SDLK_ESCAPE)){
            done = true;
        }

        update(playing, keys, bird);
        draw(renderer, bird);

        SDL_UpdateWindowSurface(window);

        end = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
        duration = end - start;
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