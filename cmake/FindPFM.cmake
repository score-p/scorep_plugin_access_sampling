# Try to find PFM headers and libraries.
#
# Usage:
#     find_package(PFM)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  PFM_PREFIX         Set this variable to the root installation of
#                     libpfm4 if the module has problems finding the
#                     proper installation path.
#
# Variables defined by this module:
#
#  PFM_FOUND              Found PFM libraries and headers
#  PFM_LIBRARIES          The PFM library
#  PFM_INCLUDE_DIRS       The location of PFM headers

find_path(PFM_PREFIX
    NAMES include/perfmon/pfmlib.h
          include/perfmon/pfmlib_perf_event.h)

find_library(PFM_LIBRARIES
    NAMES libpfm.so pfm
    HINTS ${PFM_PREFIX}/lib ${HILTIDEPS}/lib
)

find_path(PFM_INCLUDE_DIRS
    NAMES perfmon/pfmlib.h perfmon/pfmlib_perf_event.h
    HINTS ${PFM_PREFIX}/include ${HILTIDEPS}/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PFM DEFAULT_MSG
  PFM_LIBRARIES
  PFM_INCLUDE_DIRS
)

mark_as_advanced(
  PFM_PREFIX
  PFM_LIBRARIES
  PFM_INCLUDE_DIRS
)
