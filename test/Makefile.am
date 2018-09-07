@VALGRIND_CHECK_RULES@
@CODE_COVERAGE_RULES@

gtest_cppflags = $(AM_CPPFLAGS) $(GTEST_CFLAGS) $(GMOCK_CFLAGS)
gtest_ldadd = $(TEXTENCODE_LIBS) $(GTEST_LIBS) $(GMOCK_LIBS) -lgmock_main

check_PROGRAMS =
TESTS = $(check_PROGRAMS)
noinst_HEADERS = common.hpp

check_PROGRAMS += base_n
base_n_SOURCES = base_n.cpp
base_n_CPPFLAGS = $(gtest_cppflags)
base_n_LDADD = $(gtest_ldadd)

check_PROGRAMS += binary
binary_SOURCES = binary.cpp
binary_CPPFLAGS = $(gtest_cppflags)
binary_LDADD = $(gtest_ldadd)

check_PROGRAMS += internal/base_n
internal_base_n_SOURCES = internal/base_n.cpp
internal_base_n_CPPFLAGS = $(gtest_cppflags)
internal_base_n_LDADD = $(gtest_ldadd)

check_PROGRAMS += internal/utils
internal_utils_SOURCES = internal/utils.cpp
internal_utils_CPPFLAGS = $(gtest_cppflags)
internal_utils_LDADD = $(gtest_ldadd)

check_PROGRAMS += internal/nix
internal_nix_SOURCES = internal/nix.cpp
internal_nix_CPPFLAGS = $(gtest_cppflags)
internal_nix_LDADD = $(gtest_ldadd)

check_PROGRAMS += nix
nix_SOURCES = nix.cpp
nix_CPPFLAGS = $(gtest_cppflags)
nix_LDADD = $(gtest_ldadd)
