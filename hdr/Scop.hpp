#ifndef SCOP_HPP
# define SCOP_HPP

# define SCOP_WINDOW_WIDTH 1280
# define SCOP_WINDOW_HEIGHT 720

# include <SDL2pp.hpp>

class Scop
{
	SDL2pp sdl;
	VkInstance instance;

	public:
		Scop(void);
		Scop(const Scop &cpy);
		virtual ~Scop(void) noexcept;
	
		Scop &operator=(const Scop &cpy);

		bool manageEvent(void);
		void initVulkan(void);
		void createInstance(void);
		void mainLoop(void);
};

#endif