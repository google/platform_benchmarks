licenses(["unencumbered"])
# licenses(["notice"])  # License is similar to BSD.
# exports_files(["LICENSE"])

cc_binary(
    name = "bigtable_analog",
    srcs = [
        "bigtable_analog.c",
    ],
    copts = ["-O0 -g -DNDEBUG"],
    linkopts = ["-static"],  # security: disable=cc-static-no-pie
    linkstatic = 1,
    deps = [
        "//third_party/platform_benchmarks:util",
    ],
)

cc_binary(
    name = "memcpy_analog",
    srcs = [
        "exegesis_memcpy.h",
        "local_memcpy.h",
        "memcpy_analog.c",
        "memcpy_analog.h",
    ],
    copts = ["-O3 -DNDEBUG"],
    linkopts = ["-static"],  # security: disable=cc-static-no-pie
    linkstatic = 1,
    deps = [
        "//third_party/platform_benchmarks:util",
        "//third_party/platform_benchmarks:x86_primitives",
    ],
)
