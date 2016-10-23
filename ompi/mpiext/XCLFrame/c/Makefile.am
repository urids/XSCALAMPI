#take Care about the openCL inclusion
AM_CPPFLAGS = -DOMPI_PROFILE_LAYER=0 -DOMPI_COMPILING_FORTRAN_WRAPPERS=1 #-I/home/uriel/Dev/mpiApps/FTWrkDistr/multiDeviceMgmt
#AM_LDFLAGS=-L$(libdir)/XCLFrame/multiDeviceMgmt -lmultiDeviceMgmt #this can be avoided because we use dyn linkage =)!!
# Convenience libtool library that will be slurped up into libmpi.la.
noinst_LTLIBRARIES = libmpiext_XCLFrame_c.la

# This is where the top-level header file (that is included in
# <mpi-ext.h>) must be installed.
ompidir = $(ompiincludedir)/ompi/mpiext/XCLFrame/c

# Installable headers file.
#THE XclScatter.h show us that headers can be distributed on multiple blocks.
ompi_HEADERS = mpiext_XCLFrame_c.h \
			   ../DataManager/Base/entityInfo.h
			   
#../DataManager/TwoSidedComms/XclScatter.h
# Sources for the convenience libtool library.  Other than the one
# header file, all source files in the extension have no file naming
# conventions.


#######################################
# Build information for XCLFrame_c library


libmpiext_XCLFrame_c_la_SOURCES = \
        $(ompi_HEADERS) \
        mpiext_XCLFrame_early.c \
        mpiext_XCLFrame_late.c \
        c_Interface.c \
        argsParser.c \
        getStoragePath.c \
        ./CallsBag/callsBag.c \
        ./ErrorHandling/errorHandling.c 
		
		
#######################################		
# Linker options  for XCLFrame_c library
#libmpiext_XCLFrame_c_la_LIBADD = -L$(libdir)/XCLFrame/DataManager -ldataManager

libmpiext_XCLFrame_c_la_LIBADD = $(top_ompi_builddir)/ompi/mpiext/XCLFrame/DataManager/libdataManager.la \
								 $(top_ompi_builddir)/ompi/mpiext/XCLFrame/TaskManager/libtaskManager.la \
								 $(top_ompi_builddir)/ompi/mpiext/XCLFrame/Scheduling/libscheduling.la \
								 $(top_ompi_builddir)/ompi/mpiext/XCLFrame/PoolManager/libpoolManager.la \
								 $(top_ompi_builddir)/ompi/mpiext/XCLFrame/multiDeviceMgmt/libmultiDeviceMgmt.la

libmpiext_XCLFrame_c_la_LDFLAGS = -module -avoid-version -ldl -lOpenCL -lm \
	-Wl,-rpath,$(libdir)/XCLFrame/PoolManager \
	-Wl,-rpath,$(libdir)/XCLFrame/DataManager \
	-Wl,-rpath,$(libdir)/XCLFrame/DataManager/DataCopy \
	-Wl,-rpath,$(libdir)/XCLFrame/DataManager/BufferMgmt \
	-Wl,-rpath,$(libdir)/XCLFrame/DataManager/CollectiveOps \
	-Wl,-rpath,$(libdir)/XCLFrame/TaskManager \
    -Wl,-rpath,$(libdir)/XCLFrame/TaskManager/KernelMgmt \
    -Wl,-rpath,$(libdir)/XCLFrame/TaskManager/SynchMgmt \
    -Wl,-rpath,$(libdir)/XCLFrame/Scheduling \
    -Wl,-rpath,$(libdir)/XCLFrame/Scheduling/ManualSched \
    -Wl,-rpath,$(libdir)/XCLFrame/Scheduling/StaticSched \
    -Wl,-rpath,$(libdir)/XCLFrame/Scheduling/DynamicSched \
    -Wl,-rpath,$(libdir)/XCLFrame/Scheduling/DynamicSched/DefaultDynamic \
    -Wl,-rpath,$(libdir)/XCLFrame/Scheduling/Benchmark \
    -Wl,-rpath,$(libdir)/XCLFrame/multiDeviceMgmt   	
#	 \
#	-L/home/uriel/mpiELFs/lib/XCLFrame/DataManager -ldataManager

libmpiext_XCLFrame_c_la_DEPENDENCIES = $(libmpiext_XCLFrame_c_la_LIBADD)	

	