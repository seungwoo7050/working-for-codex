#!/bin/bash
# Basic IRC server test script

echo "Testing IRC server basic functionality..."
echo ""

# Test connection and authentication
{
    sleep 0.5
    echo "PASS testpass"
    sleep 0.5
    echo "NICK alice"
    sleep 0.5
    echo "USER alice 0 * :Alice Wonderland"
    sleep 0.5
    echo "JOIN #test"
    sleep 0.5
    echo "PRIVMSG #test :Hello from test script!"
    sleep 0.5
    echo "LIST"
    sleep 0.5
    echo "NAMES #test"
    sleep 0.5
    echo "QUIT :Test complete"
    sleep 0.5
} | nc localhost 6667

echo ""
echo "Test complete!"
