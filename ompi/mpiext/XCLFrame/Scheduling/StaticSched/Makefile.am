
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
							./U_TEST/staticSchedulersTest.cpp \
							./U_TEST/entryPnt.c
							
# Linker options libTestProgram
libstaticSched_la_LDFLAGS =-module -shared -fPIC -avoid-version -lOpenCL -Wl,-e,entryp
AM_CXXFLAGS = -std=c++11