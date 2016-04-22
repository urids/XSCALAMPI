
# $HEADER$
#

# This Makefile is not traversed during a normal "make all" in an OMPI
# build.  It *is* traversed during "make dist", however.  So you can
# put EXTRA_DIST targets in here.
#
# You can also use this as a convenience for building this MPI
# extension (i.e., "make all" in this directory to invoke "make all"
# in all the subdirectories).

SUBDIRS = DataManager \
		DataManager/DataCopy \
		DataManager/BufferMgmt \
		c \
		c/binding/dvMgmt \
		c/binding/tskMgmt \
		multiDeviceMgmt \
		TaskManager \
		TaskManager/KernelMgmt \
		TaskManager/SynchMgmt \
		Scheduling \
		Scheduling/Benchmark \
		Scheduling/ManualSched

EXTRA_DIST = README.txt
