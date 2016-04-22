
benchmarkdir=$(libdir)/XCLFrame/Scheduling/Benchmark

benchmark_LTLIBRARIES = libbenchmark.la

#######################################
# Build information for this library

# Sources for libbenchmark
libbenchmark_la_SOURCES = commsBench.c \
						  commsBench.h \
						  compBench.c 						  

# Linker options libTestProgram

#libbenchmark_la_LIBADD = $(top_ompi_builddir)/ompi/mpiext/XCLFrame/DataManager/libdataManager.la \
						 $(top_ompi_builddir)/ompi/mpiext/XCLFrame/TaskManager/libtaskManager.la

libbenchmark_la_LDFLAGS = -module -avoid-version -ldl -lOpenCL -lm \
	-Wl,-rpath,$(libdir)/XCLFrame/DataManager \
	-Wl,-rpath,$(libdir)/XCLFrame/TaskManager
		
#libbenchmark_la_DEPENDENCIES = $(libbenchmark_la_LIBADD)