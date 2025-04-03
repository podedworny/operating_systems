#ifndef MESSAGE_H
#define MESSAGE_H


struct Message {
    char queue_name[64];
    int number;
    char message[64];
};

#endif // MESSAGE_H