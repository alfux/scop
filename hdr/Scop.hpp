#ifndef SCOP_HPP
# define SCOP_HPP

# define SCOP_WINDOW_WIDTH 1280
# define SCOP_WINDOW_HEIGHT 720

# include <SDL2pp.hpp>
# include <cstring>
# include <optional>

class Scop
{
	SDL2pp sdl;

	const uint32_t width;
	const uint32_t height;

	std::vector<const char *> validation_layers;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debug_messenger;
	VkPhysicalDevice physical_device;

	const bool enableValidationLayers;

	public:
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphics_family;

			bool isComplete();
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
		void checkValidationLayerSupport(void);
		void pickPhysicalDevice(void);
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