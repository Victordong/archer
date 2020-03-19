load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "net",
    srcs = [
        "eventloop/channel.cpp",
        "eventloop/eventloop.cpp",
        "poller/epoll_poller.cpp",
        "poller/poller.cpp",
        "tests/eventloop_test.cpp",
    ],
    hdrs = [
        "eventloop/channel.hpp",
        "eventloop/eventloop.hpp",
        "poller/epoll_poller.hpp",
        "poller/poller.hpp",
    ],
    deps = [
        "//archer/base",
    ],
)

cc_binary(
    name = "event_test",
    srcs = [
        "tests/eventloop_test.cpp",
    ],
    deps = [
        ":net",
    ],
)
