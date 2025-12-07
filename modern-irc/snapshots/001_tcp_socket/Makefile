# Snapshot 001: TCP Socket Server
# 기본 TCP 소켓 서버 구현

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -g
INCLUDES = -I./src

NAME = ircserv

SRC_DIR = src
SRC = $(SRC_DIR)/main.cpp \
      $(SRC_DIR)/network/Socket.cpp

OBJ_DIR = obj
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

GREEN = \033[0;32m
BLUE = \033[0;34m
RED = \033[0;31m
NC = \033[0m

all: $(NAME)

$(NAME): $(OBJ)
	@echo "$(BLUE)Linking $(NAME)...$(NC)"
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	@echo "$(GREEN)Build complete: $(NAME)$(NC)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "$(BLUE)Compiling $<...$(NC)"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "$(RED)Removing object files...$(NC)"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "$(RED)Removing $(NAME)...$(NC)"
	@rm -f $(NAME)

re: fclean all

test: $(NAME)
	@echo "$(GREEN)Starting server on port 6667...$(NC)"
	@./$(NAME) 6667 testpass

.PHONY: all clean fclean re test
