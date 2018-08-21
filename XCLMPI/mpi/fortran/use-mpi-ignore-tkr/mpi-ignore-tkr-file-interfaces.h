! -*- fortran -*-
!
! Copyright (c) 2006-2012 Cisco Systems, Inc.  All rights reserved.
! Copyright (c) 2007      Los Alamos National Security, LLC.  All rights
!                         reserved.
! $COPYRIGHT$
!
! Additional copyrights may follow
!
! $HEADER$


interface MPI_File_call_errhandler

subroutine MPI_File_call_errhandler(fh, errorcode, ierror)
  integer, intent(in) :: fh
  integer, intent(in) :: errorcode
  integer, intent(out) :: ierror
end subroutine MPI_File_call_errhandler

end interface

interface PMPI_File_call_errhandler

subroutine PMPI_File_call_errhandler(fh, errorcode, ierror)
  integer, intent(in) :: fh
  integer, intent(in) :: errorcode
  integer, intent(out) :: ierror
end subroutine PMPI_File_call_errhandler

end interface


interface MPI_File_close

subroutine MPI_File_close(fh, ierror)
  integer, intent(inout) :: fh
  integer, intent(out) :: ierror
end subroutine MPI_File_close

end interface

interface PMPI_File_close

subroutine PMPI_File_close(fh, ierror)
  integer, intent(inout) :: fh
  integer, intent(out) :: ierror
end subroutine PMPI_File_close

end interface


interface MPI_File_create_errhandler

subroutine MPI_File_create_errhandler(function, errhandler, ierror)
  external :: function
  integer, intent(out) :: errhandler
  integer, intent(out) :: ierror
end subroutine MPI_File_create_errhandler

end interface

interface PMPI_File_create_errhandler

subroutine PMPI_File_create_errhandler(function, errhandler, ierror)
  external :: function
  integer, intent(out) :: errhandler
  integer, intent(out) :: ierror
end subroutine PMPI_File_create_errhandler

end interface


interface MPI_File_delete

subroutine MPI_File_delete(filename, info, ierror)
  character(len=*), intent(in) :: filename
  integer, intent(in) :: info
  integer, intent(out) :: ierror
end subroutine MPI_File_delete

end interface

interface PMPI_File_delete

subroutine PMPI_File_delete(filename, info, ierror)
  character(len=*), intent(in) :: filename
  integer, intent(in) :: info
  integer, intent(out) :: ierror
end subroutine PMPI_File_delete

end interface


interface MPI_File_get_amode

subroutine MPI_File_get_amode(fh, amode, ierror)
  integer, intent(in) :: fh
  integer, intent(out) :: amode
  integer, intent(out) :: ierror
end subroutine MPI_File_get_amode

end interface

interface PMPI_File_get_amode

subroutine PMPI_File_get_amode(fh, amode, ierror)
  integer, intent(in) :: fh
  integer, intent(out) :: amode
  integer, intent(out) :: ierror
end subroutine PMPI_File_get_amode

end interface


interface MPI_File_get_atomicity

subroutine MPI_File_get_atomicity(fh, flag, ierror)
  integer, intent(in) :: fh
  logical, intent(out) :: flag
  integer, intent(out) :: ierror
end subroutine MPI_File_get_atomicity

end interface

interface PMPI_File_get_atomicity

subroutine PMPI_File_get_atomicity(fh, flag, ierror)
  integer, intent(in) :: fh
  logical, intent(out) :: flag
  integer, intent(out) :: ierror
end subroutine PMPI_File_get_atomicity

end interface


interface MPI_File_get_byte_offset

