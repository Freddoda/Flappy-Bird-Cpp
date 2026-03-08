#include<SDL3/SDL.h>
#include<SDL3/SDL_main.h>
#include<SDL3_ttf/SDL_ttf.h>
#include<chrono>
#include<thread>
#include<iostream>
#include<vector>
#include<array>
#include<random>
#include<fstream>
#include<string>

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
    int height;
    std::mt19937 generator;
    std::vector<std::array<int,4>> rocks;
    std::uniform_int_distribution<> rockAmount;
    std::uniform_int_distribution<> rockSize;
    std::uniform_int_distribution<> rockX;
    std::uniform_int_distribution<> rockY;
    int amountRocks;
    float offset;
    float increment;


    Cave(float increment){
        height=40;
        std::uniform_int_distribution<> rockAmount(2,8);
        this->rockAmount = rockAmount;
        std::uniform_int_distribution<> rockSize(10,180);
        this->rockSize = rockSize;
        std::uniform_int_distribution<> rockX(-30,650);
        this->rockX = rockX;
        std::uniform_int_distribution<> rockY(-30,950);
        this->rockY = rockY;
        std::random_device rd;
        std::mt19937 generator(rd());
        this->generator = generator;
        amountRocks= this->rockAmount(this->generator);
        for (int i=0; i<amountRocks; i++){
            rocks.push_back({this->rockX(this->generator),this->rockY(this->generator),this->rockSize(this->generator),this->rockSize(this->generator)});
        }
        offset = 0;
        this->increment = increment;
    }

    void move(){
        offset += increment;
        if (offset>640){
            offset-=640;
        }
    }

    void reset(){
        rocks.clear();
        amountRocks = rockAmount(generator);
        for (int i=0; i<amountRocks; i++){
            rocks.push_back({rockX(generator),rockY(generator),rockSize(generator),rockSize(generator)});
        }
        offset = 0;
    }

    void floorceildraw(SDL_Renderer* renderer){
        renderRect(renderer, {0,0}, {640,static_cast<float>(height)}, {30,30,30});
        renderRect(renderer, {0,static_cast<float>(940-height)}, {640,static_cast<float>(height)}, {30,30,30});
    }

    void backdraw(SDL_Renderer* renderer){
        for (std::array<int,4> rock: rocks){
            renderRect(renderer, {rock[0] - offset - 640, static_cast<float>(rock[1])}, {static_cast<float>(rock[2]), static_cast<float>(rock[3])}, {20,20,20});
            renderRect(renderer, {rock[0] - offset, static_cast<float>(rock[1])}, {static_cast<float>(rock[2]), static_cast<float>(rock[3])}, {20,20,20});
            renderRect(renderer, {rock[0] - offset + 640, static_cast<float>(rock[1])}, {static_cast<float>(rock[2]), static_cast<float>(rock[3])}, {20,20,20});
        }
    }
};

struct Pipes{
    float x; //centre of pipes
    int y; //height of directly inbetween pipes
    int gap; //distance from centre of gap to pipe
    int width; //entire width
    float speed;
    int index;
    bool scored;
    std::mt19937 generator;
    std::uniform_int_distribution<> dist;

    Pipes(float speed, int gap, int width, int index){
        x = 520 * index;
        this->gap = gap;
        this->width = width;
        this->index = index;
        this->speed = speed;
        scored = false;
        std::random_device rd;
        std::mt19937 generator(rd());
        this->generator = generator;
        std::uniform_int_distribution<> dist(gap+45,940-gap-45);
        this->dist=dist;
        y = this->dist(this->generator);
    }

    void reset(){
        x = 500 * index;
        y = dist(generator);
        scored = false;
    }

    void move(){
        x-=speed;
        if (x<=-width/2){
            x+=520*2;
            y = dist(generator);
            scored = false;
        }
    }

    void draw(SDL_Renderer *renderer){
        renderRect(renderer,{x-width/2,0},{static_cast<float>(width),static_cast<float>(y-gap)},{0,100,0});
        renderRect(renderer,{x-width/2,static_cast<float>(y+gap)},{static_cast<float>(width),static_cast<float>(940-y-gap)},{0,100,0});
    }
};

struct Bird{
    int x;
    float y;
    int rad;
    float ysp;
    int jtimer;
    Cave* cave;

    Bird(Cave* cave){
        this->x=200;
        this->y=400;
        this->rad=25;
        this->ysp=0;
        this->jtimer=0;
        this->cave = cave;
    }

