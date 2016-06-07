#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib

tskMgmtdir=$(libdir)/XCLFrame/binding

tskMgmt_LTLIBRARIES = libtskMgmt.la


#######################################
# Build information for each library

# Sources for libtskMgmt
libtskMgmt_la_SOURCES =tskMgmt.c \
		       tskExec.c \
		       taskScheduling.c
					  
libtskMgmt_la_LDFLAGS = -ldl -lOpenCL -Wl,-rpath,$(libdir)/XCLFrame/multiDeviceMgmt \
    -Wl,-rpath,$(libdir)/XCLFrame/DataManager \
    -Wl,-rpath,$(libdir)/XCLFrame/TaskManager 	
