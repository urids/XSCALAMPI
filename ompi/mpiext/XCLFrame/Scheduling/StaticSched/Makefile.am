
staticScheddir=$(libdir)/XCLFrame/Scheduling/StaticSched

staticSched_LTLIBRARIES = libstaticSched.la

#######################################
# Build information for this library

# Sources for libstaticSched
libstaticSched_la_SOURCES = wrapperFactory.cpp \
							./ISH/ISH.cpp \
							./HEFT/HEFT.cpp \
							./Utilities/topoSort.cpp \
							./Utilities/utilities.cpp \
							./U_TEST/entryPnt.c \
							./U_TEST/SSentryPnt.cpp \
							./U_TEST/randNumbers.cpp \
							./U_TEST/randDag.cpp \
							./U_TEST/randRsrcGraph.cpp							
							
# Linker options libTestProgram
libstaticSched_la_LDFLAGS =-module -shared -fPIC -avoid-version -lOpenCL \
						   -Wl,-e,entrypSS -ldl
AM_CXXFLAGS = -std=c++0x -g -O0