    void draw(SDL_Renderer* renderer){
        renderCircle(renderer,x,y,rad,{150,150,0});
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
        if (y-rad<cave->height || y+rad>940-cave->height) {
            collide = true;
        } else{
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

struct Score{
    int score;
    int highscore;
    std::string filename;
    Bird* bird;
    std::array<Pipes,2>* pipes_arr;
    TTF_Font* font;
    TTF_TextEngine* engine;

    Score(Bird* bird, std::array<Pipes,2> *pipes_arr, const char filename[]){
        const char* base_path = SDL_GetBasePath(); 
        this->filename = std::string(base_path) + std::string(filename);
        this->bird = bird;
        this->pipes_arr = pipes_arr;
        score=0;
        std::ifstream file;
        file.open(this->filename);
        std::string line;
        while (std::getline(file,line)) {
            highscore = std::stoi(line);
        }
    }

    void setFont(TTF_Font* font){
        this->font = font;
    }

    void setEngine(TTF_TextEngine* engine){
        this->engine = engine;
    }

    void update(){
        for (Pipes &pipe: *pipes_arr){
            if (pipe.x+pipe.width/2<bird->x-bird->rad && !pipe.scored){
                score+=1;
                pipe.scored=true;
            }
        }
        if (score>highscore){
            highscore = score;
        }
        std::ofstream file;
        file.open(filename);
        file << highscore;
    }

    void reset(){
        score = 0;
    }

    void draw(){
        TTF_Text* textobj = TTF_CreateText(engine, font, std::to_string(score).c_str(), 0);
        int* width = new int(0);
        int* height = new int(0);
        TTF_GetTextSize(textobj, width, height);
        TTF_SetTextColor(textobj, 200, 0, 0, 255);
        TTF_DrawRendererText(textobj,320-(*width)/2,5);
        TTF_DestroyText(textobj);
        TTF_Text* textobj2 = TTF_CreateText(engine, font, std::to_string(highscore).c_str(), 0);
        TTF_SetTextColor(textobj2, 200, 0, 0, 255);
        TTF_DrawRendererText(textobj2,320-(*width)/2,10+*height);
        TTF_DestroyText(textobj2);
        delete width;
        delete height;
    }
};

void update(bool* playing, bool* playable, std::vector<SDL_Keycode> &keys, Bird *bird, std::array<Pipes,2> &pipe_arr, Cave* cave, Score* score){
    if (*playing){
        bird->jump(keys);
        cave->move();
        for (Pipes &pipe : pipe_arr){
            pipe.move();
        }
        if (bird->collision(pipe_arr)){
            *playing = false;
            bird->reset();
            cave->reset();
            for (Pipes &pipe : pipe_arr){
                pipe.reset();
            }
            score->reset();
        } else {
            score->update();
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

void draw(SDL_Renderer *renderer, Bird *bird, std::array<Pipes,2> &pipe_arr, Cave* cave, Score* score){
    SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
    SDL_RenderClear(renderer);

    cave->backdraw(renderer);
    bird->draw(renderer);
    for (Pipes pipe : pipe_arr){
            pipe.draw(renderer);
    }
    cave->floorceildraw(renderer);
    score->draw();

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
    TTF_TextEngine* textEngine;
    TTF_Init();
    bool done = false;
    SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL3

    std::chrono::nanoseconds start;
    std::chrono::nanoseconds end;
    std::chrono::nanoseconds duration;
    std::chrono::nanoseconds frametime(16666667);

    std::vector<SDL_Keycode> keys;

    bool* playing = new bool(false);
    bool* playable = new bool(true);

    Cave* cave = new Cave(2.4f);
    Bird* bird = new Bird(cave);
    std::array<Pipes,2> pipes_arr = {Pipes(2.8f,100,100,1),Pipes(2.8f,100,100,2)};
    Score* score = new Score(bird, &pipes_arr, "Score.txt");
    
    const char* base_path = SDL_GetBasePath(); 
    std::string font_path = std::string(base_path) + "Geneva.ttf";
    TTF_Font* font = TTF_OpenFont(font_path.c_str(), 60);
    if (font == NULL){
        std::cout<<SDL_GetError();
    }
    delete base_path;

    window = SDL_CreateWindow(
        "Flappy Bird",                  // title
        640,                               // width
        940,                               // height
        SDL_WINDOW_OPENGL                  // flags
    );

    renderer = SDL_CreateRenderer(window,NULL);
    textEngine = TTF_CreateRendererTextEngine(renderer);

    score->setFont(font);
    score->setEngine(textEngine);

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

        update(playing, playable, keys, bird, pipes_arr, cave, score);
        draw(renderer, bird, pipes_arr, cave, score);

        SDL_UpdateWindowSurface(window);

        end = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
        duration = end - start;
        //std::cout << "d" << duration.count() << std::endl;
        //std::cout << "t" << frametime.count() << std::endl;
        if (frametime>duration){
            std::this_thread::sleep_for(frametime-duration);
        }
    }

    // destroy/delete heap allocated memory
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    TTF_DestroyRendererTextEngine(textEngine);
    TTF_CloseFont(font);

    delete score;
    delete playing;
    delete playable;
    delete bird;
    delete cave;

    // Clean up
    SDL_Quit();
    return 0;
}