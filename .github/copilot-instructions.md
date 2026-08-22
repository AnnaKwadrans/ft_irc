# ft_irc Repository Guide

## Project shape

- This is a 42 `ft_irc` server written for C++98.
- Public headers live in `includes/`; implementations belong in `src/`.
- `main.cpp` is the executable entry point and constructs `Server` with `<port>` and `<password>`.
- Keep networking responsibilities in `Server`, per-connection state in `Client`, and channel membership and permissions in `Channel`.

## Build and validation

- The intended executable is `ircserv`.
- Use `make`, `make clean`, `make fclean`, and `make re` through the repository `Makefile`.
- Compiler flags are `-Wall -Wextra -Werror -std=c++98`; preserve C++98 compatibility.
- The current Makefile does not yet pass `-Iincludes`, so the baseline `make` command fails to find `Server.hpp` from `main.cpp`. Fix the Makefile before relying on a successful build.
- Do not treat generated files such as `*.o`, `ircserv`, or `irc.dSYM/` as source changes.

## Implementation conventions

- Prefer the existing class boundaries and standard C++98 containers and APIs.
- Keep edits focused; avoid changing unrelated work already present in the working tree.
- For server behavior, add or update a small executable or integration check when practical, especially around sockets, polling, parsing, registration, and IRC command responses.