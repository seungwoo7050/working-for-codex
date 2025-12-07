# Snapshot 002: poll() Event Loop

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -g
INCLUDES = -I./src

NAME = ircserv

SRC_DIR = src
SRC = $(SRC_DIR)/main.cpp \
      $(SRC_DIR)/network/Socket.cpp \
      $(SRC_DIR)/network/Poller.cpp

OBJ_DIR = obj
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	@echo "Build complete: $(NAME)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

test: $(NAME)
	./$(NAME) 6667 testpass

.PHONY: all clean fclean re test
