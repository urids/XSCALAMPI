
staticScheddir=$(libdir)/XCLFrame/Scheduling/StaticSched

staticSched_LTLIBRARIES = libstaticSched.la

#######################################
# Build information for this library

# Sources for libstaticSched
libstaticSched_la_SOURCES = wrapperFactory.cpp \
							ISH.cpp \
							./Utilities/topoSort.cpp

# Linker options libTestProgram
libstaticSched_la_LDFLAGS =-module -avoid-version -lOpenCL

AM_CXXFLAGS = -std=c++11