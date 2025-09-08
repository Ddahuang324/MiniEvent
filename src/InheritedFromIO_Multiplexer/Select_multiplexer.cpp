#include "../include/InheritedFromIO_Multiplexer/Select_multiplexer.hpp"
#include <algorithm>
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
    int fd = channel->getFd();
    

}


