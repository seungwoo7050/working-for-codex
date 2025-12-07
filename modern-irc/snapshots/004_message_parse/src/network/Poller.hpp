#ifndef POLLER_HPP
#define POLLER_HPP

#include <poll.h>
#include <vector>
#include <map>
#include <cstddef>

class Poller {
private:
    std::vector<struct pollfd> fds;
    std::map<int, int> fdToIndex;

public:
    Poller();
    ~Poller();

    bool addFd(int fd, short events = POLLIN);
    bool removeFd(int fd);
    int poll(int timeout = -1);
    bool hasEvent(int fd, short event) const;
    std::vector<int> getFds() const;
    size_t size() const;
};

#endif
