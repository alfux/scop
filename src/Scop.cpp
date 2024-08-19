#include <Scop.hpp>

/**
 * Checks if the structure has a value.
 */
bool Scop::QueueFamilyIndices::isComplete(void)
{
	return (graphic_family.has_value() && present_family.has_value());
}

/**
 * Default standard constructor.
 */
Scop::Scop(void) :
	sdl {SDL_INIT_EVERYTHING},
	width {SCOP_WINDOW_WIDTH},
	height {SCOP_WINDOW_HEIGHT},
	validation_layers {"VK_LAYER_KHRONOS_validation"},
	device_extensions {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME},
	physical_device {VK_NULL_HANDLE},
#ifdef NDEBUG
	enableValidationLayers(false)
#else
	enableValidationLayers(true)
#endif
{
	sdl.addWindow(
		"scop",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCOP_WINDOW_WIDTH,
		SCOP_WINDOW_HEIGHT,
		SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI
	);
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
	device_extensions {cpy.device_extensions},
	physical_device {VK_NULL_HANDLE},
#ifdef NDEBUG
	enableValidationLayers(false)
#else
	enableValidationLayers(true)
#endif
{
	sdl.addWindow(
		"scop",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCOP_WINDOW_WIDTH,
		SCOP_WINDOW_HEIGHT,
		SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI
	);
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
	device_extensions = cpy.device_extensions;
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
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
}

/**
 * Cleans Vulkan application up before exit or reinitialisation
 */
void Scop::cleanup(void)
{
	vkDestroySwapchainKHR(device, swapchain, nullptr);
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
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
 * Creates an instance of Vulkan according to given <create_info> and 
 * <allocator>, then assigns the handle to <instance>.
 */
static inline void createVkInstance(const VkInstanceCreateInfo &create_info,
	const VkAllocationCallbacks *allocator, VkInstance &instance)
{
	if (vkCreateInstance(&create_info, allocator, &instance) != VK_SUCCESS)
	{
		throw (Error("SDL2pp::vkCreateInstance", "failed to create instance"));
	}
}

/**
 * Set up the nessessary structure to initialize debug utils messenger
 */
static inline void setDebugMessengerCreateInfo(
	VkDebugUtilsMessengerCreateInfoEXT &debug_info)
{
	debug_info.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debug_info.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debug_info.pfnUserCallback = Scop::debugCallback;
	debug_info.pUserData = nullptr;
	debug_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
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
		create_info.enabledLayerCount =
			static_cast<uint32_t> (validation_layers.size());
		create_info.ppEnabledLayerNames = validation_layers.data();
		setDebugMessengerCreateInfo(debug_info);
		// Next line causes memory leaks, probably VkDestroyInstance forgets to
		// deallocate the utils messenger callback.
		create_info.pNext =
			reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*> (&debug_info);
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

void Scop::createSurface(void)
{
	sdl.vkCreateSurface(instance, surface);
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
 * Checks if the given queue family property contains the graphic bit flag
 * and sets <indices> structure according to the result
 */
static inline void checkGraphicSupport(VkQueueFamilyProperties &property,
	uint32_t i, Scop::QueueFamilyIndices &indices)
{
	if (property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
	{
		indices.graphic_family = i;
	}
}

/**
 * Checks if the queue family from <physical_device> at index <i> supports
 * presenting to window surface <surface> and sets <indices> structure
 * according to the result
 */
static inline void checkPresentSupport(const VkPhysicalDevice &device,
	uint32_t i, VkSurfaceKHR &surface, Scop::QueueFamilyIndices &indices)
{
	VkBool32 support {false};

	vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &support);
	if (support)
	{
		indices.present_family = i;
	}
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
 * Checks if the designated device is suitable for the application's use.
 */
bool Scop::isDeviceSuitable(const VkPhysicalDevice &device)
{
	Scop::QueueFamilyIndices indices {findQueueFamilies(device)};
	bool extensionSupported {checkDeviceExtensionSupport(device)};
	bool swapChainAdequate {false};

	if (extensionSupported)
	{
		Scop::SwapChainSupportDetails details {querySwapChainSupport(device)};
		swapChainAdequate = !details.formats.empty() && !details.modes.empty();
	}
	return (indices.isComplete() && extensionSupported && swapChainAdequate);
}

/**
 * Finds available queue families and checks presence of needed ones.
 */
Scop::QueueFamilyIndices Scop::findQueueFamilies(
	const VkPhysicalDevice &tested_device)
{
	Scop::QueueFamilyIndices indices {};
	uint32_t count {0};

	vkGetPhysicalDeviceQueueFamilyProperties(tested_device, &count, nullptr);

	std::vector<VkQueueFamilyProperties> properties(count);

	vkGetPhysicalDeviceQueueFamilyProperties(tested_device, &count,
		properties.data());
	for (uint32_t i {0}; i < count; ++i)
	{
		checkGraphicSupport(properties[i], i, indices);
		checkPresentSupport(tested_device, i, surface, indices);
		if (indices.isComplete())
		{
			break ;
		}
	}
	return (indices);
}

/**
 * Checks if the device supports required extensions
 */
bool Scop::checkDeviceExtensionSupport(const VkPhysicalDevice &tested_device)
{
	uint32_t count;

	vkEnumerateDeviceExtensionProperties(tested_device, nullptr,
		&count, nullptr);

	std::vector<VkExtensionProperties> available_extensions {count};

	vkEnumerateDeviceExtensionProperties(tested_device, nullptr,
		&count, available_extensions.data());
	
	std::set<std::string> required_extensions {device_extensions.begin(),
		device_extensions.end()};

	for (const auto &extension : available_extensions)
	{
		required_extensions.erase(extension.extensionName);
	}
	return (required_extensions.empty());
}

/**
 * Fills the SwapChainSupportDetails structure with information about available
 * features of the device swap chains
 */
Scop::SwapChainSupportDetails Scop::querySwapChainSupport(
	const VkPhysicalDevice &dev)
{
	Scop::SwapChainSupportDetails details {};

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev, surface,
		&details.capabilities);
	
	uint32_t count {0};

	vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &count, nullptr);
	if (count != 0)
	{
		details.formats.resize(count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &count,
			details.formats.data());
	}
	vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &count, nullptr);
	if (count != 0)
	{
		details.modes.resize(count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &count,
			details.modes.data());
	}
	return (details);
}

/**
 * Choose the best suitable format for the swap chain.
 */
VkSurfaceFormatKHR Scop::chooseSwapSurfaceFormat(
	const std::vector<VkSurfaceFormatKHR> &available_formats)
{
	for (const auto &format : available_formats)
	{
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB
			&& format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return (format);
		}
	}
	return (available_formats[0]);
}

/**
 * Choose the best suitable mode for the swap chain. FIFO is the default,
 * MAILBOX is like FIFO but instead of blocking when the chain is full, it
 * replaces images by the newer ones
 */
VkPresentModeKHR Scop::chooseSwapPresentMode(
	const std::vector<VkPresentModeKHR> &available_modes)
{
	for (const auto &mode : available_modes)
	{
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return (mode);
		}
	}
	return (VK_PRESENT_MODE_FIFO_KHR);
}

