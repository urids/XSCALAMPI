#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib

dvMgmtdir=$(libdir)/XCLFrame/binding

dvMgmt_LTLIBRARIES = libdvMgmt.la


#######################################
# Build information for each library

# Sources for libmultiDeviceMgmt
libdvMgmt_la_SOURCES =dvXplore.c \
					  dvInit.c \
					  memMgmt.c \
					  commsBench.c
					  
libdvMgmt_la_LDFLAGS = -ldl	-lOpenCL -Wl,-rpath,$(libdir)/XCLFrame/multiDeviceMgmt			  
					  