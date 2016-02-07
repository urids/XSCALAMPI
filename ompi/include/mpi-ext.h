/*
 * $HEADER$
 */

#ifndef OMPI_MPI_EXT_H
#define OMPI_MPI_EXT_H 1

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif

#define OMPI_HAVE_MPI_EXT 1

/* Enabled Extension: XCLFrame */
#define OMPI_HAVE_MPI_EXT_XCLFRAME 1
#include "openmpi/ompi/mpiext/XCLFrame/c/mpiext_XCLFrame_c.h"


#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

#endif /* OMPI_MPI_EXT_H */

