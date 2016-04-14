#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib

dataCopydir=$(libdir)/XCLFrame/DataManager/DataCopy

dataCopy_LTLIBRARIES = libdataCopy.la

#######################################
# Build information for each library

# Sources for libmultiDeviceMgmt
libdataCopy_la_SOURCES = intraDevBufferCpy.c

# Linker options libTestProgram
libdataCopy_la_LDFLAGS =-module -avoid-version -lOpenCL -lm