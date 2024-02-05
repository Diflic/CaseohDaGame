#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <SDL.h>
#include "C:\devkitPro\portlibs\switch\include\SDL2\SDL_image.h"
#include <time.h>
#include <unistd.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <switch.h>
#include <string>


/* This lets the randomization happen*/

    #define randnum(min, max) \
    ((rand() % (int)(((max) + 1) - (min))) + (min))
    
    //printf("%d\n", randnum(1, 2));
    //srand(time(NULL));
/*Will caseoh move left(1) or right(2)?*/


int rand_range(int min, int max){
   return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}
//texture render sets text into a renderer
SDL_Texture * render_text(SDL_Renderer *renderer, const char* text, TTF_Font *font, SDL_Color color, SDL_Rect *rect) 
{
    SDL_Surface *surface;
    SDL_Texture *texture;

    surface = TTF_RenderText_Solid(font, text, color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    rect->w = surface->w;
    rect->h = surface->h;

    SDL_FreeSurface(surface);

    return texture;
}


int main(int argc, char** argv) //builds the window
{
  
    romfsInit();
    chdir("romfs:/");
    SDL_Texture *chunkyTex = NULL, *foodTex = NULL, *numTex = NULL;

    /*SDL_Rect {location upper left x, y: size width, height}*/
    //if you change the first numbers you can change the start pos
    SDL_Rect dentPos = { 480, 86, 0, 0};
    SDL_Rect foodPos = { 960, 900, 0, 0};
    Mix_Music *music = NULL;
    SDL_Event event;
    TTF_Init();
    SDL_Color colors[] = {
        { 128, 128, 128, 0 }, // gray
        { 255, 255, 255, 0 }, // white
        { 255, 0, 0, 0 },     // red
        { 0, 255, 0, 0 },     // green
        { 0, 0, 255, 0 },     // blue
        { 255, 255, 0, 0 },   // brown
        { 0, 255, 255, 0 },   // cyan
        { 255, 0, 255, 0 },   // purple
    };

    int done = 0;
    int imgW = 256; int imgH = 256; int chunkySpeed = 10, SPEED = 10, setFalse = 0;
    int delay=100; // milliseconds
    int startDrection = 0;
    
   
     


    /*   Setup    */

    //define area of diplaying and how it will give the info
    SDL_Window* window = SDL_CreateWindow("CaseohDaGame", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Surface *roadBlock = IMG_Load("data/Caseoh.png");
    if (roadBlock) 
    {
        //set the w and h of dentPos to the size of roadBlock's w and h
        dentPos.w = roadBlock -> w; 
        dentPos.h = roadBlock -> h;
        chunkyTex = SDL_CreateTextureFromSurface(renderer,roadBlock);
        SDL_FreeSurface(roadBlock);
    }

    SDL_Surface *foodThick = IMG_Load("data/shrimp.png");
    if (foodThick) 
    {
        //set the w and h of dentPos to the size of roadBlock's w and h
        foodPos.w = foodThick -> w; 
        foodPos.h = foodThick -> h;
        foodTex = SDL_CreateTextureFromSurface(renderer,foodThick);
        SDL_FreeSurface(foodThick);
    }

    
    SDL_Rect numrect = { 0, 1080 - 36, 0, 0 };
    
    
    // mandatory at least on switch, else gfx is not properly closed
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        SDL_Log("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }
   
    
    


    /* Controller */
    // open CONTROLLER_PLAYER_1 and CONTROLLER_PLAYER_2
    // when railed, both joycons are mapped to joystick #0,
    // else joycons are individually mapped to joystick #0, joystick #1, ...
    // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L45
    for (int i = 0; i < 2; i++) {
        if (SDL_JoystickOpen(i) == NULL) {
            SDL_Log("SDL_JoystickOpen: %s\n", SDL_GetError());
            SDL_Quit();
            return -1;
        }
    }
    /*music set up*/
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    Mix_AllocateChannels(5);
    Mix_OpenAudio(48000, AUDIO_S16, 2, 4096);

    music = Mix_LoadMUS("data/tubaSong.ogg");
    Mix_Chunk *sounds[1] = {NULL};
    

    int widthing = 1920 - dentPos.w; int movement = 20, mspeed = 20, uppies = 20, go = 0;
    int chikening = 1920 - foodPos.w, nomove = 0;
    float rotate = 0.0f; 




    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    /* Gameplay Actions*/
    sounds[0] = Mix_LoadWAV("data/Alfredo.wav");
    if (music) //load ogg music loop
        Mix_PlayMusic(music, -1);
    while (!done){
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_JOYAXISMOTION:
                    SDL_Log("Joystick %d axis %d value: %d\n",
                            event.jaxis.which,
                            event.jaxis.axis, event.jaxis.value);
                if(event.jaxis.which == 0){
                    if(event.jaxis.axis == 0){
                            nomove = 1;
                        } else if(event.jaxis.axis == 1){
                            nomove = 2;
                        }else{
                    nomove = 0;
                }
                }
                        
                    break;
                case SDL_JOYBUTTONDOWN:
                    SDL_Log("Joystick %d button %d down\n",
                            event.jbutton.which, event.jbutton.button);
                    // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L52
                    // seek for joystick #0
                if (event.jbutton.which == 0) {
                        if (event.jbutton.button == 0) {
                            //(A) button down (true)
                            startDrection = rand_range(1,6);
            std::string str = std::to_string(startDrection);

            const char * c = str.c_str();
            TTF_Font* font = TTF_OpenFont("data/ComicSansMS3.ttf", 36);
            numTex = render_text(renderer, c, font, { 255, 255, 255, 0 }, &numrect);
            TTF_CloseFont(font);
                        }
                        else if (event.jbutton.button == 1) {
                            // (B) button down
                            delay = 10;
                            go = 1;
                        }else if (event.jbutton.button == 2) {
                            // (x) button down
                            
                        }else if (event.jbutton.button == 3) {
                            // (y) button down
                           
                        }else if (event.jbutton.button == 10) {
                            // (+) button down
                            done = 1;
                        }else if (event.jbutton.button == 11) {
                            // (-) button down
                            Mix_FadeOutMusic(100);
                        }
                    
                }
                
            }
            break;
        }

        if(setFalse == 0){
            startDrection = rand_range(1,6);
            std::string str = std::to_string(startDrection);

            const char * c = str.c_str();
            TTF_Font* font = TTF_OpenFont("data/ComicSansMS3.ttf", 36);
            numTex = render_text(renderer, c, font, { 255, 255, 255, 0 }, &numrect);
            TTF_CloseFont(font);
            setFalse += 1;
        }
        SDL_RenderClear(renderer); //this reloads the screen so imgs can move without duplicating



        if (numTex){
            
            SDL_RenderCopy(renderer, numTex, NULL, &numrect);
            
        }

        if(chunkyTex){
        
        SDL_RenderCopyEx(renderer, chunkyTex, NULL, &dentPos,rotate,NULL,SDL_FLIP_HORIZONTAL);
        }
        if(foodTex){
            SDL_RenderCopy(renderer, foodTex, NULL,&foodPos);
        }

        if(music){
            //SDL_Delay(delay);
            rotate += 10.0f;
        }



        /*starts Caseoh moving right*/
        if(startDrection >= 3){
            dentPos.x += chunkySpeed;
            if(dentPos.x >= widthing){
                chunkySpeed = -SPEED;
            }
            if(dentPos.x <= 0 ){
                chunkySpeed = SPEED;
            }
        }
        
        /*starts Caseoh moving left*/
        if(startDrection <= 4){
            dentPos.x -= chunkySpeed;
            if(dentPos.x <= 0 ){
                chunkySpeed = -SPEED;
            }
            if(dentPos.x >= widthing){
                chunkySpeed = SPEED;
            }
        }
        /*character movement*/
        if(nomove == 1){
            movement += mspeed;
        } else if(nomove == 2){
            movement += -mspeed;
        } 
        
        if(foodPos.x >= chikening){
            movement = -mspeed;
        }
        if(foodPos.x <= 0){
            movement = mspeed;
        }

        if(go == 1){
            foodPos.y -= uppies;
        }
        
        /*Touchies*/
        if(foodPos.y <= dentPos.y + dentPos.h){ //left bottom corner height need to add because otherwise its too high
            if(foodPos.x >= dentPos.x){ //lefr bottom corner size 2 side
                if(foodPos.x <= dentPos.x + dentPos.w){
                    go = 0;
                    foodPos.y = 900;
                    dentPos.h +=100;
                    dentPos.w +=100;
                    Mix_PlayChannel(-1, sounds[0], 0);

                    
                }
            }
        }
        if(foodPos.y <= 0){
            go = 0;
            foodPos.y = 900;
        }
   
           

        
        /*render screen*/
        SDL_RenderPresent(renderer);
    }
    
    /*when game ends*/
    if (chunkyTex)
    {SDL_DestroyTexture(chunkyTex);}
    
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}