subroutine MPI_File_get_byte_offset(fh, offset, disp, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
  integer(kind=MPI_OFFSET_KIND), intent(out) :: disp
  integer, intent(out) :: ierror
end subroutine MPI_File_get_byte_offset

end interface

interface PMPI_File_get_byte_offset

subroutine PMPI_File_get_byte_offset(fh, offset, disp, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
  integer(kind=MPI_OFFSET_KIND), intent(out) :: disp
  integer, intent(out) :: ierror
end subroutine PMPI_File_get_byte_offset

end interface


interface MPI_File_get_errhandler

subroutine MPI_File_get_errhandler(file, errhandler, ierror)
  integer, intent(in) :: file
  integer, intent(out) :: errhandler
  integer, intent(out) :: ierror
end subroutine MPI_File_get_errhandler

end interface

interface PMPI_File_get_errhandler

subroutine PMPI_File_get_errhandler(file, errhandler, ierror)
  integer, intent(in) :: file
  integer, intent(out) :: errhandler
  integer, intent(out) :: ierror
end subroutine PMPI_File_get_errhandler

end interface


interface MPI_File_get_group

subroutine MPI_File_get_group(fh, group, ierror)
  integer, intent(in) :: fh
  integer, intent(out) :: group
  integer, intent(out) :: ierror
end subroutine MPI_File_get_group

end interface

interface PMPI_File_get_group

subroutine PMPI_File_get_group(fh, group, ierror)
  integer, intent(in) :: fh
  integer, intent(out) :: group
  integer, intent(out) :: ierror
end subroutine PMPI_File_get_group

end interface


interface MPI_File_get_info

subroutine MPI_File_get_info(fh, info_used, ierror)
  integer, intent(in) :: fh
  integer, intent(out) :: info_used
  integer, intent(out) :: ierror
end subroutine MPI_File_get_info

end interface

interface PMPI_File_get_info

subroutine PMPI_File_get_info(fh, info_used, ierror)
  integer, intent(in) :: fh
  integer, intent(out) :: info_used
  integer, intent(out) :: ierror
end subroutine PMPI_File_get_info

end interface


interface MPI_File_get_position

subroutine MPI_File_get_position(fh, offset, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(out) :: offset
  integer, intent(out) :: ierror
end subroutine MPI_File_get_position

end interface

interface PMPI_File_get_position

subroutine PMPI_File_get_position(fh, offset, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(out) :: offset
  integer, intent(out) :: ierror
end subroutine PMPI_File_get_position

end interface


interface MPI_File_get_position_shared

subroutine MPI_File_get_position_shared(fh, offset, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(out) :: offset
  integer, intent(out) :: ierror
end subroutine MPI_File_get_position_shared

end interface

interface PMPI_File_get_position_shared

subroutine PMPI_File_get_position_shared(fh, offset, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(out) :: offset
  integer, intent(out) :: ierror
end subroutine PMPI_File_get_position_shared

end interface


interface MPI_File_get_size

subroutine MPI_File_get_size(fh, size, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(out) :: size
  integer, intent(out) :: ierror
end subroutine MPI_File_get_size

end interface

interface PMPI_File_get_size

subroutine PMPI_File_get_size(fh, size, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(out) :: size
  integer, intent(out) :: ierror
end subroutine PMPI_File_get_size

end interface


interface MPI_File_get_type_extent

subroutine MPI_File_get_type_extent(fh, datatype, extent, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer, intent(in) :: datatype
  integer(kind=MPI_ADDRESS_KIND), intent(out) :: extent
  integer, intent(out) :: ierror
end subroutine MPI_File_get_type_extent

end interface

interface PMPI_File_get_type_extent

subroutine PMPI_File_get_type_extent(fh, datatype, extent, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer, intent(in) :: datatype
  integer(kind=MPI_ADDRESS_KIND), intent(out) :: extent
  integer, intent(out) :: ierror
end subroutine PMPI_File_get_type_extent

end interface


interface MPI_File_get_view

subroutine MPI_File_get_view(fh, disp, etype, filetype, datarep&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(out) :: disp
  integer, intent(out) :: etype
  integer, intent(out) :: filetype
  character(len=*), intent(out) :: datarep
  integer, intent(out) :: ierror
end subroutine MPI_File_get_view

end interface

interface PMPI_File_get_view

subroutine PMPI_File_get_view(fh, disp, etype, filetype, datarep&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(out) :: disp
  integer, intent(out) :: etype
  integer, intent(out) :: filetype
  character(len=*), intent(out) :: datarep
  integer, intent(out) :: ierror
end subroutine PMPI_File_get_view

end interface


interface MPI_File_iread

subroutine MPI_File_iread(fh, buf, count, datatype, request&
        , ierror)
  integer, intent(in) :: fh
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: request
  integer, intent(out) :: ierror
end subroutine MPI_File_iread

end interface

interface PMPI_File_iread

subroutine PMPI_File_iread(fh, buf, count, datatype, request&
        , ierror)
  integer, intent(in) :: fh
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: request
  integer, intent(out) :: ierror
end subroutine PMPI_File_iread

end interface


interface MPI_File_iread_at

subroutine MPI_File_iread_at(fh, offset, buf, count, datatype, &
        request, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: request
  integer, intent(out) :: ierror
end subroutine MPI_File_iread_at

end interface

interface PMPI_File_iread_at

subroutine PMPI_File_iread_at(fh, offset, buf, count, datatype, &
        request, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: request
  integer, intent(out) :: ierror
end subroutine PMPI_File_iread_at

end interface


interface MPI_File_iread_shared

subroutine MPI_File_iread_shared(fh, buf, count, datatype, request&
        , ierror)
  integer, intent(in) :: fh
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: request
  integer, intent(out) :: ierror
end subroutine MPI_File_iread_shared

end interface

interface PMPI_File_iread_shared

subroutine PMPI_File_iread_shared(fh, buf, count, datatype, request&
        , ierror)
  integer, intent(in) :: fh
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: request
  integer, intent(out) :: ierror
end subroutine PMPI_File_iread_shared

end interface


interface MPI_File_iwrite

subroutine MPI_File_iwrite(fh, buf, count, datatype, request&
        , ierror)
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: request
  integer, intent(out) :: ierror
end subroutine MPI_File_iwrite

end interface

interface PMPI_File_iwrite

subroutine PMPI_File_iwrite(fh, buf, count, datatype, request&
        , ierror)
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: request
  integer, intent(out) :: ierror
end subroutine PMPI_File_iwrite

end interface


interface MPI_File_iwrite_at

subroutine MPI_File_iwrite_at(fh, offset, buf, count, datatype, &
        request, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: request
  integer, intent(out) :: ierror
end subroutine MPI_File_iwrite_at

end interface

interface PMPI_File_iwrite_at

subroutine PMPI_File_iwrite_at(fh, offset, buf, count, datatype, &
        request, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: request
  integer, intent(out) :: ierror
end subroutine PMPI_File_iwrite_at

end interface


interface MPI_File_iwrite_shared

subroutine MPI_File_iwrite_shared(fh, buf, count, datatype, request&
        , ierror)
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: request
  integer, intent(out) :: ierror
end subroutine MPI_File_iwrite_shared

end interface

interface PMPI_File_iwrite_shared

subroutine PMPI_File_iwrite_shared(fh, buf, count, datatype, request&
        , ierror)
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: request
  integer, intent(out) :: ierror
end subroutine PMPI_File_iwrite_shared

end interface


interface MPI_File_open

subroutine MPI_File_open(comm, filename, amode, info, fh&
        , ierror)
  integer, intent(in) :: comm
  character(len=*), intent(in) :: filename
  integer, intent(in) :: amode
  integer, intent(in) :: info
  integer, intent(out) :: fh
  integer, intent(out) :: ierror
end subroutine MPI_File_open

end interface

interface PMPI_File_open

subroutine PMPI_File_open(comm, filename, amode, info, fh&
        , ierror)
  integer, intent(in) :: comm
  character(len=*), intent(in) :: filename
  integer, intent(in) :: amode
  integer, intent(in) :: info
  integer, intent(out) :: fh
  integer, intent(out) :: ierror
end subroutine PMPI_File_open

end interface


interface MPI_File_preallocate

subroutine MPI_File_preallocate(fh, size, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: size
  integer, intent(out) :: ierror
end subroutine MPI_File_preallocate

end interface

interface PMPI_File_preallocate

subroutine PMPI_File_preallocate(fh, size, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: size
  integer, intent(out) :: ierror
end subroutine PMPI_File_preallocate

end interface


interface MPI_File_read

subroutine MPI_File_read(fh, buf, count, datatype, status&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine MPI_File_read

end interface

interface PMPI_File_read

subroutine PMPI_File_read(fh, buf, count, datatype, status&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine PMPI_File_read

end interface


interface MPI_File_read_all

subroutine MPI_File_read_all(fh, buf, count, datatype, status&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine MPI_File_read_all

end interface

interface PMPI_File_read_all

subroutine PMPI_File_read_all(fh, buf, count, datatype, status&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine PMPI_File_read_all

end interface


interface MPI_File_read_all_begin

subroutine MPI_File_read_all_begin(fh, buf, count, datatype, ierror)
  integer, intent(in) :: fh
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: ierror
end subroutine MPI_File_read_all_begin

end interface

interface PMPI_File_read_all_begin

subroutine PMPI_File_read_all_begin(fh, buf, count, datatype, ierror)
  integer, intent(in) :: fh
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: ierror
end subroutine PMPI_File_read_all_begin

end interface


interface MPI_File_read_all_end

subroutine MPI_File_read_all_end(fh, buf, status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
   :: buf
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine MPI_File_read_all_end

end interface

interface PMPI_File_read_all_end

subroutine PMPI_File_read_all_end(fh, buf, status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
   :: buf
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine PMPI_File_read_all_end

end interface


interface MPI_File_read_at

subroutine MPI_File_read_at(fh, offset, buf, count, datatype, &
        status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine MPI_File_read_at

end interface

interface PMPI_File_read_at

subroutine PMPI_File_read_at(fh, offset, buf, count, datatype, &
        status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine PMPI_File_read_at

end interface


interface MPI_File_read_at_all

subroutine MPI_File_read_at_all(fh, offset, buf, count, datatype, &
        status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine MPI_File_read_at_all

end interface

interface PMPI_File_read_at_all

subroutine PMPI_File_read_at_all(fh, offset, buf, count, datatype, &
        status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine PMPI_File_read_at_all

end interface


interface MPI_File_read_at_all_begin

subroutine MPI_File_read_at_all_begin(fh, offset, buf, count, datatype&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: ierror
end subroutine MPI_File_read_at_all_begin

end interface

interface PMPI_File_read_at_all_begin

subroutine PMPI_File_read_at_all_begin(fh, offset, buf, count, datatype&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: ierror
end subroutine PMPI_File_read_at_all_begin

end interface


interface MPI_File_read_at_all_end

subroutine MPI_File_read_at_all_end(fh, buf, status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
   :: buf
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine MPI_File_read_at_all_end

end interface

interface PMPI_File_read_at_all_end

subroutine PMPI_File_read_at_all_end(fh, buf, status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
   :: buf
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine PMPI_File_read_at_all_end

end interface


interface MPI_File_read_ordered

subroutine MPI_File_read_ordered(fh, buf, count, datatype, status&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine MPI_File_read_ordered

end interface

interface PMPI_File_read_ordered

subroutine PMPI_File_read_ordered(fh, buf, count, datatype, status&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine PMPI_File_read_ordered

end interface


interface MPI_File_read_ordered_begin

subroutine MPI_File_read_ord_begin(fh, buf, count, datatype, ierror)
  integer, intent(in) :: fh
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: ierror
end subroutine MPI_File_read_ord_begin

end interface

interface PMPI_File_read_ordered_begin

subroutine PMPI_File_read_ord_begin(fh, buf, count, datatype, ierror)
  integer, intent(in) :: fh
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: ierror
end subroutine PMPI_File_read_ord_begin

end interface


interface MPI_File_read_ordered_end

subroutine MPI_File_read_ordered_end(fh, buf, status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
   :: buf
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine MPI_File_read_ordered_end

end interface

interface PMPI_File_read_ordered_end

subroutine PMPI_File_read_ordered_end(fh, buf, status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
   :: buf
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine PMPI_File_read_ordered_end

end interface


interface MPI_File_read_shared

subroutine MPI_File_read_shared(fh, buf, count, datatype, status&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine MPI_File_read_shared

end interface

interface PMPI_File_read_shared

subroutine PMPI_File_read_shared(fh, buf, count, datatype, status&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
   :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine PMPI_File_read_shared

end interface


interface MPI_File_seek

subroutine MPI_File_seek(fh, offset, whence, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
  integer, intent(in) :: whence
  integer, intent(out) :: ierror
end subroutine MPI_File_seek

end interface

interface PMPI_File_seek

subroutine PMPI_File_seek(fh, offset, whence, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
  integer, intent(in) :: whence
  integer, intent(out) :: ierror
end subroutine PMPI_File_seek

end interface


interface MPI_File_seek_shared

subroutine MPI_File_seek_shared(fh, offset, whence, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
  integer, intent(in) :: whence
  integer, intent(out) :: ierror
end subroutine MPI_File_seek_shared

end interface

interface PMPI_File_seek_shared

subroutine PMPI_File_seek_shared(fh, offset, whence, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
  integer, intent(in) :: whence
  integer, intent(out) :: ierror
end subroutine PMPI_File_seek_shared

end interface


interface MPI_File_set_atomicity

subroutine MPI_File_set_atomicity(fh, flag, ierror)
  integer, intent(in) :: fh
  logical, intent(in) :: flag
  integer, intent(out) :: ierror
end subroutine MPI_File_set_atomicity

end interface

interface PMPI_File_set_atomicity

subroutine PMPI_File_set_atomicity(fh, flag, ierror)
  integer, intent(in) :: fh
  logical, intent(in) :: flag
  integer, intent(out) :: ierror
end subroutine PMPI_File_set_atomicity

end interface


interface MPI_File_set_errhandler

subroutine MPI_File_set_errhandler(file, errhandler, ierror)
  integer, intent(in) :: file
  integer, intent(in) :: errhandler
  integer, intent(out) :: ierror
end subroutine MPI_File_set_errhandler

end interface

interface PMPI_File_set_errhandler

subroutine PMPI_File_set_errhandler(file, errhandler, ierror)
  integer, intent(in) :: file
  integer, intent(in) :: errhandler
  integer, intent(out) :: ierror
end subroutine PMPI_File_set_errhandler

end interface


interface MPI_File_set_info

subroutine MPI_File_set_info(fh, info, ierror)
  integer, intent(in) :: fh
  integer, intent(in) :: info
  integer, intent(out) :: ierror
end subroutine MPI_File_set_info

end interface

interface PMPI_File_set_info

subroutine PMPI_File_set_info(fh, info, ierror)
  integer, intent(in) :: fh
  integer, intent(in) :: info
  integer, intent(out) :: ierror
end subroutine PMPI_File_set_info

end interface


interface MPI_File_set_size

subroutine MPI_File_set_size(fh, size, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: size
  integer, intent(out) :: ierror
end subroutine MPI_File_set_size

end interface

interface PMPI_File_set_size

subroutine PMPI_File_set_size(fh, size, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: size
  integer, intent(out) :: ierror
end subroutine PMPI_File_set_size

end interface


interface MPI_File_set_view

subroutine MPI_File_set_view(fh, disp, etype, filetype, datarep, &
        info, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: disp
  integer, intent(in) :: etype
  integer, intent(in) :: filetype
  character(len=*), intent(in) :: datarep
  integer, intent(in) :: info
  integer, intent(out) :: ierror
end subroutine MPI_File_set_view

end interface

interface PMPI_File_set_view

subroutine PMPI_File_set_view(fh, disp, etype, filetype, datarep, &
        info, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: disp
  integer, intent(in) :: etype
  integer, intent(in) :: filetype
  character(len=*), intent(in) :: datarep
  integer, intent(in) :: info
  integer, intent(out) :: ierror
end subroutine PMPI_File_set_view

end interface


interface MPI_File_sync

subroutine MPI_File_sync(fh, ierror)
  integer, intent(in) :: fh
  integer, intent(out) :: ierror
end subroutine MPI_File_sync

end interface

interface PMPI_File_sync

subroutine PMPI_File_sync(fh, ierror)
  integer, intent(in) :: fh
  integer, intent(out) :: ierror
end subroutine PMPI_File_sync

end interface


interface MPI_File_write

subroutine MPI_File_write(fh, buf, count, datatype, status&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine MPI_File_write

end interface

interface PMPI_File_write

subroutine PMPI_File_write(fh, buf, count, datatype, status&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine PMPI_File_write

end interface


interface MPI_File_write_all

subroutine MPI_File_write_all(fh, buf, count, datatype, status&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine MPI_File_write_all

end interface

interface PMPI_File_write_all

subroutine PMPI_File_write_all(fh, buf, count, datatype, status&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine PMPI_File_write_all

end interface


interface MPI_File_write_all_begin

subroutine MPI_File_write_all_begin(fh, buf, count, datatype, ierror)
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: ierror
end subroutine MPI_File_write_all_begin

end interface

interface PMPI_File_write_all_begin

subroutine PMPI_File_write_all_begin(fh, buf, count, datatype, ierror)
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: ierror
end subroutine PMPI_File_write_all_begin

end interface


interface MPI_File_write_all_end

subroutine MPI_File_write_all_end(fh, buf, status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine MPI_File_write_all_end

end interface

interface PMPI_File_write_all_end

subroutine PMPI_File_write_all_end(fh, buf, status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine PMPI_File_write_all_end

end interface


interface MPI_File_write_at

subroutine MPI_File_write_at(fh, offset, buf, count, datatype, &
        status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine MPI_File_write_at

end interface

interface PMPI_File_write_at

subroutine PMPI_File_write_at(fh, offset, buf, count, datatype, &
        status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine PMPI_File_write_at

end interface


interface MPI_File_write_at_all

subroutine MPI_File_write_at_all(fh, offset, buf, count, datatype, &
        status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine MPI_File_write_at_all

end interface

interface PMPI_File_write_at_all

subroutine PMPI_File_write_at_all(fh, offset, buf, count, datatype, &
        status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine PMPI_File_write_at_all

end interface


interface MPI_File_write_at_all_begin

subroutine MPI_File_wr_at_all_begin(fh, offset, buf, count, datatype&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: ierror
end subroutine MPI_File_wr_at_all_begin

end interface

interface PMPI_File_write_at_all_begin

subroutine PMPI_File_wr_at_all_begin(fh, offset, buf, count, datatype&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
  integer(kind=MPI_OFFSET_KIND), intent(in) :: offset
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: ierror
end subroutine PMPI_File_wr_at_all_begin

end interface


interface MPI_File_write_at_all_end

subroutine MPI_File_write_at_all_end(fh, buf, status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine MPI_File_write_at_all_end

end interface

interface PMPI_File_write_at_all_end

subroutine PMPI_File_write_at_all_end(fh, buf, status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine PMPI_File_write_at_all_end

end interface


interface MPI_File_write_ordered

subroutine MPI_File_write_ordered(fh, buf, count, datatype, status&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine MPI_File_write_ordered

end interface

interface PMPI_File_write_ordered

subroutine PMPI_File_write_ordered(fh, buf, count, datatype, status&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine PMPI_File_write_ordered

end interface


interface MPI_File_write_ordered_begin

subroutine MPI_File_write_ordered_begin(fh, buf, count, datatype, ierror)
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: ierror
end subroutine MPI_File_write_ordered_begin

end interface

interface PMPI_File_write_ordered_begin

subroutine PMPI_File_write_ordered_begin(fh, buf, count, datatype, ierror)
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, intent(out) :: ierror
end subroutine PMPI_File_write_ordered_begin

end interface


interface MPI_File_write_ordered_end

subroutine MPI_File_write_ordered_end(fh, buf, status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine MPI_File_write_ordered_end

end interface

interface PMPI_File_write_ordered_end

subroutine PMPI_File_write_ordered_end(fh, buf, status, ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine PMPI_File_write_ordered_end

end interface


interface MPI_File_write_shared

subroutine MPI_File_write_shared(fh, buf, count, datatype, status&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine MPI_File_write_shared

end interface

interface PMPI_File_write_shared

subroutine PMPI_File_write_shared(fh, buf, count, datatype, status&
        , ierror)
  include 'mpif-config.h'
  integer, intent(in) :: fh
   buf
  , intent(in) :: buf
  integer, intent(in) :: count
  integer, intent(in) :: datatype
  integer, dimension(MPI_STATUS_SIZE), intent(out) :: status
  integer, intent(out) :: ierror
end subroutine PMPI_File_write_shared

end interface
