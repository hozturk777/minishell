MAKEFLAGS 			+= --no-print-directory
NAME				= minishell
EXEC_DIR			= executer/
EXPA_DIR			= expander/
LEX_DIR				= lexer/
PAR_DIR				= parser/
SRC_DIR				= source/
UTL_DIR				= utils/
LRDR				= -lreadline
CC					= @cc
CFLAGS				= -Wall -Wextra -Werror -pthread
RM					= rm -rf

MINISHELL_SRCS			=	$(SRC_DIR)main.c \
								$(UTL_DIR)starting.c \

SRCS 				= $(MINISHELL_SRCS)
OBJ					= $(addprefix $(OBJ_DIR), $(notdir $(SRCS:.c=.o)))

COLOR_YELLOW	=	\033[0;33m \033[1m
COLOR_GREEN		=	\033[0;32m \033[1m
COLOR_RED		=	\033[0;31m \033[1m
COLOR_END		=	\033[0m
all: 				$(NAME)

$(NAME): 			$(OBJ)
					@$(CC) $(CFLAGS) $(OBJ) $(LRDR) -o $(NAME)
					@echo "\n\e[1m$(COLOR_YELLOW)$(NAME)	$(COLOR_GREEN)[is ready!]\e[0m\n$(COLOR_END)"

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
					# @mkdir -p $(OBJ_DIR)
					@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)%.o: $(UTL_DIR)%.c
					# @mkdir -p $(OBJ_DIR)
					@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)%.o: $(ERR_DIR)%.c
					# @mkdir -p $(OBJ_DIR)
					@$(CC) $(CFLAGS) -c $< -o $@

clean:
					@$(RM) $(OBJ_DIR)
					@echo "\n\e[1m$(COLOR_YELLOW)objects	$(COLOR_RED)[are deleted!]\e[0m$(COLOR_END)"

fclean: 			clean
					@$(RM) $(NAME)
					@echo "\e[1m$(COLOR_YELLOW)$(NAME)	$(COLOR_RED)[is deleted!]\e[0m\n$(COLOR_END)"

re: 				fclean all

.PHONY: 			all clean fclean re