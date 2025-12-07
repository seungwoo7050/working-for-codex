#include "Poller.hpp"
#include <iostream>
#include <cerrno>
#include <cstring>

Poller::Poller() {
}

Poller::~Poller() {
}

bool Poller::addFd(int fd, short events) {
    if (fdToIndex.find(fd) != fdToIndex.end()) {
        return false;  // Already exists
    }

    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = events;
    pfd.revents = 0;

    fds.push_back(pfd);
    fdToIndex[fd] = fds.size() - 1;
    return true;
}

bool Poller::removeFd(int fd) {
    auto it = fdToIndex.find(fd);
    if (it == fdToIndex.end()) {
        return false;  // Not found
    }

    int index = it->second;

    // Remove from fds vector by swapping with last element
    if (index < static_cast<int>(fds.size()) - 1) {
        fds[index] = fds.back();
        fdToIndex[fds[index].fd] = index;
    }
    fds.pop_back();
    fdToIndex.erase(it);

    return true;
}

int Poller::poll(int timeout) {
    if (fds.empty()) {
        return 0;
    }

    int ret = ::poll(&fds[0], fds.size(), timeout);
    if (ret < 0) {
        if (errno != EINTR) {
            std::cerr << "Error in poll: " << strerror(errno) << std::endl;
        }
        return -1;
    }
    return ret;
}

bool Poller::hasEvent(int fd, short event) const {
    std::map<int, int>::const_iterator it = fdToIndex.find(fd);
    if (it == fdToIndex.end()) {
        return false;
    }

    int index = it->second;
    return (fds[index].revents & event) != 0;
}

std::vector<int> Poller::getFds() const {
    std::vector<int> result;
    for (size_t i = 0; i < fds.size(); ++i) {
        result.push_back(fds[i].fd);
    }
    return result;
}

size_t Poller::size() const {
    return fds.size();
}
