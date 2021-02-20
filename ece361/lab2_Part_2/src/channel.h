#pragma once

#include <chrono>
#include <tuple>
#include <vector>

#include "message.h"

#define T_NOW                                                                  \
    std::chrono::duration_cast<std::chrono::milliseconds>(                     \
      std::chrono::system_clock::now().time_since_epoch())                     \
      .count()

class Channel
{
    class ChannelUser
    {
        std::string m_username;
        uint64_t m_join_timestamp;

      public:
        ChannelUser(const std::string& username)
          : m_username(username)
          , m_join_timestamp(T_NOW)
        {}

        ChannelUser(const ChannelUser&) = default;
        ~ChannelUser() = default;

        const std::string& username() const
        {
            return m_username;
        }

        const uint64_t& join_timestamp() const
        {
            return m_join_timestamp;
        }

        inline bool operator<(uint64_t timestamp) const
        {
            return m_join_timestamp < timestamp;
        }

        inline bool operator==(uint64_t timestamp) const
        {
            return m_join_timestamp == timestamp;
        }

        inline bool operator<(const std::string& username) const
        {
            return m_username < username;
        }

        inline bool operator==(const std::string& username) const
        {
            return m_username == username;
        }
    };

    std::string m_channel_name;
    std::vector<Message> m_messages;
    std::vector<Message> p_messages;
    std::vector<ChannelUser> m_users;
    ChannelUser* m_op;

  public:
    Channel()
    {
        abort();
    }

    Channel(const std::string& channel_name, const std::string& op)
      : m_channel_name(channel_name)
    {
        m_users.emplace_back(ChannelUser(op));
        m_op = &m_users[0];
    }

    Channel(const Channel&) = default;
    ~Channel() = default;

    void subscribe(const std::string& username)
    {
        m_users.emplace_back(ChannelUser(username));
    }

    void unsubscribe(const std::string& username)
    {
        m_users.erase(
          std::remove(std::begin(m_users), std::end(m_users), username),
          std::end(m_users));
    }

    std::string name()
    {
        return m_channel_name;
    }

    void post_msg(const std::string& username, const std::string& message)
    {
        uint64_t ms(T_NOW);

        m_messages.emplace_back(Message(ms, username, message, false, NULL));
    }

    void post_private_msg(const std::string& username, const std::string& message, const std::string for_user)
    {
        uint64_t ms(T_NOW);

        p_messages.emplace_back(Message(ms, username, message, true, for_user));
    }

    std::vector<Message> get_msg(uint64_t timestamp, std::string username)
    {
        std::vector<Message> ret;
        auto lower = std::lower_bound(
          std::begin(m_messages), std::end(m_messages), timestamp);

        ret = std::vector<Message>(lower,
                                    std::min(lower + 5, std::end(m_messages)));

        for(auto it = p_messages.begin() ; it != p_messages.end(); it++){
            if(it-> for_user == username){
                ret.push_back(*it);
            }
        }

        return ret;
    }

    size_t n_users()
    {
        return m_users.size();
    }

    bool is_user(const std::string& username)
    {
        return std::find(std::begin(m_users), std::end(m_users), username) !=
               std::end(m_users);
    }

    std::string get_op()
    {
        return m_op->username();
    }

    void set_op(const std::string& op)
    {
        auto it = std::find(std::begin(m_users), std::end(m_users), op);
        if (it == std::end(m_users))
            return;

        m_op = &(*it);
    }
};

#undef T_NOW
