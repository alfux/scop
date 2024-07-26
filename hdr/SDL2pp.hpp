#ifndef SDL2PP_HPP
# define SDL2PP_HPP
# include <Error.hpp>
# include <SDL.h>

/*
 * Wrapper class for the SDL2 library functionalities.
 */
class	SDL2pp
{
	static size_t	instances;

	Uint32		init_flags;
	SDL_Window	*window;
	SDL_Event	events;

	public:
		SDL2pp(void);
		SDL2pp(Uint32 flags);
		SDL2pp(SDL2pp const &cpy);
		virtual ~SDL2pp(void) noexcept;

		SDL2pp	&operator=(SDL2pp const &cpy);

		int		main() const;
};

#endif
