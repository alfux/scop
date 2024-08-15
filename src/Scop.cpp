#include <Scop.hpp>

/**
 * Checks if the structure has a value.
 */
bool Scop::QueueFamilyIndices::isComplete(void)
{
	return (graphics_family.has_value());
}

/**
 * Default standard constructor.
 */
Scop::Scop(void) :
	sdl {SDL_INIT_EVERYTHING},
	width {1280},
	height {720},
	validation_layers {"VK_LAYER_KHRONOS_validation"},
	physical_device {VK_NULL_HANDLE},
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

/**
 * Copy constructor.
 */
Scop::Scop(const Scop &cpy) :
	sdl{cpy.sdl},
	width{cpy.width},
	height{cpy.height},
	validation_layers{cpy.validation_layers},
	physical_device {VK_NULL_HANDLE},
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

/**
 * Class destructor, cleans the class up for destruction.
 */
Scop::~Scop(void) noexcept
{
	cleanup();
}

/**
 * Copy assignement operator. Cleans up Vulkan instance, copy settings and 
 * initialize Vulkan again. This is here only for Coplien standard form
 * compliance and should not really have any use at the moment.
 */
Scop &Scop::operator=(const Scop &cpy)
{
	cleanup();
	sdl = cpy.sdl;
	validation_layers = cpy.validation_layers;
	physical_device = cpy.physical_device;
	initVulkan();
	return (*this);
}

/**
 * Management of keyboard events
 */
static inline bool keyboardEvent(SDL_Keycode &key)
{
	if (key == SDLK_ESCAPE)
	{
		return (false);
	}

	return (true);
}

/**
 * Management of events
 */
bool Scop::manageEvent()
{
	SDL_Event event;

	while (sdl.pollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				return (false);
			case SDL_KEYDOWN:
				return (keyboardEvent(event.key.keysym.sym));
			default:
				return (true);
		}
	}

	return (true);
}

/**
 * Initialization of Vulkan
 */
void Scop::initVulkan(void)
{
	createInstance();
	setupDebugMessenger();
	pickPhysicalDevice();
	createLogicalDevice();
}

/**
 * Cleans Vulkan application up before exit or reinitialisation
 */
void Scop::cleanup(void)
{
	vkDestroyDevice(device, nullptr);
	if (enableValidationLayers)
	{
		destroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
	}

	vkDestroyInstance(instance, nullptr);
}

/**
 * Part of the createInstance() method
 */
static inline void setAppInfo(VkApplicationInfo &app_info)
{
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Scop";
	app_info.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	app_info.pEngineName = "No Engine";
	app_info.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;
}

/**
 * Output << operator overload to print VkExtensionProperties.
 */
std::ostream &operator<<(std::ostream &os, 
	const std::vector<VkExtensionProperties> &properties)
{
	os << "Available extensions:" << std::endl;
	for (const auto &extension : properties)
	{
		os << '\t' << extension.extensionName << std::endl;
	}

	return (os);
}

/**
 * Creates an instance of Vulkan according to given <create_info> and 
 * <allocator>, then assigns the handle to <instance>.
 */
static inline void createVkInstance(const VkInstanceCreateInfo &create_info,
	const VkAllocationCallbacks *allocator, VkInstance &instance)
{
	uint32_t count = 0;

	vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

	std::vector<VkExtensionProperties> properties(count);

	vkEnumerateInstanceExtensionProperties(nullptr, &count, properties.data());
	if (::vkCreateInstance(&create_info, allocator, &instance) != VK_SUCCESS)
	{
		throw (Error("SDL2pp::vkCreateInstance", "failed to create instance"));
	}
#ifndef NDEBUG
	std::cout << "Vulkan instance created successfully\n" << std::endl;
#endif
}

/**
 * Set up the nessessary structure to initialize debug utils messenger
 */
