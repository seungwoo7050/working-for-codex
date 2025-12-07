# Snapshot 016: v3.0 Admin Commands

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -g
INCLUDES = -I./src

NAME = ircserv

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
          $(SRC_DIR)/commands/AdminCommands.cpp

MAIN_SRC = $(SRC_DIR)/main.cpp

SRC = $(CORE_SRC) $(NETWORK_SRC) $(UTILS_SRC) $(CMD_SRC) $(MAIN_SRC)

OBJ_DIR = obj
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	@echo "Build complete: $(NAME) (v3.0 admin)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
