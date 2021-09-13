# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: axbal <marvin@42.fr>                       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/09/07 11:25:48 by axbal             #+#    #+#              #
#    Updated: 2021/09/10 13:27:19 by axbal            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

EXEC = ft_ping

SRCS = main.c

SRCO = $(SRCS:.c=.o)

HEADER = includes/ping.h

FLAGS = -o2 -Wall -Wextra -Werror

LIB = ./libft/libft.a

SRC_PATH = srcs

OBJ_PATH = .obj

OBJ = $(addprefix $(OBJ_PATH)/,$(SRCO))

.PHONY: all, clean, fclean, re

all: $(EXEC)

$(EXEC) : $(LIB) $(OBJ) $(HEADER)
	gcc -o $(EXEC) $(OBJ) $(LIB)

$(LIB) :
	make -C libft

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c $(HEADER)
	@mkdir $(OBJ_PATH) 2> /dev/null || true
	gcc -c $(FLAGS) -Iincludes -o $@ -c $<

clean :
	rm -rf $(OBJ)
	@rmdir $(OBJ_PATH) 2> /dev/null || true
	make -C libft clean

fclean : clean
	rm -rf $(EXEC)
	make -C libft fclean

re : fclean all
