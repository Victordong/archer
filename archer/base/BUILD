load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "base",
    srcs = [
        "buffer.cpp",
        "logger/logger.cpp",
        "socket.cpp",
        "thread.cpp",
    ],
    hdrs = [
        "buffer.hpp",
        "context.hpp",
        "logger/logger.hpp",
        "noncopyable.hpp",
        "safe_queue.hpp",
        "socket.hpp",
        "thread.hpp",
        "timestamp.hpp",
    ],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
)
