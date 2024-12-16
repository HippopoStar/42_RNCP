
#ifndef FT_LOG_H
# define FT_LOG_H

/*
** https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-format-function-attribute
** https://gcc.gnu.org/onlinedocs/gcc/Variadic-Macros.html
** https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
** https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html
** https://www.gnu.org/software/libc/manual/html_node/Variable-Arguments-Output.html
*/

# ifdef DEBUG
#  define FT_LOG_DEBUG(format, ...) \
          ft_log_debug("%s:%d: " format "\n", __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
# else
#  define FT_LOG_DEBUG(...) \
          /* do {} while(0) */
# endif /* DEBUG */

# define FT_LOG_INFO(format, ...) \
         ft_log_info(format "\n" __VA_OPT__(,) __VA_ARGS__)

# define FT_LOG_ERROR(format, ...) \
         ft_log_error("%s: " format "\n", program_name __VA_OPT__(,) __VA_ARGS__)

/* String containing name the program is called with. */
extern const char *program_name;

void
ft_log_init(const char *argv0);

int
ft_log_debug(const char *format, ...)
	__attribute__ ((format (printf, 1, 2)));

int
ft_log_info(const char *format, ...)
	__attribute__ ((format (printf, 1, 2)));

int
ft_log_error(const char *format, ...)
	__attribute__ ((format (printf, 1, 2)));

#endif /* FT_LOG_H */
