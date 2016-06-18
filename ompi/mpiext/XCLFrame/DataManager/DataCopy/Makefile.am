#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib

dataCopydir=$(libdir)/XCLFrame/DataManager/DataCopy

dataCopy_LTLIBRARIES = libdataCopy.la

#######################################
# Build information for each library

# Sources for libmultiDeviceMgmt
libdataCopy_la_SOURCES = bufferRead.c \
  			bufferWrite.c \
        	./CopyContainers/containersList.c \
        	./IntraDev/intraDev.c \
        	./IntraDev/intraDevBufferCpy.c \
        	./InterDev/interDev.c 
        	

# Linker options libTestProgram
libdataCopy_la_LDFLAGS =-module -avoid-version -lOpenCL -lm