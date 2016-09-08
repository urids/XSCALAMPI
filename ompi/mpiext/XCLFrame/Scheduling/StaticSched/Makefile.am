
staticScheddir=$(libdir)/XCLFrame/Scheduling/StaticSched

staticSched_LTLIBRARIES = libstaticSched.la

#######################################
# Build information for this library

# Sources for libstaticSched
libstaticSched_la_SOURCES = wrapperFactory.cpp \
							./ISH/ISH.cpp \
							./HEFT/HEFT.cpp \
							./Utilities/topoSort.cpp \
							./Utilities/utilities.cpp

# Linker options libTestProgram
libstaticSched_la_LDFLAGS =-module -avoid-version -lOpenCL

AM_CXXFLAGS = -std=c++11