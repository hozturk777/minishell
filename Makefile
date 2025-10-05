MAKEFLAGS			+= --no-print-directory
NAME				= minishell
EXEC_DIR			= source/executer/
EXPA_DIR			= source/expander/
PAR_DIR				= source/parser/
LXR_DIR				= source/lexer/
BUILD_DIR			= source/builtins/
REDIRECT_DIR		= source/redirection/
ENV_DIR				= source/environment/
SIG_DIR				= source/signal/
UTL_DIR				= source/utils/
SRC_DIR				= source/core/
LIBFT_DIR			= include/libft/
LRDR				= -lreadline
CC					= @cc
CFLAGS				= -Wall -Wextra -Werror -g
RM					= rm -rf
OBJ_DIR				= objects/

MINISHELL_SRCS		= $(SRC_DIR)main.c \
					  $(SRC_DIR)print_welcome.c \
					  $(LXR_DIR)lexer.c \
					  $(LXR_DIR)tokenizer.c \
					  $(LXR_DIR)token_handlers.c \
					  $(LXR_DIR)token_handlers_advanced.c \
					  $(UTL_DIR)global_state.c \
					  $(ENV_DIR)environment.c \
					  $(ENV_DIR)env_utils.c \
					  $(ENV_DIR)env_utils2.c \
					  $(ENV_DIR)path_utils.c \
					  $(UTL_DIR)debug.c \
					  $(UTL_DIR)debug_utils.c \
					  $(EXPA_DIR)variable_expansion.c \
					  $(EXPA_DIR)quote_expansion.c \
					  $(EXPA_DIR)expansion_utils.c \
					  $(EXPA_DIR)argument_processor.c \
					  $(EXPA_DIR)expand_utils.c \
					  $(SIG_DIR)signal_handler.c \
					  $(SIG_DIR)sigint.c \
					  $(BUILD_DIR)builtins.c \
					  $(BUILD_DIR)builtins_cd.c \
					  $(BUILD_DIR)path_built.c \
					  $(BUILD_DIR)builtin_export.c \
					  $(BUILD_DIR)builtin_export_utils.c \
					  $(BUILD_DIR)echo_builtin.c \
					  $(BUILD_DIR)builtins_utils.c \
					  $(BUILD_DIR)builtins_utils2.c \
					  $(EXEC_DIR)executor.c \
					  $(EXEC_DIR)execute_external.c \
					  $(EXEC_DIR)executor_utils.c \
					  $(EXEC_DIR)executer_utils2.c \
					  $(EXEC_DIR)executer_utils3.c \
					  $(EXEC_DIR)pipeline_execution.c \
					  $(REDIRECT_DIR)redirections.c \
					  $(REDIRECT_DIR)redirections2.c \
					  $(REDIRECT_DIR)redirections_utils.c \
					  $(PAR_DIR)parser.c \
					  $(PAR_DIR)parser_utils.c \
					  $(PAR_DIR)command_utils.c \
					  $(PAR_DIR)parser_is.c \

SRCS				= $(MINISHELL_SRCS)
OBJ					= $(addprefix $(OBJ_DIR), $(notdir $(SRCS:.c=.o)))

COLOR_YELLOW		= \033[0;33m \033[1m
COLOR_GREEN			= \033[0;32m \033[1m
COLOR_RED			= \033[0;31m \033[1m
COLOR_END			= \033[0m

all:				libft $(NAME)

libft:
					@make -C $(LIBFT_DIR)

$(NAME):			$(OBJ)
					@$(CC) $(CFLAGS) $(OBJ) $(LIBFT_DIR)libft.a $(LRDR) -o $(NAME)
					@echo "\n\e[1m$(COLOR_YELLOW)$(NAME)	$(COLOR_GREEN)[is ready!]\e[0m\n$(COLOR_END)"

VPATH = $(SRC_DIR):$(UTL_DIR):$(LXR_DIR):$(EXEC_DIR)::$(EXPA_DIR):$(PAR_DIR):$(BUILD_DIR):$(REDIRECT_DIR):$(ENV_DIR):$(SIG_DIR):$(UTL_DIR)

$(OBJ_DIR)%.o: %.c
					@mkdir -p $(OBJ_DIR)
					@$(CC) $(CFLAGS) -c $< -o $@

#$(OBJ_DIR)%.o: $(UTL_DIR)%.c
#					@mkdir -p $(OBJ_DIR)
#					@$(CC) $(CFLAGS) -c $< -o $@

#$(OBJ_DIR)%.o: loop_dir/%.c
#					@mkdir -p $(OBJ_DIR)
#					@$(CC) $(CFLAGS) -c $< -o $@

#$(OBJ_DIR)%.o: $(EXEC_DIR)%.c
#					@mkdir -p $(OBJ_DIR)
#					@$(CC) $(CFLAGS) -c $< -o $@

clean:
					@$(RM) $(OBJ_DIR)
					@make -C $(LIBFT_DIR) clean
					@echo "\n\e[1m$(COLOR_YELLOW)objects	$(COLOR_RED)[are deleted!]\e[0m$(COLOR_END)"

fclean:				clean
					@$(RM) $(NAME)
					@make -C $(LIBFT_DIR) fclean
					@echo "\e[1m$(COLOR_YELLOW)$(NAME)	$(COLOR_RED)[is deleted!]\e[0m\n$(COLOR_END)"

re:					fclean all

.PHONY:				all clean fclean re libft