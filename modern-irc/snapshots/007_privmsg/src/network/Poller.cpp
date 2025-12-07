#include "Poller.hpp"
#include <iostream>
#include <cerrno>
#include <cstring>

Poller::Poller() {}
Poller::~Poller() {}

bool Poller::addFd(int fd, short events) {
    if (fdToIndex.find(fd) != fdToIndex.end()) return false;
    struct pollfd pfd = {fd, events, 0};
    fds.push_back(pfd);
    fdToIndex[fd] = fds.size() - 1;
    return true;
}

bool Poller::removeFd(int fd) {
    auto it = fdToIndex.find(fd);
    if (it == fdToIndex.end()) return false;
    int index = it->second;
    if (index < static_cast<int>(fds.size()) - 1) {
        fds[index] = fds.back();
        fdToIndex[fds[index].fd] = index;
    }
    fds.pop_back();
    fdToIndex.erase(it);
    return true;
}

int Poller::poll(int timeout) {
    if (fds.empty()) return 0;
    int ret = ::poll(&fds[0], fds.size(), timeout);
    if (ret < 0 && errno != EINTR) {
        std::cerr << "Poll error: " << strerror(errno) << std::endl;
    }
    return ret;
}

bool Poller::hasEvent(int fd, short event) const {
    auto it = fdToIndex.find(fd);
    if (it == fdToIndex.end()) return false;
    return (fds[it->second].revents & event) != 0;
}

std::vector<int> Poller::getFds() const {
    std::vector<int> result;
    for (const auto& pfd : fds) result.push_back(pfd.fd);
    return result;
}

size_t Poller::size() const { return fds.size(); }
