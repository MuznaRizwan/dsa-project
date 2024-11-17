#ifndef BASE_H
#define BASE_H
class BaseScreen {
	public:
		handleEvents(SDL_Event event); //-keymouse keypress
		cleanUp();//-clean free surfaces
		load(SDL_Renderer* renderer);//-load the textures, font
		render();//-screen draw
};
#endif