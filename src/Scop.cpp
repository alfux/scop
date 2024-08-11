#include <Scop.hpp>

Scop::Scop(void) :
	sdl{SDL_INIT_EVERYTHING},
	width{1280},
	height{720},
	validationLayers{"VK_LAYER_KHRONOS_validation"},
#ifdef NDEBUG
	enableValidationLayers(false)
#else
	enableValidationLayers(true)
#endif
{
	sdl.addWindow("scop", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SCOP_WINDOW_WIDTH, SCOP_WINDOW_HEIGHT, SDL_WINDOW_VULKAN);
	initVulkan();
}

Scop::Scop(const Scop &cpy) :
	sdl{cpy.sdl},
	width{cpy.width},
	height{cpy.height},
	validationLayers{cpy.validationLayers},
#ifdef NDEBUG
	enableValidationLayers(false)
#else
	enableValidationLayers(true)
#endif
{
	sdl.addWindow("scop", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SCOP_WINDOW_WIDTH, SCOP_WINDOW_HEIGHT, SDL_WINDOW_VULKAN);
	initVulkan();
}

Scop::~Scop(void) noexcept
{
	vkDestroyInstance(instance, nullptr);
}

Scop &Scop::operator=(const Scop &cpy)
{
	sdl = cpy.sdl;
	instance = cpy.instance;
	validationLayers = cpy.validationLayers;
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

void Scop::initVulkan(void)
{
	createInstance();
}

/**
 * Part of the createInstance() method
 */
inline void setAppInfo(VkApplicationInfo &app_info)
{
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Scop";
	app_info.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	app_info.pEngineName = "No Engine";
	app_info.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;
}

/**
 * Creates an instance of Vulkan according to given <create_info> and 
 * <allocator>, then assigns the handle to <instance>
 */
inline void createVkInstance(const VkInstanceCreateInfo &create_info,
	const VkAllocationCallbacks *allocator, VkInstance &instance)
{
	uint32_t count = 0;

	vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

	std::vector<VkExtensionProperties> properties(count);

	vkEnumerateInstanceExtensionProperties(nullptr, &count, properties.data());
	// std::cout << "Available extensions:" << std::endl;
	// for (const auto &extension : properties)
	// {
	// 	std::cout << '\t' << extension.extensionName << std::endl;
	// }

	// std::cout << "\nRequired extension:" << std::endl;
	// for (uint32_t i = 0; i < create_info.enabledExtensionCount; ++i)
	// {
	// 	std::cout << '\t' << create_info.ppEnabledExtensionNames[i] << '\n';
	// }

	if (::vkCreateInstance(&create_info, allocator, &instance) != VK_SUCCESS)
	{
		throw (Error("SDL2pp::vkCreateInstance", "failed to create instance"));
	}

	// std::cout << "Vulkan instance created successfully" << std::endl;
}

void Scop::setupDebugMessenger(void)
{
	if (enableValidationLayers)
	{
		VkDebugUtilsMessengerCreateInfoEXT create_info {};

		create_info.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
		;

		create_info.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
		;

		create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		create_info.pfnUserCallback = Scop::debugCallback;
		create_info.pUserData = nullptr;
	}
}

/**
 * Creates a Vulkan instance and sets <instance> to be the handle
 */
void Scop::createInstance(void)
{
	checkValidationLayerSupport();

	std::vector<const char *> extensions;
	VkInstanceCreateInfo create_info{};
	VkApplicationInfo app_info{};

	setAppInfo(app_info);
	sdl.getVulkanExtensions(extensions, enableValidationLayers);
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	create_info.enabledExtensionCount = static_cast<Uint32> (extensions.size());
	create_info.ppEnabledExtensionNames = extensions.data();
	create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
	create_info.enabledLayerCount = 0;
	if (enableValidationLayers)
	{
		create_info.enabledLayerCount =
			static_cast<uint32_t> (validationLayers.size());
		create_info.ppEnabledLayerNames = validationLayers.data();
	}

	createVkInstance(create_info, nullptr, instance);
}

/**
 * Part of the checkValidationLayerSupport() method
 */
inline bool checkPresence(const std::vector<VkLayerProperties> &layerProperties,
	const char *layerName)
{
	for (VkLayerProperties layerPropertie : layerProperties)
	{
		if (!strcmp(layerName, layerPropertie.layerName))
		{
			std::cout << "Validation layer " << layerName;
			std::cout << " required and found" << std::endl << std::endl;
			return (true);
		}
	}

	return (false);
}

/**
 * Checks wether required validation layers are supported or not, throws an
 * error if they aren't found
 */
void Scop::checkValidationLayerSupport()
{
	if (enableValidationLayers)
	{
		uint32_t count {0};

		vkEnumerateInstanceLayerProperties(&count, nullptr);

		std::vector<VkLayerProperties> layerProperties {count};

		vkEnumerateInstanceLayerProperties(&count, layerProperties.data());
		for (const char *layerName : validationLayers)
		{
			if (!checkPresence(layerProperties, layerName))
			{
				throw (Error("Scop::checkValidationLayerSupport",
					"Validation layer requested, but not available"));
			}
		}
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL Scop::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT  *callback_data,
    void*                                       pUserData
) {
	(void)messageSeverity;
	(void)messageTypes;
	(void)pUserData;
	std::cerr << "Validation layer: " << callback_data->pMessage << std::endl;
	return (VK_FALSE);
}

void Scop::mainLoop(void)
{
	while (manageEvent())
	{
		// Empty;
	}
}