# modern-irc - Phase 2 Implementation

## Overview
Phase 2 adds advanced IRC features including operator commands, channel/user modes, information commands, message history, and topic persistence.

## Implemented Features

### 1. Channel Operator Commands

#### KICK Command
- **Syntax**: `KICK <channel> <nickname> [<reason>]`
- **Description**: Remove a user from a channel (operators only)
- **Validation**:
  - User must be in channel
  - User must be channel operator
  - Target must be in channel
- **Error Codes**: ERR_NOTONCHANNEL, ERR_CHANOPRIVSNEEDED, ERR_USERNOTINCHANNEL

#### INVITE Command
- **Syntax**: `INVITE <nickname> <channel>`
- **Description**: Invite a user to a channel
- **Features**:
  - Adds user to invite list for +i channels
  - Sends invitation to target user
  - Requires operator privilege for invite-only channels
- **Error Codes**: ERR_NOSUCHNICK, ERR_NOTONCHANNEL, ERR_USERONCHANNEL, ERR_CHANOPRIVSNEEDED

#### TOPIC Command
- **Syntax**: `TOPIC <channel> [<new topic>]`
- **Description**: View or set channel topic
- **Features**:
  - Display current topic (no parameter)
  - Set new topic (requires operator if +t mode is set)
  - Topic persistence (saved to disk)
- **Error Codes**: ERR_NOTONCHANNEL, ERR_CHANOPRIVSNEEDED

### 2. MODE Command

#### Channel Modes
- **+i** (Invite-only): Only invited users can join
- **+t** (Topic protection): Only operators can change topic
- **+k <key>** (Channel key): Password required to join
- **+l <limit>** (User limit): Maximum number of users
- **+o <nick>** (Operator status): Grant/revoke operator privileges

**Syntax**: `MODE <channel> [<modes> [<parameters>]]`

**Examples**:
```
MODE #channel +i          # Make channel invite-only
MODE #channel +k secret   # Set channel password
MODE #channel +l 50       # Set user limit to 50
MODE #channel +o Alice    # Give operator to Alice
MODE #channel -t          # Allow everyone to change topic
```

#### User Modes
- **+i** (Invisible): Hide from WHO queries
- **+o** (Operator): IRC operator status (cannot be set via MODE)

**Syntax**: `MODE <nickname> [<modes>]`

**Examples**:
```
MODE Alice +i    # Make Alice invisible
MODE Alice -o    # Remove operator status
```

### 3. Information Commands

#### WHOIS Command
- **Syntax**: `WHOIS <nickname>`
- **Description**: Get detailed information about a user
- **Returns**:
  - User info (nick, username, hostname, real name)
  - Channels user is in (with @ prefix for operators)
  - Server info
  - Operator status (if applicable)
  - Away message (if away)
- **Reply Codes**: RPL_WHOISUSER, RPL_WHOISCHANNELS, RPL_WHOISSERVER, RPL_WHOISOPERATOR, RPL_AWAY, RPL_ENDOFWHOIS

#### WHO Command
- **Syntax**: `WHO <channel|mask>`
- **Description**: List users matching a pattern (usually a channel)
- **Features**:
  - Lists all members of a channel
  - Shows operator status (@) and away status (G/H)
  - Respects +i (invisible) mode
- **Reply Codes**: RPL_WHOREPLY, RPL_ENDOFWHO

### 4. Enhanced Error Handling

Added RFC error codes:
- ERR_NOSUCHSERVER (402)
- ERR_TOOMANYCHANNELS (405)
- ERR_WASNOSUCHNICK (406)
- ERR_TOOMANYTARGETS (407)
- ERR_UNKNOWNCOMMAND (421)
- ERR_NOMOTD (422)
- ERR_USERNOTINCHANNEL (441)
- ERR_USERONCHANNEL (443)
- ERR_NOTREGISTERED (451)
- ERR_YOUREBANNEDCREEP (465)
- ERR_UNKNOWNMODE (472)
- ERR_BANNEDFROMCHAN (474)
- ERR_NOPRIVILEGES (481)
- ERR_CHANOPRIVSNEEDED (482)
- ERR_UMODEUNKNOWNFLAG (501)
- ERR_USERSDONTMATCH (502)

### 5. Private Message History

**Feature**: Stores last 50 private messages per user
**Implementation**:
- Messages stored in `Client::messageHistory` (deque)
- Automatically managed (FIFO when limit exceeded)
- Format: `[from/to nickname] message`
- Accessible programmatically (can be extended for /HISTORY command)

