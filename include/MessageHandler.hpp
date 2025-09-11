#pragma once 

class AbstractServer;

class MessageHandler {
public:

    MessageHandler(void) = default;
    ~MessageHandler(void)= default;

    virtual void handleMessage(void* arg) = 0  ;

};

