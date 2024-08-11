#ifndef SCOP_HPP
# define SCOP_HPP

# define SCOP_WINDOW_WIDTH 1280
# define SCOP_WINDOW_HEIGHT 720

# include <SDL2pp.hpp>
# include <cstring>

class Scop
{
	SDL2pp sdl;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debug_messenger;

	const uint32_t width;
	const uint32_t height;

	std::vector<const char *> validationLayers;

	const bool enableValidationLayers;

	public:
		Scop(void);
		Scop(const Scop &cpy);
		virtual ~Scop(void) noexcept;
	
		Scop &operator=(const Scop &cpy);

		bool manageEvent(void);
		void initVulkan(void);
		void createInstance(void);
		void checkValidationLayerSupport();
		void mainLoop(void);

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    		VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    		VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
    		const VkDebugUtilsMessengerCallbackDataEXT  *callback_data,
    		void*                                       pUserData
		);
};

#endif