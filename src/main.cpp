#include <main.hpp>

int main(void)
{
	try
	{
		Scop scop;

		scop.mainLoop();
		return (0);
	}
	catch (std::exception const &e)
	{
		Error::print(e);
		return (1);
	}
}
