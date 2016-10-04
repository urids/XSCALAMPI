
defaultDynamicdir=$(libdir)/XCLFrame/Scheduling/DynamicSched/DefaultDynamic

defaultDynamic_LTLIBRARIES = libdefaultDynamic.la

#######################################
# Build information for this library

# Sources for libdynamicSched
libdefaultDynamic_la_SOURCES = defaultFactory.cpp \
							  ./RoundRobin/RR.cpp
											  
# Linker options libTestProgram
libdefaultDynamic_la_LDFLAGS =-module -avoid-version -lOpenCL

AM_CXXFLAGS = -std=c++11