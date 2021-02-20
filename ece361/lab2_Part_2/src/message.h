#pragma once

#include "json.h"
#include <stdbool.h>

class Message
{
    uint64_t m_timestamp;
    std::string m_username;
    std::string m_message;

  public:
    bool private_message;
    std::string for_user;
    Message() = default;
    Message(const Message&) = default;

    // Message(uint64_t timestamp,
    //         const std::string& username,
    //         const std::string& message)
    //   : m_timestamp(timestamp)
    //   , m_username(username)
    //   , m_message(message)
    // {}

    Message(uint64_t timestamp,
            const std::string& username,
            const std::string& message,
            bool private_message_,
            std::string for_user_)
    {
        m_timestamp = timestamp;
        m_username = username;
        m_message = message;
        private_message = private_message_;
        for_user = for_user_;
    }

    ~Message() = default;

    std::string printable() const { return m_username + "> " + m_message; }

    uint64_t time() const { return m_timestamp; }

    inline bool operator<(const Message& rhs) const
    {
        return m_timestamp < rhs.m_timestamp;
    }

    inline bool operator<(uint64_t timestamp) const
    {
        return m_timestamp < timestamp;
    }

    friend void to_json(nlohmann::json& j, const Message& m);
    friend void from_json(const nlohmann::json& j, Message& m);
};

void
to_json(nlohmann::json& j, const Message& m)
{
    j = nlohmann::json{ { "timestamp", m.m_timestamp },
                        { "username", m.m_username },
                        { "message", m.m_message } };
}

void
from_json(const nlohmann::json& j, Message& m)
{
    j.at("timestamp").get_to(m.m_timestamp);
    j.at("username").get_to(m.m_username);
    j.at("message").get_to(m.m_message);
}
