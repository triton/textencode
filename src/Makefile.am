nobase_include_HEADERS =
noinst_HEADERS =
pkgconfig_DATA = textencode.pc
lib_LTLIBRARIES = libtextencode.la
libtextencode_la_SOURCES =
libtextencode_la_LIBADD = $(COMMON_LIBS)

nobase_include_HEADERS += textencode/base_n.hpp
libtextencode_la_SOURCES += textencode/base_n.cpp

nobase_include_HEADERS += textencode/binary.hpp
libtextencode_la_SOURCES += textencode/binary.cpp

nobase_include_HEADERS += textencode/common.hpp

nobase_include_HEADERS += textencode/fd.hpp
libtextencode_la_SOURCES += textencode/fd.cpp

nobase_include_HEADERS += textencode/map.hpp
libtextencode_la_SOURCES += textencode/map.cpp

nobase_include_HEADERS += textencode/nix.hpp
libtextencode_la_SOURCES += textencode/nix.cpp

noinst_HEADERS += textencode/internal/base_n.hpp
noinst_HEADERS += textencode/internal/common.hpp
noinst_HEADERS += textencode/internal/nix.hpp
noinst_HEADERS += textencode/internal/utils.hpp


EXTRA_DIST = ../third_party/CLI11/include
bin_PROGRAMS =

if BUILD_CLI

bin_PROGRAMS += textencode/textencode
textencode_textencode_SOURCES = textencode/main.cpp
textencode_textencode_LDADD = libtextencode.la $(COMMON_LIBS)

endif
