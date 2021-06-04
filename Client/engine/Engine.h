#pragma once
#include <SDL.h>
#include <string>
#include <vector>
namespace Engine {

	SDL_Texture* loadTexture(std::string path);

	void init();

	void uninit();

	void doRender();

	void drawTexture(SDL_Texture* texture,
		SDL_Rect& src_rect, SDL_Rect& rect);

	void drawTexture(SDL_Texture *texture, 
		SDL_Rect& src_rect, SDL_Rect& rect,
		double angle);

	void drawFormattedString(const std::string& str, 
		int x, int y, 
		uint8_t alpha, int size, bool centered);

	void drawString(const std::string& str, 
		int x, int y, 
		const SDL_Color& color, int size, bool centered);

	void fill(const SDL_Color& color);

	void fillRect(const SDL_Color& c, int x, int y, int w, int h);



	//extern SDL_Texture* sprite_sheet;

	struct Animation {

		uint16_t frame_x, frame_y; // topleft begin
		std::vector<uint16_t> durations; // constlike

	};

	class Sprite final {
	public:
		static float CAMERA_X, CAMERA_Y;
		static float CAMERA_SCALE;

		std::vector<Animation> animations; // a sprite might have different animations

	private:

		uint16_t w, h; // size of the sprite
		SDL_Texture* sprite_sheet;

	public:
		Sprite(std::string filename);

		void draw(float x, float y, uint8_t cur_frame = 0, uint8_t cur_anim = 0, double angle = 0);
	
		static void queueLoad(std::string filename, Sprite** sprite);
	};

	//void drawSprite(Sprite* sprite, float x, float y);
}