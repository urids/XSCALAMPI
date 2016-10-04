
dynamicScheddir=$(libdir)/XCLFrame/Scheduling/DynamicSched

dynamicSched_LTLIBRARIES = libdynamicSched.la

#######################################
# Build information for this library

# Sources for libdynamicSched
libdynamicSched_la_SOURCES = asbtractFactoryWrapper.cpp 
											  
# Linker options libTestProgram
libdynamicSched_la_LDFLAGS =-module -avoid-version -lOpenCL \
						  -Wl,-rpath,$(libdir)/XCLFrame/Scheduling/DynamicSched/DefaultDynamic

AM_CXXFLAGS = -std=c++11