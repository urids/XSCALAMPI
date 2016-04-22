#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib

schedulingdir=$(libdir)/XCLFrame/Scheduling

scheduling_LTLIBRARIES = libscheduling.la


#######################################
# Build information for each library

# Sources for libcommsInfo
libscheduling_la_SOURCES =./Base/scheduling.c \
						  ./Base/taskSetup.c
					  
libscheduling_la_LDFLAGS = -avoid-version -lOpenCL -lm -ldl		  				  