#include <assert.h>
#include <string>
#include <iostream>
#include <unordered_map>

#include <SDL.h>
#include <SDL_image.h>
#ifndef _WIN32
#include <signal.h>
#endif

#include "Engine.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"

using namespace rapidjson;

class Engine::Sprite;

float Engine::Sprite::CAMERA_SCALE = 2;
float Engine::Sprite::CAMERA_X = 0;
float Engine::Sprite::CAMERA_Y = 0;

static SDL_Window* sdl_window;
static SDL_Renderer* sdl_renderer;
// store the { filename, sdl_texture }
static std::unordered_map<std::string, Engine::Sprite*> sprites;
static std::unordered_map<std::string, std::vector<Engine::Sprite**>> sprite_loader;
static SDL_Texture* font;
static SDL_Color colors[] = {
    Engine::BLACK,
    Engine::DARK_BLUE,
    Engine::DARK_GREEN,
    Engine::DARK_AQUA,
    Engine::DARK_RED,
    Engine::DARK_PURPLE,
    Engine::GOLD,
    Engine::GRAY,
    Engine::DARK_GRAY,
    Engine::BLUE,
    Engine::GREEN,
    Engine::AQUA,
    Engine::RED,
    Engine::LIGHT_PURPLE,
    Engine::YELLOW,
    Engine::WHITE
};

static void drawTexture(SDL_Texture* texture,
    SDL_Rect& src_rect, SDL_Rect& rect) {
    SDL_RenderCopy(sdl_renderer, texture, &src_rect, &rect);
}

static void drawTexture(SDL_Texture* texture,
    SDL_Rect& src_rect, SDL_Rect& rect, double angle) {
    SDL_RenderCopyEx(sdl_renderer, texture, &src_rect, &rect, angle, NULL, SDL_RendererFlip::SDL_FLIP_NONE);
}

