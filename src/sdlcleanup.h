/***************************************************************************************************
    A nice little helper, thanks to:
    http://www.willusher.io/sdl2%20tutorials/2014/08/01/postscript-1-easy-cleanup/
***************************************************************************************************/

#ifndef SDLCLEANUP_H
#define SDLCLEANUP_H

#include <utility>
#include <SDL2/SDL.h>

/*
 * Recurse through the list of arguments to clean up, cleaning up
 * the first one in the list each iteration.
 */
template<typename T, typename... Args>
void sdlcleanup(T *t, Args&&... args){
    //Cleanup the first item in the list
    sdlcleanup(t);
    //Recurse to clean up the remaining arguments
    sdlcleanup(std::forward<Args>(args)...);
}
/*
 * These specializations serve to free the passed argument and also provide the
 * base cases for the recursive call above, eg. when args is only a single item
 * one of the specializations below will be called by
 * sdlcleanup(std::forward<Args>(args)...), ending the recursion
 * We also make it safe to pass nullptrs to handle situations where we
 * don't want to bother finding out which values failed to load (and thus are null)
 * but rather just want to clean everything up and let cleanup sort it out
 */
template<>
void sdlcleanup<SDL_Window>(SDL_Window *win){
    if (!win){
        return;
    }
    SDL_DestroyWindow(win);
}
template<>
void sdlcleanup<SDL_Renderer>(SDL_Renderer *ren){
    if (!ren){
        return;
    }
    SDL_DestroyRenderer(ren);
}
template<>
void sdlcleanup<SDL_Texture>(SDL_Texture *tex){
    if (!tex){
        return;
    }
    SDL_DestroyTexture(tex);
}
template<>
void sdlcleanup<SDL_Surface>(SDL_Surface *surf){
    if (!surf){
        return;
    }
    SDL_FreeSurface(surf);
}

#endif
