#ifndef _ARCHER_CALLBACK_HPP
#define _ARCHER_CALLBACK_HPP

#pragma once
#include <functional>
#include "archer/base/timestamp.hpp"

namespace archer {

using TimerCallback = std::function<void()>;

using EventCallback = std::function<void()>;

};  // namespace archer

#endif  // _ARCHER_CALLBACK_HPP
