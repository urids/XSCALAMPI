dataManagerdir=$(libdir)/XCLFrame/DataManager
dataManager_LTLIBRARIES = libdataManager.la

#######################################
# Build information for each library
 
       
# Sources for libdataManager
libdataManager_la_SOURCES = ./Base/dataManager.c \
							./Base/dLinkListMgmt.c \
							./Base/dMutexMgmt.c		

#######################################		
# Linker options  for the library
##libdataManager_la_LDFLAGS =-module -avoid-version -lOpenCL -lm -ldl
libdataManager_la_LDFLAGS =-avoid-version -lOpenCL -lm -ldl

