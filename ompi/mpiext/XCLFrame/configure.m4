# -*- shell-script -*-
#
# 
# $HEADER$
#

# OMPI_MPIEXT_example_CONFIG([action-if-found], [action-if-not-found])
# -----------------------------------------------------------
# HERE WE DECLARE WICH MAKEFILES MUST BE CREATED.

AC_DEFUN([OMPI_MPIEXT_XCLFrame_CONFIG],[
    AC_CONFIG_FILES([ompi/mpiext/XCLFrame/Makefile])
    AC_CONFIG_FILES([ompi/mpiext/XCLFrame/c/Makefile])
    AC_CONFIG_FILES([ompi/mpiext/XCLFrame/PoolManager/Makefile])
    AC_CONFIG_FILES([ompi/mpiext/XCLFrame/multiDeviceMgmt/Makefile])    
    AC_CONFIG_FILES([ompi/mpiext/XCLFrame/Scheduling/Makefile])
    AC_CONFIG_FILES([ompi/mpiext/XCLFrame/Scheduling/Benchmark/Makefile])
    AC_CONFIG_FILES([ompi/mpiext/XCLFrame/Scheduling/ManualSched/Makefile])
    AC_CONFIG_FILES([ompi/mpiext/XCLFrame/TaskManager/Makefile])
    AC_CONFIG_FILES([ompi/mpiext/XCLFrame/TaskManager/KernelMgmt/Makefile])
    AC_CONFIG_FILES([ompi/mpiext/XCLFrame/TaskManager/SynchMgmt/Makefile])
    AC_CONFIG_FILES([ompi/mpiext/XCLFrame/DataManager/Makefile])
    AC_CONFIG_FILES([ompi/mpiext/XCLFrame/DataManager/BufferMgmt/Makefile])
    AC_CONFIG_FILES([ompi/mpiext/XCLFrame/DataManager/DataCopy/Makefile])

    # This example can always build, so we just execute $1.
    $1
])

# only need to set this if the component needs init/finalize hooks
AC_DEFUN([OMPI_MPIEXT_XCLFrame_NEED_INIT], [1])