static void drawChar(char c, int x, int y, const SDL_Color &color, uint8_t size) {
    if (c >= 32 && c <= 126) {
        int offset = (int)(c - 32) * 7;

        //const int startingScreenX = x + (int)(i * fonts[0].w * size) + centerXOffset;
        //const int startingScreenY = y + centerYOffset;
        //
        //int currentScreenX = startingScreenX;
        //int currentScreenY = startingScreenY;

        SDL_SetTextureColorMod(font, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(font, color.a);

        SDL_Rect srcrect = { offset, 0, 7, 11 };
        SDL_Rect dstrect = { x, y, 7*size, 11*size };

        SDL_RenderCopy(sdl_renderer, font, &srcrect, &dstrect);
    }
}

static SDL_Texture* loadTexture(std::string path)
{
    //The final texture
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else
    {
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(sdl_renderer, loadedSurface);
        if (newTexture == NULL)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}

static bool ends_with(std::string const& fullString, std::string const& ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    else {
        return false;
    }
}

namespace Engine {

	void init() {
        SDL_CreateWindowAndRenderer(800, 600, 
            SDL_WINDOW_SHOWN 
            | SDL_RENDERER_PRESENTVSYNC
            | SDL_RENDERER_ACCELERATED, 
            &sdl_window, &sdl_renderer);

        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags))
        {
            printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        }

        SDL_SetWindowTitle(sdl_window, "Client");

        
        font = loadTexture("resources/font7x11.png");

        // Load all queued sprites
        for (auto&& pair : sprite_loader) {
            auto&& sprite = new Sprite(pair.first);
            for (size_t i = 0; i < pair.second.size(); i++) {
                *(pair.second[i]) = sprite;
            }
            // add to sprites library
            sprites.insert({ pair.first, sprite });
        }
        sprite_loader.clear();

#ifndef _WIN32
        signal(SIGINT, SIG_DFL);
#endif
        assert(sdl_renderer);
	}

    void uninit() {

        /*
        * SDL will free everything
        * https://gamedev.stackexchange.com/questions/114131/will-sdl-destroyrenderer-destroy-every-texture-created-with-it
        */

        //Quit SDL subsystems
        IMG_Quit();
        SDL_Quit();
    }

    void doRender() {
        SDL_RenderPresent(sdl_renderer);
    }

    void drawFormattedString(const std::string& str, int x, int y, uint8_t alpha, int size, bool centered) {

        const int w = 7 * size * str.length(),
            h = 11 * size;

        if (centered) {
            x -= w / 2;
            y -= h / 2;
        }

        //int copy_x = x;

        SDL_Color color = { 255, 255, 255, alpha };

        unsigned int i = 0;
        unsigned int draw_i = 0;
        while (i < str.length()) {

            while (str[i] == '\n') {
                y += 9 * size;
                draw_i = 0;
                i++;
            }

            // detect a color code &ccc...
            if (str[i] == '&') {
                const char c1 = str[i + 1];

                if (c1 >= '0' && c1 <= '9') {
                    color = colors[c1 - '0'];
                    i += 2;
                }
                else if (c1 >= 'a' && c1 <= 'f') {
                    color = colors[c1 - 'a' + 10];
                    i += 2;
                }
            }

            drawChar(str[i], x + draw_i * 7 * size, y, color, size);
            draw_i++;
            i++;
        }

    }

    void drawString(const std::string &str, int x, int y, const SDL_Color &color, int size, bool centered) {
        
        const int w = 7 * size * str.length(),
                  h = 11 * size;
        
        if (centered) {
            x -= w / 2;
            y -= h / 2;
        }

        unsigned int i = 0;
        while (i < str.length()) {
            drawChar(str[i], x + i * 7 * size, y, color, size);
            i++;
        }        
    }

    void fill(const SDL_Color &c) {
        SDL_SetRenderDrawColor(sdl_renderer, c.r, c.g, c.b, c.a);
        SDL_RenderFillRect(sdl_renderer, NULL);
    }

    void fillRect(const SDL_Color& c, int x, int y, int w, int h) {
        SDL_Rect rect = { x, y, w, h };
        SDL_SetRenderDrawColor(sdl_renderer, c.r, c.g, c.b, c.a);
        SDL_RenderFillRect(sdl_renderer, &rect);
    }

    Sprite::Sprite(std::string filename) {
        FILE* f = fopen(filename.c_str(), "rb");

        if (!f) {
            std::cout << filename << " was not found\n";
            return;
        }

        if (!ends_with(filename, ".json")) {
            // load png directly
            sprite_sheet = loadTexture(filename.c_str());
            //SDL_Point pt;
            int _w, _h;
            SDL_QueryTexture(sprite_sheet, NULL, NULL, &_w, &_h);

            this->w = static_cast<uint16_t>(_h);
            this->h = static_cast<uint16_t>(_h);
            this->animations[0].durations.push_back(0);
            this->animations[0].frame_x = 0;
            this->animations[0].frame_y = 0;

            return;
        }

        char* readBuffer = new char[65536];
        FileReadStream is(f, readBuffer, sizeof(readBuffer));

        Document d;
        d.ParseStream(is);

        fclose(f);

        // relative paths simplicity
        sprite_sheet = loadTexture(std::string("resources/") + d["filename"].GetString());
        w = d["width"].GetUint();
        h = d["height"].GetUint();

        const Value& V_anim = d["animations"];

        for (Value::ConstValueIterator itr = V_anim.Begin(); itr != V_anim.End(); ++itr) {
            std::vector<uint16_t> durations;

            uint16_t frame_x = static_cast<uint16_t>((*itr)["x"].GetUint());
            uint16_t frame_y = static_cast<uint16_t>((*itr)["y"].GetUint());

            for (auto& item : (*itr)["durations"].GetArray()) {
                durations.push_back(item.GetUint());
            }
            
            animations.push_back({ frame_x, frame_y, durations });
        }

        delete[] readBuffer;
    }

    void Sprite::draw(float x, float y, uint8_t cur_frame, uint8_t cur_anim, double angle) {

        Animation& anim = animations[cur_anim];
        SDL_Rect srcrect = { anim.frame_x + cur_frame * w, 
            anim.frame_y, w, h };

        unsigned int scale_h_off = (float)w * CAMERA_SCALE * 0.5f;
        unsigned int scale_v_off = (float)h * CAMERA_SCALE * 0.5f;

        SDL_Rect dstrect = { CAMERA_X + x - scale_h_off,
            CAMERA_Y - y - scale_v_off,
            (float)w * CAMERA_SCALE, (float)h * CAMERA_SCALE };

        drawTexture(sprite_sheet, srcrect, dstrect, angle);
    }

    void Sprite::queueLoad(std::string filename, Sprite** sprite) {
        /*
        * if sprite already has been loaded into library
        */
        auto&& find = sprites.find(filename);
        if (find != sprites.end()) {
            // dont load a new instance of sprite
            *sprite = find->second;
        }
        else { // sprite has not been loaded


            // if sdl has been fully loaded, immediately load a new texture
            if (sdl_renderer) {
                (*sprite) = new Sprite(filename);

                // then add into sprites
                sprites.insert({ filename, *sprite });
            }
            else { // sdl hasn't been loaded (queue sprite member for loading)

                auto&& find_loader = sprite_loader.find(filename);
                // if it doesnt exist insert it
                if (find_loader == sprite_loader.end())
                    sprite_loader.insert({ filename, std::vector<Sprite**>() });

                // insert the sprite
                find_loader->second.push_back(sprite);
            }
        }
    }

}