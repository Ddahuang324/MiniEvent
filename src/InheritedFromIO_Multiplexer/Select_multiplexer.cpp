#include "../../include/InheritedFromIO_Multiplexer/Select_multiplexer.hpp"
#include "../../include/Channel.hpp"
#include <algorithm>
#include <cstdio>
#include <iostream>



SelectMultiplexer::SelectMultiplexer(): max_fd_(-1){
    FD_ZERO(&read_set_);
    FD_ZERO(&write_set_);
    FD_ZERO(&error_set_);
    std::cout << "SelectMultiplexer created" << std::endl;
}

SelectMultiplexer::~SelectMultiplexer() {
    std::cout << "SelectMultiplexer destroyed" << std::endl;
}

void SelectMultiplexer::addChannel(Channel* channel) {
    int fd = channel->fd();
    channels_[fd] = channel;
    if (fd > max_fd_) {
        max_fd_ = fd;
    }
    if (channel->isReading()) {
        FD_SET(fd, &read_set_);
    }
    if (channel->isWriting()) {
        FD_SET(fd, &write_set_);
    }

    if (fd > max_fd_) {
        max_fd_ = fd;
    }

}

void SelectMultiplexer::removeChannel(Channel* channel) {
    int fd = channel->fd();

    FD_CLR (fd, &read_set_);
    FD_CLR (fd, &write_set_);
    FD_CLR (fd, &error_set_);

    channels_.erase(fd);

    if (fd == max_fd_) {
        max_fd_ = -1;
        if(!channels_.empty()){
            max_fd_ = channels_.rbegin()->first;
        }
    }
}


void SelectMultiplexer::updateChannel(Channel* channel) {
    int fd = channel->fd();

    FD_CLR (fd, &read_set_);
    FD_CLR (fd, &write_set_);

    if (channel->isReading()) {
        FD_SET(fd, &read_set_);
    }
    if (channel->isWriting()) {
        FD_SET(fd, &write_set_);
    }
}

int SelectMultiplexer::dispatch(int timeout_ms, std::vector<Channel*>& active_channels) {
    fd_set read_fds = read_set_;
    fd_set write_fds = write_set_;
    fd_set error_fds = error_set_;

    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;//秒
    timeout.tv_usec = (timeout_ms % 1000) * 1000;//微秒

    int nfds = max_fd_ + 1;

    int num_events = select(nfds, &read_fds, &write_fds, &error_fds, &timeout);

    if( num_events < 0 ){
        perror("select error");
        return -1;
    }else if( num_events == 0 ){
        return 0;
    }

    for(int fd = 0 ; fd < max_fd_ ; ++fd){
        bool is_active = false;
        int active_events = 0;

        if(FD_ISSET(fd, &read_fds)){
            active_events |= Channel::kReadEvent;
            is_active = true;
        } 
        if(FD_ISSET(fd, &write_fds)){
            active_events |= Channel::kWriteEvent;
            is_active = true;
        }
        if(FD_ISSET(fd, &error_fds)){
            active_events |= Channel::kErrorEvent;
            is_active = true;
        }

        if(is_active){
            auto it = channels_.find(fd);
            if (it != channels_.end()) {
                Channel* channel = it->second;
                channel->ReadyEvents(active_events);
                // 将通道添加到活动通道列表中
                active_channels.push_back(channel);
            }
        }
    }
    return num_events;
}





