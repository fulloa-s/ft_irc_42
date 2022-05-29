# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: francesco <francesco@student.42.fr>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/11/10 16:46:02 by fulloa-s          #+#    #+#              #
#    Updated: 2022/03/15 12:50:11 by francesco        ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			=		irc

BOT				=		bot

SRCS			=		main.cpp \
						utils.cpp \
						errors.cpp \
						commands.cpp \
						utils_nick.cpp \
						utils_join.cpp \
						utils_authentication.cpp \
						utils_quit.cpp \
						utils_mode.cpp \
						utils_part.cpp \
						utils_kick.cpp \
						server.cpp \
						utils_who.cpp

SRCS_BONUS		=		./bonus/bot.cpp

CXX				=		clang++

CXXFLAGS		=		-Wall -Wextra -Werror -std=c++98

OBJCS			=		$(SRCS:.cpp=.o)

OBJCS_BONUS		=		$(SRCS_BONUS:.cpp=.o)

RM				=		rm -f

%.o				: 		%.cpp
						@$(CXX) $(CXXFLAGS) -c $<

$(NAME)			:		$(OBJCS)
						@$(CXX) -o $(NAME) $(OBJCS)

$(BOT)			:		$(OBJCS_BONUS)
						@$(CXX) -o $(BOT) bot.o

all				:		$(NAME)

bonus			:		$(NAME) $(BOT)

clean			:
						@$(RM) $(OBJCS) bot.o

fclean			:		clean
						@$(RM) $(NAME) $(BOT)

re				:		fclean all

.PHONY			:		all clean fclean re
