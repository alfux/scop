#include <Error.hpp>


Error::Error(void): wht(strerror(errno)), whr("") {}


Error::Error(Error const &cpy): wht(cpy.wht), whr(cpy.whr) {}


Error::Error(char const *whr): wht(strerror(errno)), whr(whr) {}


Error::Error(char const *whr, char const *wht): wht(wht), whr(whr) {}


Error::~Error(void) throw() {}


Error	&Error::operator=(Error const &cpy)
{
	this->wht = cpy.wht;
	this->whr = cpy.whr;
	return (*this);
}


char const	*Error::what(void) const throw()
{
	return (this->wht);
}


char const	*Error::where(void) const throw()
{
	return (this->whr);
}


void	Error::print(std::exception const &e)
{
	try
	{
		std::cerr << "Error: " << (dynamic_cast<Error const &> (e)).where() << ": ";
	}
	catch (...) {}
	std::cerr << e.what() << std::endl;
}
