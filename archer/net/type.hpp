#ifndef _ARCHER_CALLBACK_HPP
#define _ARCHER_CALLBACK_HPP

#include <functional>
#include <memory>
#include "archer/base/socket.hpp"
#include "archer/base/timestamp.hpp"

namespace archer {

using TimerCallback = std::function<void()>;

using EventCallback = std::function<void()>;

using SocketPtr = std::shared_ptr<Socket>;

using Functor = std::function<void()>;


};  // namespace archer

#endif  // _ARCHER_CALLBACK_HPP