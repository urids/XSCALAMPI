#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib

poolManagerdir=$(libdir)/XCLFrame/PoolManager
poolManager_LTLIBRARIES = libpoolManager.la


#######################################
# Build information for each library

# Sources for libcommsInfo
libpoolManager_la_SOURCES =./Base/poolManager.c \
						  ./Base/queues.c
					  
libpoolManager_la_LDFLAGS = -avoid-version -lOpenCL -ldl		  				  