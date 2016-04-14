
kernelMgmtdir=$(libdir)/XCLFrame/TaskManager/KernelMgmt

kernelMgmt_LTLIBRARIES = libkernelMgmt.la

#######################################
# Build information for this library

# Sources for libmultiDeviceMgmt
libkernelMgmt_la_SOURCES = taskProcedures.c \
						   taskProcedures.h \
						   argsParsing.c

# Linker options libTestProgram
libkernelMgmt_la_LDFLAGS =-module -avoid-version -lOpenCL