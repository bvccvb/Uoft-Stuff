#define MAXDATASIZE 1024

enum class PacketType : uint8_t
{
    LOGIN,
    LOGIN_ACK,
    LOGOUT,
    LOGOUT_ACK,
    LEAVE,
    LEAVE_ACK,
    JOIN,
    JOIN_ACK,
    CREATE,
    CREATE_ACK,
    LIST,
    LIST_ACK,
    MESSAGE,
    REGISTER,
    REGISTER_ACK,
};