
CFLAGS   = -Wall -Wextra -Werror
CPPFLAGS = -I./include/

HEAD = \
	./include/ft_ping.h

CC = gcc

NAME = ft_ping

SRC_PATH = ./src

SRC_NAME = \
	ft_ping.c

SRC = $(addprefix $(SRC_PATH)/,$(SRC_NAME))

OBJ_PATH = ./obj

OBJ_NAME = $(SRC_NAME:.c=.o)

OBJ = $(addprefix $(OBJ_PATH)/,$(OBJ_NAME))



all: $(NAME)

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(LDFLAGS) $(OBJ)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c $(HEAD) | $(OBJ_PATH)
	$(CC) -c -o $@ $(CFLAGS) $(CPPFLAGS) $<

$(OBJ_PATH):
	mkdir $(OBJ_PATH)

clean:
	rm -f $(OBJ)
	rm -df $(OBJ_PATH)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
