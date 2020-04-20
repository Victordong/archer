load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "net",
    srcs = [
        "acceptor.cpp",
        "codec.cpp",
        "eventloop/channel.cpp",
        "eventloop/eventloop.cpp",
        "poller/epoll_poller.cpp",
        "poller/poller.cpp",
        "tcp_conn.cpp",
        "tcp_server.cpp",
        "timer/idle.cpp",
        "timer/timer.cpp",
        "timer/timer_queue.cpp",
    ],
    hdrs = [
        "acceptor.hpp",
        "archer_imp.hpp",
        "codec.hpp",
        "eventloop/channel.hpp",
        "eventloop/eventloop.hpp",
        "poller/epoll_poller.hpp",
        "poller/poller.hpp",
        "tcp_conn.hpp",
        "tcp_server.hpp",
        "timer/idle.hpp",
        "timer/timer.hpp",
        "timer/timer_queue.hpp",
    ],
    linkopts = ["-pthread"],
    deps = [
        "//archer/base",
    ],
)

cc_binary(
    name = "timer_test",
    srcs = [
        "tests/timer_test.cpp",
    ],
    linkopts = ["-pthread"],
    deps = [
        ":net",
    ],
)

cc_binary(
    name = "echo_test",
    srcs = [
        "tests/echo_test.cpp",
    ],
    linkopts = ["-pthread"],
    deps = [
        ":net",
    ],
)


cc_binary(
    name = "msg_test",
    srcs = [
        "tests/msg_test.cpp",
    ],
    linkopts = ["-pthread"],
    deps = [
        ":net",
    ],
)
