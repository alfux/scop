#include <main.hpp>

int	main(void)
{
	try
	{
		SDL2pp	instance;

		instance.main();
		return (0);
	}
	catch (std::exception const &e)
	{
		Error::print(e);
		return (1);
	}
}
