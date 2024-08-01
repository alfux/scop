#include <Error.hpp>


Error::Error(void) : wht(strerror(errno)), whr("")
{
	// Empty;
}


Error::Error(Error const &cpy) : wht(cpy.wht), whr(cpy.whr)
{
	// Empty;
}


Error::Error(char const *whr) : wht(strerror(errno)), whr(whr)
{
	// Empty;
}


Error::Error(char const *whr, char const *wht) : wht(wht), whr(whr)
{
	// Empty;
}


Error::~Error(void) noexcept
{
	// Empty;
}


Error &Error::operator=(Error const &cpy)
{
	this->wht = cpy.wht;
	this->whr = cpy.whr;
	return (*this);
}


char const *Error::what(void) const throw()
{
	return (this->wht);
}


char const *Error::where(void) const throw()
{
	return (this->whr);
}


void Error::print(std::exception const &e)
{
	try
	{
		std::cerr << "Error: " << (dynamic_cast<Error const &> (e)).where() << ": ";
	}
	catch (...)
	{
		// Empty;
	}
	std::cerr << e.what() << std::endl;
}
