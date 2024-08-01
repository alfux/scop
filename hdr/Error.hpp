#ifndef ERROR_HPP
# define ERROR_HPP
# include <exception>
# include <iostream>
# include <string>


class Error: public std::exception
{
	char const *wht;
	char const *whr;

	public:
		Error(void);
		Error(Error const &cpy);
		Error(char const *whr);
		Error(char const *whr, char const *wht);
		virtual ~Error(void) noexcept;

		Error &operator=(Error const &cpy);

		char const *what(void) const throw();
		char const *where(void) const throw();

		static void print(std::exception const &e);
};


#endif
