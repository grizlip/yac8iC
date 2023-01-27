#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL.h>
#include "chip_vm.h"
#define PIXEL_SIZE 16
#define PI 3.14159265
#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif
static SDL_AudioSpec audio_want;
static SDL_AudioSpec audio_have;

void audio_callback(void* userdata, Uint8* stream, int len);
uint8_t translate_sdl_key(SDL_Keycode sdl_key);
Uint16 get_sample_data(int sample);


int main(int argc, char** argv)
{
    if(argc <2) {
        SDL_Log("Rom path as an arugment needed");
        return -1;
    }
    FILE *fp = fopen(argv[1], "rb"); 
    uint8_t* program = (uint8_t*)malloc((4096-512) * sizeof(uint8_t));
    size_t program_size = fread(program, sizeof(uint8_t), (4096-512),fp);
    if(program_size<=0) {
        SDL_Log("No program in file.");
        return -2;
    }

    chip_vm_state* vm = vm_create_state(program,program_size);
    fclose(fp);
    free(program);
    if(vm == NULL) {
        SDL_Log("Problems with creating VM.");
        return -3;
    }

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) <0) {
        SDL_Log("Error while initializing SDL: %s\n",SDL_GetError());
        return -4;
    }

    SDL_zero(audio_want);
    SDL_zero(audio_have);
    audio_want.freq = 44100;
    audio_want.format = AUDIO_S16SYS;
    audio_want.channels = 1;
    audio_want.samples = 512;
    audio_want.callback = audio_callback;
    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &audio_want, &audio_have, 0);
    if (dev == 0) {
        SDL_Log("Failed to open audio: %s\n", SDL_GetError());
    } 
    else {
        if (audio_have.format != audio_want.format) { /* we let this one thing change. */
            SDL_Log("We didn't get AUDIO_S16SYS audio format.\n");
        }
    }

    SDL_Window* window = SDL_CreateWindow("Yet another Chip8 interpreter ++", 
            SDL_WINDOWPOS_UNDEFINED, 
            SDL_WINDOWPOS_UNDEFINED, 
            64 * PIXEL_SIZE, 32 * PIXEL_SIZE,
            0);
    if(window == NULL) {
        SDL_Log("Error while creating window: %s\n", SDL_GetError());
        return -5;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL) {
        SDL_Log("Error while creating renderer: %s\n", SDL_GetError());
        return -6;
    }

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    if(surface == NULL) {
        SDL_Log("Error while geting window surface: %s\n", SDL_GetError());
        return -7;
    }

    int pitch = surface->pitch;

    SDL_Texture* texture = SDL_CreateTexture(renderer,surface->format->format,SDL_TEXTUREACCESS_STREAMING, 64 * PIXEL_SIZE, 32 * PIXEL_SIZE);
    if(texture == NULL) {
        SDL_Log("Error while getting window texture: %s\n", SDL_GetError());
        return -8;
    }

    bool running = true;
    float elapsedMS  = 0;
    uint8_t* raw_surface = malloc(sizeof(uint8_t)* 512*4096);
    while(running) {
        Uint64 start = SDL_GetPerformanceCounter();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT: {
                                   running = false;
                                   break;
                               }
                case SDL_WINDOWEVENT: {
                                          if(event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)) {
                                              running = false;
                                          }
                                          break;
                                      }
                case SDL_KEYUP: {

                                    uint8_t key = translate_sdl_key(event.key.keysym.sym);
                                    if(key != 0xFF) {
                                        vm->pressed_keys &=  ~(1 << (key));
                                    }
                                    break;
                                }
                case SDL_KEYDOWN: {
                                      uint8_t key = translate_sdl_key(event.key.keysym.sym);
                                      if(key != 0xFF) {
                                          vm->pressed_keys |=  (1 << (key));
                                      }
                                      break;
                                  }
            }
        }
        if(elapsedMS >=16.666f) {

            for(int x=0;x<64;x++) {
                for(int y=0;y<32;y++) {
                    int index = (y * PIXEL_SIZE * pitch) + (x * PIXEL_SIZE * 4);
                    if((index + PIXEL_SIZE) >= (512*4096)) {
                        SDL_Log("Failed to update pixel x:%d y:%d\n",x,y);
                        continue;
                    }
                    for(int i = 0;i < PIXEL_SIZE; i++) {
                        int currentLineIndex = index + (i*pitch);
                        for(int j=0;j<PIXEL_SIZE; j++) {
                            int currentIndex = currentLineIndex +(j*4);
                            raw_surface[currentIndex] = 0; //blue
                            raw_surface[currentIndex +1] = 0; //green
                            if(vm->vm_surface[x][y]) {
                                raw_surface[currentIndex+2] = 0xff; //red
                                raw_surface[currentIndex +3] = 0; //alpha
                            }
                            else {
                                if (raw_surface[currentIndex + 3] > 0) {
                                    raw_surface[currentIndex + 3] = 0;
                                    raw_surface[currentIndex + 2] = 0;
                                }
                                else {
                                    raw_surface[currentIndex + 3] = 0x7f;
                                }
                            }
                        }
                    }
                } 
            }

            SDL_UpdateTexture(texture,NULL, raw_surface, pitch);
            SDL_RenderCopy(renderer, texture, NULL,NULL);
            SDL_RenderPresent(renderer);
            vm_execute(vm, 7);
            vm_tick(vm);
            elapsedMS = 0;
        }
        if(vm->st>0) {
            SDL_PauseAudioDevice(dev, 0);
        }
        else {
            SDL_PauseAudioDevice(dev, 1);
        }
        Uint64 end = SDL_GetPerformanceCounter();
        elapsedMS += (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
    }

    free(raw_surface);
    vm_destroy_state(vm);

    SDL_CloseAudioDevice(dev);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

Uint16 get_sample_data(int sample) {
    static double samplingIndex = 0;
    static const double sound_frequency = 261.63;
    static const double amplitude = 28000;

    Uint16 result = (Uint16) ( amplitude* sin(samplingIndex));
    samplingIndex += (2.0 * PI * sound_frequency) / audio_have.freq;
    //we want to wrap around after we make full circle
    if (samplingIndex >= (2.0 * PI)) {
        samplingIndex -= 2.0 * PI;
    }
    return result;
}

void audio_callback(void*  userdata, Uint8* stream, int len) {
    Uint16* stream_16 = (Uint16 *)stream;
    for (int sample = 0; sample < audio_have.samples; sample++) {
        Uint16 data = get_sample_data(sample);
        for (int channelId = 0; channelId < audio_have.channels; channelId++) {
            int offset = (sample * audio_have.channels) + channelId;
            stream_16[offset] = data;
        }
    }
}
uint8_t translate_sdl_key(SDL_Keycode sdl_key)
{
    switch(sdl_key)
    {       
        case SDLK_1:
            return 0x1;
        case SDLK_2:
            return 0x2;
        case SDLK_3: 
            return 0x3;
        case SDLK_4: 
            return 0xC;
        case SDLK_q:
            return 0x4;
        case SDLK_w:
            return 0x5;
        case SDLK_e:
            return 0x6;
        case SDLK_r:
            return 0xD;
        case SDLK_a: 
            return 0x7;
        case SDLK_s:
            return 0x8;
        case SDLK_d:
            return 0x9;
        case SDLK_f:
            return 0xE;
        case SDLK_z:
            return 0xA;
        case SDLK_x:
            return 0x0;
        case SDLK_c:
            return 0xB;
        case SDLK_v:
            return 0xF;
    }
    return 0xFF;
}