static inline void setDebugMessengerCreateInfo(
	VkDebugUtilsMessengerCreateInfoEXT &info)
{
	info.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	info.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	info.pfnUserCallback = Scop::debugCallback;
	info.pUserData = nullptr;
	info.sType =
		VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
}

/**
 * Creates a Vulkan instance and sets <instance> to be the handle
 */
void Scop::createInstance(void)
{
	checkValidationLayerSupport();

	std::vector<const char *> extensions;
	VkInstanceCreateInfo create_info {};
	VkApplicationInfo app_info {};
	VkDebugUtilsMessengerCreateInfoEXT debug_info {};

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
		setDebugMessengerCreateInfo(debug_info);
		create_info.enabledLayerCount =
			static_cast<uint32_t> (validation_layers.size());
		create_info.ppEnabledLayerNames = validation_layers.data();
		create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debug_info;
	}

	createVkInstance(create_info, nullptr, instance);
}

/**
 * Gets the debug utils messenger extension function adresss because it needs
 * to be explicitly loaded by Vulkan
 */
VkResult Scop::createDebugUtilsMessengerEXT(
	VkInstance                               instance,
	const VkDebugUtilsMessengerCreateInfoEXT *p_create_info,
	const VkAllocationCallbacks              *p_allocator,
	VkDebugUtilsMessengerEXT                 *p_debug_messenger
) {
	auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>
		(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

	return (
		func ? func(instance, p_create_info, p_allocator, p_debug_messenger)
		: VK_ERROR_EXTENSION_NOT_PRESENT
	);
}

/**
 * Same as its counterpart, loads the destroy debug utils messenger function
 */
void Scop::destroyDebugUtilsMessengerEXT(
	VkInstance                  instance,
	VkDebugUtilsMessengerEXT    p_debug_messenger,
	const VkAllocationCallbacks *p_allocator
) {
	auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>
		(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
	
	if (func)
	{
		func(instance, p_debug_messenger, p_allocator);
	}
}

/**
 * Fills the debug messenger structure for further use in the debug utils
 * messenger
 */
void Scop::setupDebugMessenger(void)
{
	if (enableValidationLayers)
	{
		VkDebugUtilsMessengerCreateInfoEXT create_info {};

		setDebugMessengerCreateInfo(create_info);
		if (createDebugUtilsMessengerEXT(instance, &create_info,
			nullptr, &debug_messenger) != VK_SUCCESS)
		{
			throw (Error("Scop::setupDebugMessenger",
				"failed to set up debug messenger"));
		}
	}
}

/**
 * Part of the checkValidationLayerSupport() method
 */
static inline bool checkPresence(const std::vector<VkLayerProperties>
	&layerProperties, const char *layerName)
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
void Scop::checkValidationLayerSupport(void)
{
	if (enableValidationLayers)
	{
		uint32_t count {0};

		vkEnumerateInstanceLayerProperties(&count, nullptr);

		std::vector<VkLayerProperties> layerProperties {count};

		vkEnumerateInstanceLayerProperties(&count, layerProperties.data());
		for (const char *layerName : validation_layers)
		{
			if (!checkPresence(layerProperties, layerName))
			{
				throw (Error("Scop::checkValidationLayerSupport",
					"Validation layer requested, but not available"));
			}
		}
	}
}

/**
 * Finds available queue families and checks presence of needed ones.
 */
static inline Scop::QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &dev)
{
	Scop::QueueFamilyIndices indices {};
	uint32_t count {0};

	vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, nullptr);

	std::vector<VkQueueFamilyProperties> properties(count);

	vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, properties.data());
	for (uint32_t i {0}; i < count; ++i)
	{
		if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphics_family = i;
		}

		if (indices.isComplete())
		{
			break ;
		}
	}

	return (indices);
}

/**
 * Checks if the designated device is suitable for the application's use.
 */
static inline bool isDeviceSuitable(const VkPhysicalDevice &device)
{
	Scop::QueueFamilyIndices indices {findQueueFamilies(device)};

	return (indices.isComplete());
}

