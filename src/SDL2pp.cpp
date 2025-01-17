#include <SDL2pp.hpp>

size_t	SDL2pp::instances = 0;

/**
 * Initializes every SDL2 library subsystems if it's the first instance
 * and sets internal variables to default state.
 */
SDL2pp::SDL2pp(void) : init_flags(SDL_INIT_EVERYTHING), window(nullptr)
{
	if (!SDL2pp::instances && SDL_Init(init_flags))
	{
		throw (Error("SDL2pp", SDL_GetError()));
	}

	++SDL2pp::instances;
}

/**
 * Initializes SDL2 library subsystems according to flags if it's the
 * first instance and sets internal variables to default state.
 */
SDL2pp::SDL2pp(Uint32 flags) : init_flags(flags), window(nullptr)
{
	if (!SDL2pp::instances && SDL_Init(init_flags))
	{
		throw (Error("SDL2pp", SDL_GetError()));
	}

	++SDL2pp::instances;
}

/**
 * Doesn't initializes SDL2 library subsystems as the copy constructor suppose
 * an instance already exists. Sets internal variables to default state.
 */
SDL2pp::SDL2pp(SDL2pp const &cpy) : init_flags(cpy.init_flags), window(nullptr)
{
	if (cpy.window && !(window = SDL_CreateWindowFrom(cpy.window)))
	{
		throw (Error("SDL2pp", SDL_GetError()));
	}

	++SDL2pp::instances;
}

/**
 * Destroys window and quits SDL2 library subsystems if it's the last instance.
 */
SDL2pp::~SDL2pp(void) noexcept
{
	--SDL2pp::instances;
	if (window)
	{
		SDL_DestroyWindow(window);
	}

	if (!SDL2pp::instances)
	{
		SDL_Quit();
	}
}

/**
 * Destroys current window if there is one, then copies the other instance.
 */
SDL2pp	&SDL2pp::operator=(SDL2pp const &cpy)
{
	this->init_flags = cpy.init_flags;
	if (window)
	{
		SDL_DestroyWindow(window);
		window = nullptr;
	}

	if (cpy.window && !(window = SDL_CreateWindowFrom(cpy.window)))
	{
		throw (Error("&operator=", SDL_GetError()));
	}

	return (*this);
}

/**
 * Creates specified window
 */
void SDL2pp::addWindow(const char *t, int x, int y, int w, int h, Uint32 flags)
{
	window = SDL_CreateWindow(t, x, y, w, h, flags);
	if (window == nullptr)
	{
		throw (Error("addWindow", SDL_GetError()));
	}
}

/**
 * Fills <width> and <height> variables with the corresponding pixel sizes
 * of the window
 */
void SDL2pp::getWindowPixelResolution(int *width, int *height)
{
	SDL_GL_GetDrawableSize(window, width, height);
}

/**
 * SDL_Vulkan
 */
void SDL2pp::vkCreateSurface(VkInstance &instance, VkSurfaceKHR &surface)
{
	if (SDL_Vulkan_CreateSurface(window, instance, &surface) != SDL_TRUE)
	{
		throw (Error("SDL2pp::vkCreateSurface", SDL_GetError()));
	}
}

/**
 * Destroys window
 */
void SDL2pp::destroyWindow()
{
	SDL_DestroyWindow(window);
}

/**
 * Polls for SDL Events
 */
int SDL2pp::pollEvent(SDL_Event *event)
{
	return (SDL_PollEvent(event));
}

/**
 * Fills vector <names> with Vulkan extenstions names needed to create a
 * VkInstance
 */
void SDL2pp::getVulkanExtensions(std::vector<const char *> &names, bool debug)
{
	unsigned int count = 0;

	if (!SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr))
	{
		throw (Error("SDL2pp::getVulkanExtensions", "can't get pCount"));
	}

	names.resize(count + 2);
	names[0] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
	names[1] = VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME;

	if (!SDL_Vulkan_GetInstanceExtensions(window, &count, names.data() + 2))
	{
		throw (Error("SDL2pp::getVulkanExtensions", "can't get pNames"));
	}

	if (debug)
	{
		names.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		std::cerr << "SDL Vulkan extension names:" << std::endl;
		for (const auto &name : names)
		{
			std::cerr << "\t" << name << std::endl;
		}
		std::cerr << std::endl;
	}
}