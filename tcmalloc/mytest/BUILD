load("//tcmalloc:copts.bzl", "TCMALLOC_DEFAULT_COPTS")
load("//tcmalloc:variants.bzl", "create_tcmalloc_benchmark", "create_tcmalloc_testsuite")

REGTEST_DEPS = [
    ":testutil",
    ":thread_manager",
    "@com_google_googletest//:gtest",
    "@com_google_absl//absl/base",
    "@com_google_absl//absl/base:core_headers",
    "@com_google_absl//absl/debugging:leak_check",
    "@com_google_absl//absl/random",
    "@com_google_absl//absl/random:distributions",
    "@com_google_absl//absl/strings",
    "@com_google_absl//absl/strings:str_format",
    "@com_google_absl//absl/synchronization",
    "@com_google_absl//absl/utility",
    "@com_google_absl//absl/numeric:bits",
    "//tcmalloc:headers_for_tests",
    "//tcmalloc/internal:declarations",
    "//tcmalloc/internal:logging",
    "//tcmalloc/internal:parameter_accessors",
    "//tcmalloc:malloc_extension",
]

REGTEST_OPTS = [
    "$(STACK_FRAME_UNLIMITED)",  # required for tcmalloc_test.cc
] + TCMALLOC_DEFAULT_COPTS

EMPIRICAL_DEPS = [
    ":empirical",
    ":empirical_distributions",
    ":testutil",
    "//tcmalloc/internal:atomic_stats_counter",
    "//tcmalloc/internal:logging",
    "//tcmalloc/internal:util",
    "//tcmalloc:want_hpaa",
    "@com_google_absl//absl/base",
    "@com_google_absl//absl/base:core_headers",
    "@com_google_absl//absl/flags:flag",
    "@com_google_absl//absl/flags:parse",
    "@com_google_absl//absl/memory",
    "@com_google_absl//absl/random:distributions",
    "@com_google_absl//absl/strings",
    "@com_google_absl//absl/strings:str_format",
    "@com_google_absl//absl/synchronization",
    "@com_google_absl//absl/time",
    "//tcmalloc:malloc_extension",

cc_library(
    name = "testutil",
    testonly = 1,
    srcs = ["testutil.cc"],
    hdrs = ["testutil.h"],
    copts = TCMALLOC_DEFAULT_COPTS,
    visibility = [
        "//tcmalloc:__subpackages__",
    ],
    deps = [
        "//tcmalloc:malloc_extension",
        "@com_github_google_benchmark//:benchmark",
        "@com_google_absl//absl/base:core_headers",
    ],
)

]
# Minh
cc_test(
    name = "huge_pagemap_test",
    srcs = ["huge_pagemap_test.cc"],
    copts = TCMALLOC_DEFAULT_COPTS,
    deps = [
        "//tcmalloc:common",
        "//tcmalloc/internal:optimization",
        "//tcmalloc/internal:util",
        "@com_github_google_benchmark//:benchmark",
        "@com_google_googletest//:gtest_main",
    ],
)