/**
 * Picks adapted physical graphic device
 */
void Scop::pickPhysicalDevice(void)
{
	uint32_t count {0};

	vkEnumeratePhysicalDevices(instance, &count, nullptr);

	if (count == 0)
	{
		throw (Error("Scop::pickPhysicalDevice", "No GPU with Vulkan support"));
	}

	std::vector<VkPhysicalDevice> devices(count);

	vkEnumeratePhysicalDevices(instance, &count, devices.data());
	for (const auto &dev : devices)
	{
		if (isDeviceSuitable(dev))
		{
			physical_device = dev;
			break ;
		}
	}

	if (physical_device == VK_NULL_HANDLE)
	{
		throw (Error("Scop::pickPhysicalDevice", "No suitable GPU"));
	}
}

/**
 * Set queue create info properties.
 */
static inline void setQueueCreateInfo(VkDeviceQueueCreateInfo &create_info,
	Scop::QueueFamilyIndices indices)
{
	float queue_priority {1.0f};

	create_info.pQueuePriorities = &queue_priority;
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	create_info.queueFamilyIndex = indices.graphics_family.value();
	create_info.queueCount = 1;
}

/**
 * Sets the VkDeviceCreateInfo structure's informations up to create the
 * logical device.
 */
static inline void setDeviceCreateInfo(
	VkDeviceCreateInfo        &create_info,
	VkDeviceQueueCreateInfo   &queue_create_info,
	VkPhysicalDeviceFeatures  &features,
	Scop::QueueFamilyIndices  &indices,
	std::vector<const char *> &portability_subset,
	VkPhysicalDevice          &physical_device,
	std::vector<const char *> &validation_layers,
	bool                      enableValidationLayers
) {

	setQueueCreateInfo(queue_create_info, indices);
	vkGetPhysicalDeviceFeatures(physical_device, &features);
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.pQueueCreateInfos = &queue_create_info;
	create_info.queueCreateInfoCount = 1;
	create_info.pEnabledFeatures = &features;
	create_info.enabledExtensionCount = portability_subset.size();
	create_info.ppEnabledExtensionNames = portability_subset.data();
	if (enableValidationLayers)
	{
		create_info.enabledLayerCount =
			static_cast<uint32_t> (validation_layers.size());
		create_info.ppEnabledLayerNames = validation_layers.data();
	}
}

/**
 * Creates an instance of a logical device.
 */
void Scop::createLogicalDevice(void)
{
	VkDeviceCreateInfo create_info {};
	VkDeviceQueueCreateInfo queue_create_info {};
	VkPhysicalDeviceFeatures features {};
	Scop::QueueFamilyIndices indices {findQueueFamilies(physical_device)};
	std::vector<const char *> portability_subset {1};

	portability_subset[0] = VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME;
	setDeviceCreateInfo(
		create_info, queue_create_info, features, indices, portability_subset,
		physical_device, validation_layers, enableValidationLayers
	);

	if (vkCreateDevice(physical_device, &create_info, nullptr, &device)
		 != VK_SUCCESS)
	{
		throw (Error("Scop::createLogicalDevice", "failed creation"));
	}

	vkGetDeviceQueue(device, indices.graphics_family.value(),
		0, &graphics_queue);
}

/**
 * Custom debug message callback function
 */
VKAPI_ATTR VkBool32 VKAPI_CALL Scop::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT  *callback_data,
    void*                                       pUserData
) {
	(void)pUserData;
	std::cerr << "Severity: " << messageSeverity << std::endl;
	std::cerr << "Message type: " << messageTypes << std::endl;
	std::cerr << "Validation layer: " << std::endl << "\t";
	std::cerr << callback_data->pMessage << std::endl << std::endl;
	return (VK_FALSE);
}

void Scop::mainLoop(void)
{
	while (manageEvent())
	{
		// Empty;
	}
}