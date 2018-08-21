#take Care about the openCL inclusion
AM_CPPFLAGS = -DOMPI_PROFILE_LAYER=0 -DOMPI_COMPILING_FORTRAN_WRAPPERS=1 #-I/home/uriel/Dev/mpiApps/FTWrkDistr/multiDeviceMgmt
#AM_LDFLAGS=-L$(libdir)/XCLFrame/multiDeviceMgmt -lmultiDeviceMgmt #this can be avoided because we use dyn linkage =)!!
# Convenience libtool library that will be slurped up into libmpi.la.
noinst_LTLIBRARIES = libmpiext_XCLFrame_c.la

# This is where the top-level header file (that is included in
# <mpi-ext.h>) must be installed.
ompidir = $(ompiincludedir)/ompi/mpiext/XCLFrame/c

# This is the header file that is installed.
ompi_HEADERS = mpiext_XCLFrame_c.h hiddenComms.h singleSidedComms/XclScatter.h

# Sources for the convenience libtool library.  Other than the one
# header file, all source files in the extension have no file naming
# conventions.
libmpiext_XCLFrame_c_la_SOURCES = \
        $(ompi_HEADERS) \
        mpiext_XCLFrame_early.c \
        mpiext_XCLFrame_late.c \
        hiddenComms.c \
        singleSidedComms/XclScatter.c \
        singleSidedComms/singleSidedComms.c
       
libmpiext_XCLFrame_c_la_LDFLAGS = -module -avoid-version -ldl 