**Technical Details**:
- Located in: `src/core/Client.hpp` and `Client.cpp`
- Max history: 50 messages per client
- Integrated with PRIVMSG command

### 6. Channel Topic Persistence

**Feature**: Topics are saved to disk and persist across server restarts

**Implementation**:
- File: `data/topics.txt`
- Format: `channelname|topic` (one per line)
- Automatically loaded when channel is created
- Automatically saved when topic is changed

**Technical Details**:
- Class: `TopicPersistence` (`src/utils/TopicPersistence.hpp`)
- Methods:
  - `saveTopic(channel, topic)`
  - `loadTopic(channel)`
  - `deleteTopic(channel)`
- Data directory created automatically with permissions 0755

## Code Architecture

### New Files Added

1. **Commands**:
   - `src/commands/OperatorCommands.hpp/cpp` - KICK, INVITE, TOPIC, MODE
   - `src/commands/InfoCommands.hpp/cpp` - WHOIS, WHO

2. **Utilities**:
   - `src/utils/TopicPersistence.hpp/cpp` - Topic persistence system

3. **Tests**:
   - `tests/integration/test_phase2.sh` - Integration tests

### Modified Files

1. **Core Classes**:
   - `src/core/Server.hpp/cpp` - Added topic persistence methods, error codes
   - `src/core/Client.hpp/cpp` - Added user modes, message history, away status
   - `src/core/Channel.hpp/cpp` - Added invite list management

2. **Commands**:
   - `src/commands/ChannelCommands.cpp` - Enhanced JOIN with invite checks and topic loading

3. **Build**:
   - `Makefile` - Added new source files

## Usage Examples

### Setting Up a Channel with Modes

```
# User1 creates channel and becomes operator
JOIN #mychannel

# Set topic protection
MODE #mychannel +t

# Set invite-only
MODE #mychannel +i

# Set user limit
MODE #mychannel +l 10

# Set channel password
MODE #mychannel +k secret123

# Set topic (only operators can do this with +t)
TOPIC #mychannel :Welcome to my channel!

# Invite another user
INVITE User2 #mychannel
```

### Managing Channel Operators

```
# Give operator status to User2
MODE #mychannel +o User2

# Remove operator status
MODE #mychannel -o User2

# Kick a user
KICK #mychannel BadUser :Breaking rules
```

### Using Information Commands

```
# Get info about a user
WHOIS Alice

# List users in a channel
WHO #mychannel

# Check your own modes
MODE Alice
```

## Testing

### Running Integration Tests

```bash
# Build the server
make re

# Run Phase 2 integration tests
./tests/integration/test_phase2.sh
```

### Manual Testing with IRC Client

```bash
# Start server
./modern-irc 6667 password

# Connect with irssi
irssi -c localhost -p 6667

# In irssi:
/QUOTE PASS password
/NICK testnick
/JOIN #test
/MODE #test +t
/TOPIC #test :Test Topic
/WHOIS testnick
```

### Testing Topic Persistence

```bash
# 1. Start server and set a topic
./modern-irc 6667 pass &
# Connect and: JOIN #test; TOPIC #test :My Topic

# 2. Stop server
killall modern-irc

# 3. Restart server
./modern-irc 6667 pass &

# 4. Rejoin channel
# Topic should still be "My Topic"
```

## Performance Considerations

1. **Message History**: Limited to 50 messages per client to prevent memory bloat
2. **Topic Persistence**: File I/O only on topic changes (not on every message)
3. **Mode Operations**: In-memory operations, no disk I/O
4. **Invite Lists**: Cleared when user joins channel

## Known Limitations

1. **Topic Persistence**: Topics stored in plain text (not encrypted)
2. **Message History**: Lost on server restart (in-memory only)
3. **Invite List**: Temporary (cleared on join or disconnect)
4. **Ban List**: Not implemented in Phase 2 (planned for Phase 3)
5. **Voice Mode (+v)**: Not implemented

## RFC Compliance

Phase 2 implements the following RFC 2812 requirements:
- ✅ MODE command (channel and user)
- ✅ KICK command
- ✅ INVITE command
- ✅ TOPIC command
- ✅ WHOIS command
- ✅ WHO command
- ✅ Channel modes: +i, +t, +k, +o, +l
- ✅ User modes: +i, +o
- ✅ Proper error code handling

## Next Steps (Phase 3)

Planned features for Phase 3:
- Server-to-server protocol
- Logging system
- SSL/TLS support
- Rate limiting / flood protection
- Admin commands (KILL, REHASH, RESTART)
- Configuration file system
- Statistics and monitoring (STATS, INFO)
- Ban lists (+b mode)
- Voice mode (+v)
