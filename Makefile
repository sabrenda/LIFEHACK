# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dgallard <dgallard@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/01/30 19:33:28 by dgallard          #+#    #+#              #
#    Updated: 2021/01/30 22:54:22 by dgallard         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= 	libftprintf.a

SRCS		= 	ft_parcer.c ft_printf.c ft_parce_word.c ft_c_type.c ft_s_type.c ft_di_type.c ft_ux_type.c ft_p_type.c ft_itoas.c ft_putsmbs.c

OBJS		= 	$(SRCS:.c=.o)

CFLAGS		= 	-Wall -Werror -Wextra

INCS		= 	./

.c.o:
			@gcc $(CFLAGS) -I$(INCS) -c $< -o $(<:.c=.o)


all:		$(NAME)

$(NAME): 	$(OBJS) ft_printf.h
			@ar rcs $(NAME) $(OBJS)
			@echo "--------Make libftprintf.a done successfully-----------"
			
clean:
			@rm -f $(OBJS)
			@echo "--------Removed object files---------------------------"

fclean: 	clean
			@rm -f $(NAME)
			@echo "--------Removed bin files------------------------------"

re: 		fclean all
			@echo "--------Remake libftprintf.a done successfully---------"

.PHONY:		all clean fclean re
