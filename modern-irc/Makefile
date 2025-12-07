# modern-irc Makefile

# Compiler and flags
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -g
INCLUDES = -I./src

# Target executable
NAME = modern-irc

# Source files
SRC_DIR = src
CORE_SRC = $(SRC_DIR)/core/Server.cpp \
           $(SRC_DIR)/core/Client.cpp \
           $(SRC_DIR)/core/Channel.cpp \
           $(SRC_DIR)/core/Message.cpp

NETWORK_SRC = $(SRC_DIR)/network/Socket.cpp \
              $(SRC_DIR)/network/Poller.cpp

UTILS_SRC = $(SRC_DIR)/utils/StringUtils.cpp \
            $(SRC_DIR)/utils/TopicPersistence.cpp \
            $(SRC_DIR)/utils/Logger.cpp \
            $(SRC_DIR)/utils/Config.cpp

CMD_SRC = $(SRC_DIR)/commands/AuthCommands.cpp \
          $(SRC_DIR)/commands/ChannelCommands.cpp \
          $(SRC_DIR)/commands/MessageCommands.cpp \
          $(SRC_DIR)/commands/OperatorCommands.cpp \
          $(SRC_DIR)/commands/InfoCommands.cpp \
          $(SRC_DIR)/commands/AdminCommands.cpp \
          $(SRC_DIR)/commands/ServerStatsCommands.cpp

MAIN_SRC = $(SRC_DIR)/main.cpp

SRC = $(CORE_SRC) $(NETWORK_SRC) $(UTILS_SRC) $(CMD_SRC) $(MAIN_SRC)

# Object files
OBJ_DIR = obj
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Colors for output
GREEN = \033[0;32m
BLUE = \033[0;34m
RED = \033[0;31m
NC = \033[0m

# Rules
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

run: $(NAME)
	@echo "$(GREEN)Starting IRC server on port 6667...$(NC)"
	@./$(NAME) 6667 password

test: $(NAME)
	@echo "$(GREEN)Starting IRC server on port 6667 for testing...$(NC)"
	@./$(NAME) 6667 testpass

.PHONY: all clean fclean re run test
