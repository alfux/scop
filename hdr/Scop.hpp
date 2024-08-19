#ifndef SCOP_HPP
# define SCOP_HPP

# define SCOP_WINDOW_WIDTH 1280
# define SCOP_WINDOW_HEIGHT 720

# include <SDL2pp.hpp>
# include <cstring>
# include <optional>
# include <set>

class Scop
{
	private:
		SDL2pp sdl;

		const uint32_t width;
		const uint32_t height;

		std::vector<const char *> validation_layers;
		std::vector<const char *> device_extensions;

		VkInstance instance;
		VkDebugUtilsMessengerEXT debug_messenger;
		VkPhysicalDevice physical_device;
		VkDevice device;
		VkQueue graphic_queue;
		VkQueue present_queue;
		VkSurfaceKHR surface;
		VkSwapchainKHR swapchain;
		std::vector<VkImage> swapchain_images;
		std::vector<VkImageView> swapchain_image_view;
		VkFormat swapchain_image_format;
		VkExtent2D swapchain_extent;

		const bool enableValidationLayers;

	public:
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphic_family;
			std::optional<uint32_t> present_family;

			bool isComplete();
		};
		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> modes;
		};

		Scop(void);
		Scop(const Scop &cpy);
		virtual ~Scop(void) noexcept;
	
		Scop &operator=(const Scop &cpy);

		bool manageEvent(void);
		void initVulkan(void);
		void cleanup(void);
		void createInstance(void);
		void setupDebugMessenger(void);
		void createSurface(void);
		void checkValidationLayerSupport(void);
		void pickPhysicalDevice(void);
		bool isDeviceSuitable(const VkPhysicalDevice &device);
		QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &device);
		bool checkDeviceExtensionSupport(const VkPhysicalDevice &device);
		SwapChainSupportDetails querySwapChainSupport(
			const VkPhysicalDevice &device);
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(
			const std::vector<VkSurfaceFormatKHR> &available_formats);
		VkPresentModeKHR chooseSwapPresentMode(
			const std::vector<VkPresentModeKHR> &available_modes);
		VkExtent2D chooseSwapExtent(
			const VkSurfaceCapabilitiesKHR &capabilities);
		void createSwapChain(void);
		void createLogicalDevice(void);
		void createImageViews(void);
		void createGraphicsPipeline(void);
		void mainLoop(void);

		static VkResult createDebugUtilsMessengerEXT(
			VkInstance                               instance,
			const VkDebugUtilsMessengerCreateInfoEXT *p_create_info,
			const VkAllocationCallbacks              *p_allocator,
			VkDebugUtilsMessengerEXT                 *p_debug_messenger
		);
		static void destroyDebugUtilsMessengerEXT(
			VkInstance                  instance,
			VkDebugUtilsMessengerEXT    p_debug_messenger,
			const VkAllocationCallbacks *p_allocator
		);
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    		VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    		VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
    		const VkDebugUtilsMessengerCallbackDataEXT  *callback_data,
    		void*                                       pUserData
		);
};

#endif