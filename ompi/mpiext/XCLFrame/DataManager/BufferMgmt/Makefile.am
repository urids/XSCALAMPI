#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib

bufferMgmtdir=$(libdir)/XCLFrame/DataManager/BufferMgmt

bufferMgmt_LTLIBRARIES = libbufferMgmt.la

#######################################
# Build information for each library

# Sources for libmultiDeviceMgmt
libbufferMgmt_la_SOURCES = bufferInit.c \
			bufferRead.c \
  			bufferWrite.c \
			bufferFree.c

# Linker options libTestProgram
libbufferMgmt_la_LDFLAGS =-module -avoid-version -lOpenCL -lm