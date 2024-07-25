#include <SDL2pp.hpp>

size_t	SDL2pp::instances = 0;

/**
 * Initializes every SDL2 library subsystems if it's the first instance
 * and sets internal variables to default state.
 */
SDL2pp::SDL2pp(void): init_flags(SDL_INIT_EVERYTHING), window(nullptr)
{
	if (!SDL2pp::instances && SDL_Init(init_flags))
		throw (Error("SDL2pp(void)", SDL_GetError()));
	memset(&this->events, 0, sizeof (SDL_Event));
	++SDL2pp::instances;
}

/**
 * Initializes SDL2 library subsystems according to flags if it's the
 * first instance and sets internal variables to default state.
 */
SDL2pp::SDL2pp(Uint32 flags): init_flags(flags), window(nullptr)
{
	if (!SDL2pp::instances && SDL_Init(init_flags))
		throw (Error("SDL2pp(Uint32)", SDL_GetError()));
	memset(&this->events, 0, sizeof (SDL_Event));
	++SDL2pp::instances;
}

/**
 * Doesn't initializes SDL2 library subsystems as the copy constructor suppose
 * an instance already exists. Sets internal variables to default state.
 */
SDL2pp::SDL2pp(SDL2pp const &cpy): init_flags(cpy.init_flags), window(nullptr)
{
	if (cpy.window && !(this->window = SDL_CreateWindowFrom(cpy.window)))
		throw (Error("SDL2pp(SDL2pp const &)", SDL_GetError()));
	this->events = cpy.events;
	++SDL2pp::instances;
}

/**
 * Destroys window and quits SDL2 library subsystems if it's the last instance.
 */
SDL2pp::~SDL2pp(void)
{
	--SDL2pp::instances;
	if (this->window)
		SDL_DestroyWindow(this->window);
	if (!SDL2pp::instances)
		SDL_Quit();
}

/**
 * Destroys current window if there is one, then copies the other instance.
 */
SDL2pp	&SDL2pp::operator=(SDL2pp const &cpy)
{
	this->init_flags = cpy.init_flags;
	if (this->window)
	{
		SDL_DestroyWindow(this->window);
		this->window = nullptr;
	}
	if (cpy.window && !(this->window = SDL_CreateWindowFrom(cpy.window)))
		throw (Error("SDL2pp &operator=(SDL2pp const &)", SDL_GetError()));
	this->events = cpy.events;
	return (*this);
}

/**
 * Will hold the main loop of the SDL2 application with event gestion.
 */
int	SDL2pp::main() const
{
	return (0);
}
