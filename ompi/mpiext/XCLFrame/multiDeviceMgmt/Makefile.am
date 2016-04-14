#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib

multiDeviceMgmtdir=$(libdir)/XCLFrame/multiDeviceMgmt

multiDeviceMgmt_LTLIBRARIES = libmultiDeviceMgmt.la

#######################################
# Build information for each library

# Sources for libmultiDeviceMgmt
libmultiDeviceMgmt_la_SOURCES = ./errorMgmt/errorHandling.c \
								./deviceMgmt/deviceExploration.c \
								./deviceMgmt/devicesInitialization.c 

# Linker options libTestProgram
libmultiDeviceMgmt_la_LDFLAGS = -lOpenCL -lm

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
#libmultiDeviceMgmt_la_CPPFLAGS = -I$(top_srcdir)/include 