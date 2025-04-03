#define MAX_CLIENTS 10
#define MAX_NAME 32

typedef enum{
    LIST,
    TO_ALL,
    TO_ONE,
    STOP,
    ALIVE,
    MESSAGE,
    CONNECT,
    ERROR
    
} MessageType;

struct message {
    MessageType type;
    char message[256];
    char id[MAX_NAME];
    char list[MAX_CLIENTS][MAX_NAME];
    int no_of_clients;
    time_t timestamp;
};

struct arr{
    int place;
    int fd;
};