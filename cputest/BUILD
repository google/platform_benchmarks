# security: disable=cc-static-no-pie

cc_library(
    name = "alu",
    srcs = ["alu.c"],
    hdrs = [
        "alu.h",
    ],
    deps = [
        "//third_party/platform_benchmarks:util",
    ],
)

cc_library(
    name = "vector",
    srcs = ["vector.c"],
    hdrs = [
        "vector.h",
    ],
    deps = [
        "//third_party/platform_benchmarks:util",
    ],
)

cc_library(
    name = "load",
    srcs = ["load.c"],
    hdrs = [
        "load.h",
    ],
    deps = [
        "//third_party/platform_benchmarks:util",
    ],
)

cc_library(
    name = "store",
    srcs = ["store.c"],
    hdrs = [
        "store.h",
    ],
    deps = [
        "//third_party/platform_benchmarks:util",
    ],
)

cc_library(
    name = "branch",
    srcs = ["branch.c"],
    hdrs = [
        "branch.h",
    ],
    linkstatic = 1,
    deps = [
        "//third_party/platform_benchmarks:util",
    ],
)

cc_library(
    name = "loadstore",
    srcs = ["loadstore.c"],
    hdrs = [
        "load.h",
        "loadstore.h",
        "store.h",
    ],
    deps = [
        "//third_party/platform_benchmarks:util",
        "//third_party/platform_benchmarks:x86_primitives",
    ],
)

cc_library(
    name = "serializing",
    srcs = ["serializing.c"],
    deps = [
        "//third_party/platform_benchmarks:util",
    ],
)

cc_binary(
    name = "cputest",
    srcs = [
        "cputest.c",
        "cputest.h",
    ],
    linkopts = ["-static"],  # security: disable=cc-static-no-pie
    linkstatic = 1,
    deps = [
        "alu",
        "branch",
        "load",
        "loadstore",
        "serializing",
        "store",
        "vector",
        "//third_party/platform_benchmarks:util",
    ],
)
