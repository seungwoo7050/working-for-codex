#!/bin/bash
# Integration test for IRC Server Phase 2 features

GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m'

PORT=6667
PASS="testpass"
SERVER_PID=""

# Start the IRC server
start_server() {
    echo -e "${BLUE}Starting IRC server on port $PORT...${NC}"
    ./ircserv $PORT $PASS &
    SERVER_PID=$!
    sleep 2

    if ! kill -0 $SERVER_PID 2>/dev/null; then
        echo -e "${RED}Failed to start server${NC}"
        exit 1
    fi
    echo -e "${GREEN}Server started with PID $SERVER_PID${NC}"
}

# Stop the IRC server
stop_server() {
    if [ -n "$SERVER_PID" ]; then
        echo -e "${BLUE}Stopping IRC server...${NC}"
        kill $SERVER_PID 2>/dev/null
        wait $SERVER_PID 2>/dev/null
        echo -e "${GREEN}Server stopped${NC}"
    fi
}

# Test function using netcat
test_command() {
    local test_name=$1
    local commands=$2
    local expected=$3

    echo -e "${BLUE}Testing: $test_name${NC}"

    response=$(echo -e "$commands" | nc -q 1 localhost $PORT 2>/dev/null)

    if echo "$response" | grep -q "$expected"; then
        echo -e "${GREEN}✓ PASS: $test_name${NC}"
        return 0
    else
        echo -e "${RED}✗ FAIL: $test_name${NC}"
        echo "Expected pattern: $expected"
        echo "Got response: $response"
        return 1
    fi
}

# Cleanup on exit
trap stop_server EXIT INT TERM

# Build the server
echo -e "${BLUE}Building IRC server...${NC}"
make re > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed${NC}"
    exit 1
fi
echo -e "${GREEN}Build successful${NC}"

# Start server
start_server

echo ""
echo -e "${BLUE}=== Running Phase 2 Integration Tests ===${NC}"
echo ""

# Test 1: Basic connection and authentication
test_command "Basic authentication" \
    "PASS $PASS\r\nNICK testuser\r\nUSER test 0 * :Test User\r\n" \
    "001"

# Test 2: MODE command - channel modes
test_command "Channel MODE command" \
    "PASS $PASS\r\nNICK modetest\r\nUSER test 0 * :Test\r\nJOIN #testchan\r\nMODE #testchan\r\n" \
    "324"

# Test 3: TOPIC command
test_command "TOPIC command" \
    "PASS $PASS\r\nNICK topictest\r\nUSER test 0 * :Test\r\nJOIN #topic\r\nTOPIC #topic :Test Topic\r\n" \
    "TOPIC"

# Test 4: WHOIS command
test_command "WHOIS command" \
    "PASS $PASS\r\nNICK whoistest\r\nUSER test 0 * :Test\r\nWHOIS whoistest\r\n" \
    "311"

# Test 5: WHO command
test_command "WHO command" \
    "PASS $PASS\r\nNICK whotest\r\nUSER test 0 * :Test\r\nJOIN #who\r\nWHO #who\r\n" \
    "352"

# Note: KICK, INVITE, and more complex MODE tests require multiple concurrent connections
# These would be better tested with a proper IRC client or more sophisticated test framework

echo ""
echo -e "${BLUE}=== Test Summary ===${NC}"
echo -e "${GREEN}Phase 2 basic integration tests completed${NC}"
echo ""
echo "Note: For comprehensive testing, please use an IRC client (irssi, WeeChat, hexchat)"
echo "and test the following features:"
echo "  - KICK command (requires 2+ users in a channel)"
echo "  - INVITE command (requires 2+ users)"
echo "  - MODE +i, +t, +k, +o, +l (various channel modes)"
echo "  - User modes (+i, +o)"
echo "  - Private message history"
echo "  - Channel topic persistence (restart server and check)"
echo ""
