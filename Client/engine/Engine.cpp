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

static SDL_Window* sdl_window;
static SDL_Renderer* sdl_renderer;

//static SDL_Texture* sdl_texture; // send to gpu

static SDL_Texture* font;



inline void drawChar(char c, int x, int y, const SDL_Color &color, uint8_t size) {
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

float Engine::Sprite::CAMERA_SCALE = 2;
float Engine::Sprite::CAMERA_X = 0;
float Engine::Sprite::CAMERA_Y = 0;

static std::unordered_map<std::string, Engine::Sprite**> sprites_to_load;

namespace Engine {

    SDL_Texture* loadTexture(std::string path)
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

    //std::

	void init() {
        SDL_CreateWindowAndRenderer(800, 600, 
            SDL_WINDOW_SHOWN 
            | SDL_RENDERER_PRESENTVSYNC
            | SDL_RENDERER_ACCELERATED, 
            &sdl_window, &sdl_renderer);

        //sdl_texture = SDL_CreateTexture(sdl_renderer,
        //    SDL_PIXELFORMAT_ARGB8888,
        //    SDL_TEXTUREACCESS_STREAMING,
        //    800, 600);

        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags))
        {
            printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        }

        SDL_SetWindowTitle(sdl_window, "Client");

        
        font = loadTexture("resources/font7x11.png");

        for (auto&& pair : sprites_to_load) {
            *(pair.second) = new Sprite(pair.first);
        }

        sprites_to_load.clear();

#ifndef _WIN32
        signal(SIGINT, SIG_DFL);
#endif
        assert(sdl_renderer);
        assert(sdl_window);
        //assert(sdl_texture);
	}

    void uninit() {
        //Free loaded image
        // https://gamedev.stackexchange.com/questions/114131/will-sdl-destroyrenderer-destroy-every-texture-created-with-it
        //SDL_DestroyTexture(font);


        //Quit SDL subsystems
        IMG_Quit();
        SDL_Quit();
    }

    void doRender() {
        //SDL_UpdateTexture(sdl_texture, NULL, screen, SCREEN_WIDTH * sizeof(COLOR));
        //SDL_RenderClear(sdl_renderer);
        //SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, NULL);
        SDL_RenderPresent(sdl_renderer);
    }

    void drawTexture(SDL_Texture* texture, 
        SDL_Rect &src_rect, SDL_Rect &rect) {
        SDL_RenderCopy(sdl_renderer, texture, &src_rect, &rect);
    }

    void drawTexture(SDL_Texture* texture,
        SDL_Rect& src_rect, SDL_Rect& rect, double angle) {
        SDL_RenderCopyEx(sdl_renderer, texture, &src_rect, &rect, angle, NULL, SDL_RendererFlip::SDL_FLIP_NONE);
    }

    //void drawSprite(SDL_Texture* texture, float x, float y, SDL_Rect& src_rect) {
    //    //SDL_RenderCopy(sdl_renderer, texture, &src_rect, &rect);
    //
    //    //Animation& anim = animations[cur_anim];
    //
    //    //SDL_Rect srcrect = { anim.frame_x + cur_frame * w,
    //    //    anim.frame_y, w, h };
    //
    //
    //    unsigned int scale_h_off = (float)src_rect.w * CAMERA_SCALE * 0.5f;
    //    unsigned int scale_v_off = (float)src_rect.h * CAMERA_SCALE * 0.5f;
    //
    //    SDL_Rect dstrect = { CAMERA_X + x - scale_h_off,
    //        CAMERA_Y - y - scale_v_off,
    //        (float)src_rect.w * CAMERA_SCALE, (float)src_rect.h * CAMERA_SCALE };
    //
    //    Engine::drawTexture(texture, src_rect, dstrect);
    //
    //}

    static SDL_Color colors[] = {
        {0x00, 0x00, 0x00, 0xFF},
        {0x00, 0x00, 0xAA, 0xFF},
        {0x00, 0xAA, 0x00, 0xFF},
        {0x00, 0xAA, 0xAA, 0xFF},
        {0xAA, 0x00, 0x00, 0xFF},
        {0xAA, 0x00, 0xAA, 0xFF},
        {0xFF, 0xAA, 0x00, 0xFF},
        {0xAA, 0xAA, 0xAA, 0xFF},
        {0x55, 0x55, 0x55, 0xFF},
        {0x55, 0x55, 0xFF, 0xFF},

        {0x55, 0xFF, 0x55, 0xFF},
        {0x55, 0xFF, 0xFF, 0xFF},
        {0xFF, 0x55, 0x55, 0xFF},
        {0xFF, 0x55, 0xFF, 0xFF},
        {0xFF, 0xFF, 0x55, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF}
    };

    void drawFormattedString(const std::string& str, int x, int y, uint8_t alpha, int size, bool centered) {

        //SDL_SetTextureColorMod(font, colors[)

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
            //for (uint16_t i = 0; i < str.length(); i++) {

            //char &c = str[i];

            while (str[i] == '\n') {
                y += 9 * size;//  7 * 
                //copy_x = x;
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
        
        //SDL_SetTextureColorMod(font, colors[)

        const int w = 7 * size * str.length(),
                  h = 11 * size;
        
        if (centered) {
            x -= w / 2;
            y -= h / 2;
        }

        unsigned int i = 0;
        while (i < str.length()) {
        //for (uint16_t i = 0; i < str.length(); i++) {

            //while (str[i] == '\n') {
            //    x += 7;
            //    i++;
            //}

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

    //SDL_Texture* sprite_sheet;

    //Sprite::Sprite(float x, float y,
    //    uint8_t sheet_x, uint8_t sheet_y,
    //    uint8_t frame_count)
    //    : x(x), y(y),
    //    vx(0), vy(0),
    //    sheet_x(sheet_x), sheet_y(sheet_y),
    //    sheet_w(16), sheet_h(16),
    //    frame_count(frame_count),
    //    frame_time(1000) { }

    bool ends_with(std::string const& fullString, std::string const& ending) {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
        }
        else {
            return false;
        }
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

        //printf("parsing tiles.json\n");

        char readBuffer[65536];
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
            //std::string name = (*itr)["id"].GetString();

            std::vector<uint16_t> durations;

            uint16_t frame_x = static_cast<uint16_t>((*itr)["x"].GetUint());
            uint16_t frame_y = static_cast<uint16_t>((*itr)["y"].GetUint());

            for (auto& item : (*itr)["durations"].GetArray()) {
                durations.push_back(item.GetUint());
            }
            
            animations.push_back({ frame_x, frame_y, durations });
        }

    }

    //Sprite::Sprite(float x, float y,
    //    uint8_t current_frame)
    //    : x(x), y(y),
    //    vx(0), vy(0),
    //    frame_x(frame_x), frame_y(frame_y),
    //    frame_w(frame_w), frame_h(frame_h),
    //    current_frame(0), current_time(0) { }
    //

    //void Sprite::draw(float x, float y) {
    //
    //    SDL_Rect srcrect = { 0, 0, w, h };
    //
    //    unsigned int scale_h_off = (float)w * CAMERA_SCALE * 0.5f;
    //    unsigned int scale_v_off = (float)h * CAMERA_SCALE * 0.5f;
    //
    //    SDL_Rect dstrect = { CAMERA_X + x - scale_h_off,
    //        CAMERA_Y - y - scale_v_off,
    //        (float)w * CAMERA_SCALE, (float)h * CAMERA_SCALE };
    //
    //    Engine::drawTexture(sprite_sheet, srcrect, dstrect, 0);
    //}

    void Sprite::draw(float x, float y, uint8_t cur_frame, uint8_t cur_anim, double angle) {

        Animation& anim = animations[cur_anim];
        SDL_Rect srcrect = { anim.frame_x + cur_frame * w, 
            anim.frame_y, w, h };

        unsigned int scale_h_off = (float)w * CAMERA_SCALE * 0.5f;
        unsigned int scale_v_off = (float)h * CAMERA_SCALE * 0.5f;

        SDL_Rect dstrect = { CAMERA_X + x - scale_h_off,
            CAMERA_Y - y - scale_v_off,
            (float)w * CAMERA_SCALE, (float)h * CAMERA_SCALE };

        Engine::drawTexture(sprite_sheet, srcrect, dstrect, angle);
    }

    void Sprite::queueLoad(std::string filename, Sprite** sprite) {
        if (!sdl_renderer)
            sprites_to_load.insert({ filename, sprite });
        else {
            // set the value of sprite
            *sprite = new Sprite(filename);
        }
    }

}