/**
 * Quick inlined clamp function
 */
static inline uint32_t clampValue(uint32_t value, uint32_t min, uint32_t max)
{
	if (value <= min)
	{
		return (min);
	}
	if (value >= max)
	{
		return (max);
	}
	return (value);
}

/**
 * Choose the best image resolution in the swap chain, either by default if 
 * Vulkan sets values or with the window manager values if it is allowed by the
 * uint32_t max special value in the capabilities structure set by Vulkan.
 */
VkExtent2D Scop::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
	if (capabilities.currentExtent.width != static_cast<uint32_t> (-1))
	{
		return (capabilities.currentExtent);
	}
	
	int width {0};
	int height {0};

	sdl.getWindowPixelResolution(&width, &height);

	VkExtent2D actual_extent {
		static_cast<uint32_t> (width),
		static_cast<uint32_t> (height)
	};

	actual_extent.width = clampValue(actual_extent.width,
		capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	actual_extent.height = clampValue(actual_extent.height,
		capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
	return (actual_extent);
}

/**
 * Sets necessary informations to create the swapchain
 */
static inline void setSwapchainCreateInfo(
	VkSwapchainCreateInfoKHR      &create_info,
	Scop::SwapChainSupportDetails &support,
	VkSurfaceFormatKHR            &format,
	VkPresentModeKHR              &present_mode,
	VkExtent2D                    &extent,
	uint32_t                      image_count,
	Scop::QueueFamilyIndices      &indices,
	uint32_t                      *queue_indices,
	VkSurfaceKHR                  &surface
) {
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = surface;
	create_info.minImageCount = image_count;
	create_info.imageFormat = format.format;
	create_info.imageColorSpace = format.colorSpace;
	create_info.imageExtent = extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (indices.graphic_family.value() != indices.present_family.value())
	{
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queue_indices;
	}
	else
	{
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	create_info.preTransform = support.capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = present_mode;
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = VK_NULL_HANDLE;
}

/**
 * Get images handles.
 */
static inline void getImagesHandles(VkDevice &device, VkSwapchainKHR swapchain,
	std::vector<VkImage> &swapchain_images)
{
	uint32_t count {0};

	vkGetSwapchainImagesKHR(device, swapchain, &count, nullptr);

	swapchain_images.resize(count);

	vkGetSwapchainImagesKHR(device, swapchain, &count, swapchain_images.data());
}

/**
 * Creates the swapchain to manage images display.
 */
void Scop::createSwapChain(void)
{
	VkSwapchainCreateInfoKHR create_info {};
	SwapChainSupportDetails support {querySwapChainSupport(physical_device)};
	VkSurfaceFormatKHR format {chooseSwapSurfaceFormat(support.formats)};
	VkPresentModeKHR present_mode {chooseSwapPresentMode(support.modes)};
	uint32_t image_count = support.capabilities.minImageCount + 1;
	QueueFamilyIndices indices = findQueueFamilies(physical_device);
	uint32_t queue_indices[] {indices.graphic_family.value(),
		indices.present_family.value()};

	swapchain_extent = chooseSwapExtent(support.capabilities);
	swapchain_image_format = format.format;
	if (support.capabilities.maxImageCount > 0
		&& image_count > support.capabilities.maxImageCount)
	{
		image_count = support.capabilities.maxImageCount;
	}
	setSwapchainCreateInfo(create_info, support, format, present_mode,
		swapchain_extent, image_count, indices, queue_indices, surface);
	if (vkCreateSwapchainKHR(device, &create_info, nullptr, &swapchain)
		!= VK_SUCCESS)
	{
		throw (Error("Scop::createSwapChain", "failed swapchain creation"));
	}
	getImagesHandles(device, swapchain, swapchain_images);
}

/**
 * Set queue create info properties.
 */
static inline void setQueueCreateInfo(
	std::vector<VkDeviceQueueCreateInfo> &create_infos,
	Scop::QueueFamilyIndices             &indices
) {
	std::set<uint32_t> uniqueQueueFamily {
		indices.graphic_family.value(),
		indices.present_family.value()
	};

	float queue_priority {1.0f};

	for (uint32_t queueFamily : uniqueQueueFamily)
	{
		VkDeviceQueueCreateInfo create_info {};

		create_info.pQueuePriorities = &queue_priority;
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		create_info.queueFamilyIndex = queueFamily;
		create_info.queueCount = 1;
		create_infos.push_back(create_info);
	}
}

/**
 * Sets the VkDeviceCreateInfo structure's informations up to create the
 * logical device.
 */
static inline void setDeviceCreateInfo(
	VkDeviceCreateInfo                     &create_info,
	std::vector<VkDeviceQueueCreateInfo>   &queue_create_info,
	VkPhysicalDeviceFeatures               &features,
	Scop::QueueFamilyIndices               &indices,
	VkPhysicalDevice                       &physical_device,
	std::vector<const char *>              &device_extensions,
	std::vector<const char *>              &validation_layers,
	bool                                   enableValidationLayers
) {
	setQueueCreateInfo(queue_create_info, indices);
	vkGetPhysicalDeviceFeatures(physical_device, &features);
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.pQueueCreateInfos = queue_create_info.data();
	create_info.queueCreateInfoCount =
		static_cast<uint32_t> (queue_create_info.size());
	create_info.pEnabledFeatures = &features;
	create_info.enabledExtensionCount =
		static_cast<uint32_t> (device_extensions.size());
	create_info.ppEnabledExtensionNames = device_extensions.data();
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
	std::vector<VkDeviceQueueCreateInfo> queue_create_info {};
	VkPhysicalDeviceFeatures features {};
	Scop::QueueFamilyIndices indices {findQueueFamilies(physical_device)};

	setDeviceCreateInfo(
		create_info,
		queue_create_info,
		features, indices,
		physical_device,
		device_extensions,
		validation_layers,
		enableValidationLayers
	);
	if (vkCreateDevice(physical_device, &create_info, nullptr, &device)
		 != VK_SUCCESS)
	{
		throw (Error("Scop::createLogicalDevice", "failed creation"));
	}
	vkGetDeviceQueue(device, indices.graphic_family.value(), 0, &graphic_queue);
	vkGetDeviceQueue(device, indices.present_family.value(), 0, &present_queue);
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
