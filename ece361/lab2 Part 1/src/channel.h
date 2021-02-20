#pragma once

#include <chrono>
#include <tuple>
#include <vector>

#include "message.h"

class Channel
{
    std::vector<Message> m_messages;

  public:
    Channel() = default;
    Channel(const Channel&) = delete;
    ~Channel() = default;

    void post(const std::string& username, const std::string& message)
    {
        uint64_t ms(std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count());

        m_messages.emplace_back(Message(ms, username, message));
    }

    std::vector<Message> get(uint64_t timestamp)
    {
        auto lower = std::lower_bound(
          std::begin(m_messages), std::end(m_messages), timestamp);

        return std::vector<Message>(lower,
                                    std::min(lower + 5, std::end(m_messages)));
    }
};
