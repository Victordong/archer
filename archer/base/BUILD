load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "base",
    srcs = [
        "thread.cpp",
        "socket.cpp",
        "buffer.cpp"
    ],
    hdrs = [
        "noncopyable.hpp",
        "socket.hpp",
        "timestamp.hpp",
        "thread.hpp",
        "buffer.hpp",
        "safe_queue.hpp"
    ],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
)
