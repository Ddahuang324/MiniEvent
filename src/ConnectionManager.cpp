#include "../include/ConnectionManager.hpp"

ConnectionManager& ConnectionManager::getInstance() {
    static ConnectionManager instance;
    return instance;
}

void ConnectionManager::requestIncrement() {
    _total_request++;
}

void ConnectionManager::responseIncrement() {
    _total_response++;
}

void ConnectionManager::timeoutResponseIncrement() {
    _timeout_response++;
}

void ConnectionManager::connectionIncrement() {
    _connections++;
}

void ConnectionManager::connectionDecrement() {
    _connections--;
}

long long ConnectionManager::getTotalRequest() const {
    return _total_request.load();
}

long long ConnectionManager::getTotalResponse() const {
    return _total_response.load();
}

long long ConnectionManager::getTimeoutResponse() const {
    return _timeout_response.load();
}

unsigned int ConnectionManager::getConnections() const {
    return _connections.load();
}
