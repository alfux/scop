#ifndef SDL2PP_HPP
# define SDL2PP_HPP
# include <Error.hpp>
# include <vector>
# include <SDL.h>
# include <SDL_vulkan.h>
# include <vulkan/vulkan.h>
# include <vulkan/vulkan_core.h>
# include <vulkan/vulkan_beta.h>


/**
 * Wrapper class for the SDL2 library functionalities.
 */
class SDL2pp
{
	static size_t instances;

	uint32_t init_flags;
	SDL_Window *window;

	public:
		SDL2pp(void);
		SDL2pp(Uint32 flags);
		SDL2pp(SDL2pp const &cpy);
		virtual ~SDL2pp(void) noexcept;

		SDL2pp &operator=(SDL2pp const &cpy);

		void addWindow(char const *t, int x, int y, int w, int h, Uint32 flags);
		void getWindowPixelResolution(int *width, int *height);
		void vkCreateSurface(VkInstance &instance, VkSurfaceKHR &surface);
		void destroyWindow(void);
		int pollEvent(SDL_Event *event);
		void getVulkanExtensions(std::vector<const char *> &names,
			bool debug = false);
};

#endif
