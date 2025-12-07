#ifndef POLLER_HPP
#define POLLER_HPP

#include <poll.h>
#include <vector>
#include <map>
#include <cstddef>

class Poller {
private:
    std::vector<struct pollfd> fds;
    std::map<int, int> fdToIndex;  // Maps fd to index in fds vector

public:
    Poller();
    ~Poller();

    // Add a file descriptor to poll
    bool addFd(int fd, short events = POLLIN);

    // Remove a file descriptor from poll
    bool removeFd(int fd);

    // Poll for events (timeout in milliseconds, -1 for infinite)
    int poll(int timeout = -1);

    // Check if a file descriptor has events
    bool hasEvent(int fd, short event) const;

    // Get all file descriptors
    std::vector<int> getFds() const;

    // Get the number of file descriptors
    size_t size() const;
};

#endif
