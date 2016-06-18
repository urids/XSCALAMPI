#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib

collectiveOpsdir=$(libdir)/XCLFrame/DataManager/CollectiveOps

collectiveOps_LTLIBRARIES = libcollectiveOps.la

#######################################
# Build information for each library

# Sources for libcollectiveOps
libcollectiveOps_la_SOURCES = XclGather.c \
							  XclScatter.c 
        	

# Linker options libTestProgram
libcollectiveOps_la_LDFLAGS =-module -avoid-version -lOpenCL -lm