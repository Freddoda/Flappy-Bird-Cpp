#include<SDL3/SDL.h>
#include<SDL3/SDL_main.h>
#include<chrono>
#include<thread>
#include<iostream>
#include<vector>
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

void renderRect(SDL_Renderer* renderer, std::array<float,2> pos, std::array<float,2> size, std::array<int,3> colour){
    SDL_SetRenderDrawColor(renderer, colour[0], colour[1], colour[2], 255);
    SDL_FRect *rect = new SDL_FRect{pos[0],pos[1],size[0],size[1]};
    SDL_RenderFillRect(renderer, rect);
    delete rect;
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

struct Cave{
    int height = 40;
};

struct Pipes{
    float x; //centre of pipes
    int y; //height of directly inbetween pipes
    int gap; //distance from centre of gap to pipe
    int width; //entire width
    float speed;
    int index;
    std::mt19937 generator;
    std::uniform_int_distribution<int> dist;

    Pipes(float speed, int gap, int width, int index){
        x = 520 * index;
        this->gap = gap;
        this->width = width;
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

    void move(){
        x-=speed;
        if (x<=-width/2){
            x+=520*2;
        }
    }

    void draw(SDL_Renderer *renderer){
        renderRect(renderer,{x-width/2,0},{(float)width,(float)(y-gap)},{0,155,0});
        renderRect(renderer,{x-width/2,(float)(y+gap)},{(float)width,(float)(940-y-gap)},{0,155,0});
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
            jtimer=20;
            ysp=-12;
        }

        if (ysp<12){
            ysp+=0.6;
        }
        y+=ysp;
    }

    void reset(){
        y=400;
        ysp=0;
        jtimer=0;
    }

    bool collision(std::array<Pipes,2> &pipes){
        bool collide = false;
            for (Pipes &pipe : pipes){
                if (pipe.x-pipe.width/2<x && pipe.x+pipe.width/2>x &&
                    !(pipe.y+pipe.gap>y+rad && pipe.y-pipe.gap<y-rad)){
                    collide = true;
                } else if (pipe.x-pipe.width/2<x+rad && pipe.x+pipe.width/2>x-rad &&
                    !(y>pipe.y-pipe.gap && y<pipe.y+pipe.gap)){
                    collide=true;
                } else {
                    if (((pipe.x-pipe.width/2-x)*(pipe.x-pipe.width/2-x))+((pipe.y-pipe.gap-y)*(pipe.y-pipe.gap-y))<rad*rad){
                        collide=true;
                    } else if (((pipe.x-pipe.width/2-x)*(pipe.x-pipe.width/2-x))+((pipe.y+pipe.gap-y)*(pipe.y+pipe.gap-y))<rad*rad){
                        collide=true;
                    } else if (((pipe.x+pipe.width/2-x)*(pipe.x+pipe.width/2-x))+((pipe.y-pipe.gap-y)*(pipe.y-pipe.gap-y))<rad*rad){
                        collide=true;
                    } else if (((pipe.x+pipe.width/2-x)*(pipe.x+pipe.width/2-x))+((pipe.y+pipe.gap-y)*(pipe.y+pipe.gap-y))<rad*rad){
                        collide=true;
                    }
                }
            }
        return collide;
    }

    bool collision(std::array<Pipes,2> &pipes, int x, int y){
        bool collide = false;
            for (Pipes &pipe : pipes){
                if (pipe.x-pipe.width/2<x && pipe.x+pipe.width/2>x &&
                    !(pipe.y+pipe.gap>y+rad && pipe.y-pipe.gap<y-rad)){
                    collide = true;
                } else if (pipe.x-pipe.width/2<x+rad && pipe.x+pipe.width/2>x-rad &&
                    !(y>pipe.y-pipe.gap && y<pipe.y+pipe.gap)){
                    collide=true;
                } else {
                    if (((pipe.x-pipe.width/2-x)*(pipe.x-pipe.width/2-x))+((pipe.y-pipe.gap-y)*(pipe.y-pipe.gap-y))<rad*rad){
                        collide=true;
                    } else if (((pipe.x-pipe.width/2-x)*(pipe.x-pipe.width/2-x))+((pipe.y+pipe.gap-y)*(pipe.y+pipe.gap-y))<rad*rad){
                        collide=true;
                    } else if (((pipe.x+pipe.width/2-x)*(pipe.x+pipe.width/2-x))+((pipe.y-pipe.gap-y)*(pipe.y-pipe.gap-y))<rad*rad){
                        collide=true;
                    } else if (((pipe.x+pipe.width/2-x)*(pipe.x+pipe.width/2-x))+((pipe.y+pipe.gap-y)*(pipe.y+pipe.gap-y))<rad*rad){
                        collide=true;
                    }
                }
            }
        return collide;
    }
};

void update(bool* playing, bool* playable, std::vector<SDL_Keycode> &keys, Bird *bird, std::array<Pipes,2> &pipe_arr){
    if (*playing){
        bird->jump(keys);
        for (Pipes &pipe : pipe_arr){
            pipe.move();
        }
        if (bird->collision(pipe_arr)){
            *playing = false;
            bird->reset();
            for (Pipes &pipe : pipe_arr){
                pipe.reset();
            }
        }

    } else {
        if (listContains(keys,SDLK_W)){
            if (*playable){
                *playing = true;
            }
        } else  {
            *playable = true;
        }
    }
}

void draw(SDL_Renderer *renderer, Bird *bird, std::array<Pipes,2> &pipe_arr){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    bird->draw(renderer);
    for (Pipes pipe : pipe_arr){
            pipe.draw(renderer);
        }

    /*
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int x=0; x<=640; x++){
        for (int y=0; y<=940; y++){
            if (bird->collision(pipe_arr,x,y)){
                SDL_RenderPoint(renderer,x,y);
            }
        }
    }
    */

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
    bool* playable = new bool(true);

    Bird* bird = new Bird();
    std::array<Pipes,2> pipes_arr = {Pipes(2.8f,100,100,1),Pipes(2.8f,100,100,2)};

    window = SDL_CreateWindow(
        "Flappy Bird",                     // title
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

        update(playing, playable, keys, bird, pipes_arr);
        draw(renderer, bird, pipes_arr);

        SDL_UpdateWindowSurface(window);

        end = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
        duration = end - start;
        std::cout << "d" << duration.count() << std::endl;
        std::cout << "t" << frametime.count() << std::endl;
        if (frametime>duration){
            std::this_thread::sleep_for(frametime-duration);
        }
    }

    // destroy/delete heap allocated memory
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    delete playing;
    delete playable;
    delete bird;

    // Clean up
    SDL_Quit();
    return 0;
}
