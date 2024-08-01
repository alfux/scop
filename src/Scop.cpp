#include <Scop.hpp>

Scop::Scop(void) : sdl(SDL_INIT_EVERYTHING)
{
	sdl.addWindow("scop", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SCOP_WINDOW_WIDTH, SCOP_WINDOW_HEIGHT, SDL_WINDOW_VULKAN);
	initVulkan();
}

Scop::Scop(const Scop &cpy) : sdl(cpy.sdl)
{
	sdl.addWindow("scop", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SCOP_WINDOW_WIDTH, SCOP_WINDOW_HEIGHT, SDL_WINDOW_VULKAN);
	initVulkan();
}

void Scop::initVulkan(void)
{
	createInstance();
}

void Scop::createInstance(void)
{
	std::vector<const char *> extentions;
	VkApplicationInfo app_info{};
	VkInstanceCreateInfo create_info{};

	sdl.getVulkanExtensions(extentions);
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Scop";
	app_info.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	app_info.pEngineName = "No Engine";
	app_info.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	create_info.enabledExtensionCount = static_cast<Uint32> (extentions.size());
	create_info.ppEnabledExtensionNames = extentions.data();
	create_info.enabledLayerCount = 0;
	create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
	sdl.vkCreateInstance(create_info, nullptr, instance);
}

Scop::~Scop(void) noexcept
{
	// Empty;
}

Scop &Scop::operator=(const Scop &cpy)
{
	sdl = cpy.sdl;
	return (*this);
}

bool Scop::manageEvent()
{
	SDL_Event event;

	while (sdl.pollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				return (false);
				break ;
			default:
				break ;
		}
	}

	return (true);
}

void Scop::mainLoop(void)
{
	while (manageEvent())
	{
		// Empty;
	}
}