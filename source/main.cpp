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


/* This lets the randomization happen*/

    #define randnum(min, max) \
        ((rand() % (int)(((max) + 1) - (min))) + (min))
    
    //printf("%d\n", randnum(1, 2));
    //srand(time(NULL));
/*Will caseoh move left(1) or right(2)?*/
int startDrection = randnum(1,2);



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
    SDL_Texture *chunkyTex = NULL, *text_tex = NULL;

    /*SDL_Rect {location upper left x, y: size width, height}*/
    //if you change the first numbers you can change the start pos
    SDL_Rect dentPos = { 480, 86, 0, 0};
    Mix_Music *music = NULL;
    SDL_Event event;
    // Mix_Chunk *sound[1] = { NULL };
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
    int imgW = 256; int imgH = 256; int chunkySpeed = 10;

   





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

    // mandatory at least on switch, else gfx is not properly closed
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        SDL_Log("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    /* Text */
     // load font from romfs
    TTF_Font* font = TTF_OpenFont("data/LeroyLetteringLightBeta01.ttf", 36);

    // render text as texture
    SDL_Rect text_rect = { 0, 1920 - 36, 0, 0 };
    text_tex = render_text(renderer, "whats up gamers", font, colors[1], &text_rect);




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
    

    int widthing = 1920 - dentPos.x;
    /* Gameplay Actions*/

    if (music) //load ogg music loop
        Mix_PlayMusic(music, -1);
    while (!done){
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_JOYAXISMOTION:
                    SDL_Log("Joystick %d axis %d value: %d\n",
                            event.jaxis.which,
                            event.jaxis.axis, event.jaxis.value);
                    break;
                case SDL_JOYBUTTONDOWN:
                    SDL_Log("Joystick %d button %d down\n",
                            event.jbutton.which, event.jbutton.button);
                    // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L52
                    // seek for joystick #0
                if (event.jbutton.which == 0) {
                        if (event.jbutton.button == 0) {
                            //(A) button down (true)
                            dentPos.x += 100;
                        }
                        else if (event.jbutton.button == 1) {
                            // (B) button down
                            dentPos.x -= 100;
                        }else if (event.jbutton.button == 2) {
                            // (x) button down
                            dentPos.w -= 100;
                        }else if (event.jbutton.button == 3) {
                            // (y) button down
                            dentPos.w += 100;
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
        SDL_RenderClear(renderer); //this reloads the screen so imgs can move without duplicating
        if(chunkyTex){
        SDL_RenderCopy(renderer, chunkyTex, NULL, &dentPos);
        }
        
        /*starts Caseoh moving left*/
        if(startDrection == 1){
            // dentPos.x += chunkySpeed;
            if(dentPos.x >= widthing){
                chunkySpeed = -10;
            }
        }
        if(startDrection == 2){
            dentPos.x -= chunkySpeed;
            if(dentPos.x <= 0 ){
                chunkySpeed = -10;
            }
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