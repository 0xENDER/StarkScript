/**
 *
 *  Manage all the console strings
 *
**/


// Define the console colours
// For more info about ANSI escape sequences, visit:
// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
#define CONSOLE_COLOR_RESET "\033[0m"
#define CONSOLE_COLOR_BLACK "\033[0;30m"
#define CONSOLE_COLOR_RED "\033[0;31m"
#define CONSOLE_COLOR_GREEN "\033[0;32m"
#define CONSOLE_COLOR_YELLOW "\033[0;33m"
#define CONSOLE_COLOR_BLUE "\033[0;34m"
#define CONSOLE_COLOR_PURPLE "\033[0;35m"
#define CONSOLE_COLOR_CYAN "\033[0;36m"
#define CONSOLE_COLOR_WHITE "\033[0;37m"
#define CONSOLE_COLOR_GRAY "\033[38;5;240m"
#define CONSOLE_COLOR_DARK_GRAY "\033[38;5;236m"

// Define the prefix of debug console messages
#define STRING_CONSOLE_DEBUG_MESSAGE "[\033[0;36m*\033[0m][\033[0;36mDebug\033[0m] -> "

// Define the prefix of informative console messages
#define STRING_CONSOLE_INFO_MESSAGE "[\033[0;32m?\033[0m][\033[0;32mInfo\033[0m] -> "

// Define the prefix of warning console messages
#define STRING_CONSOLE_WARNING_MESSAGE "[\033[0;33m!\033[0m][\033[0;33mWarn\033[0m] -> "

// Define the prefix of error console messages
#define STRING_CONSOLE_ERROR_MESSAGE "[\033[0;31mx\033[0m][\033[0;31mError\033[0m] -> "

// Define the GitHub issue suggestion string
#define STRING_CONSOLE_GITHUB_REPORT "\033[0;36m\n\n            * Please, open an issue on our GitHub repository about this error! (visit https://github.com/EnderCommunity/StarkScript/issues/)\n\033[0m"
