# SuperLU lib usually requires linking to a blas library.
# It is up to the user of this module to find a BLAS and link to it.

# From eigen

if (SUPERLU_INCLUDE_DIR AND SUPERLU_LIBRARIES)
  set(SUPERLU_FIND_QUIETLY TRUE)
endif (SUPERLU_INCLUDE_DIR AND SUPERLU_LIBRARIES)

find_package(PkgConfig)
include(LibFindMacros)

# Use pkg-config to get hints about paths
pkg_check_modules(SUPERLU QUIET superlu)

IF(SUPERLU_FOUND)
  SET(SUPERLU_INCLUDE_DIR ${SUPERLU_INCLUDE_DIRS})
  IF(SUPERLU_LIBRARY_DIRS)
    SET(SUPERLU_LIBDIR ${SUPERLU_LIBRARY_DIRS})
  ELSE(SUPERLU_LIBRARY_DIRS)
    SET(SUPERLU_LIBDIR " ")
  ENDIF(SUPERLU_LIBRARY_DIRS)
ENDIF(SUPERLU_FOUND)

find_path(SUPERLU_INCLUDE_DIR
  NAMES
  slu_ddefs.h
  PATHS
  $ENV{SUPERLUDIR}
  PATH_SUFFIXES
  superlu
)

find_library(SUPERLU_LIBRARIES superlu PATHS $ENV{SUPERLUDIR})

if(SUPERLU_LIBRARIES)

  if (NOT SUPERLU_LIBDIR)
    get_filename_component(SUPERLU_LIBDIR ${SUPERLU_LIBRARIES} PATH)
  endif(NOT SUPERLU_LIBDIR)

  find_library(COLAMD_LIBRARY colamd PATHS ${SUPERLU_LIBDIR} $ENV{SUPERLUDIR})
  if (COLAMD_LIBRARY)
    set(SUPERLU_LIBRARIES ${SUPERLU_LIBRARIES} ${COLAMD_LIBRARY})
  endif (COLAMD_LIBRARY)

  find_library(AMD_LIBRARY amd PATHS ${SUPERLU_LIBDIR} $ENV{SUPERLUDIR})
  if (AMD_LIBRARY)
    set(SUPERLU_LIBRARIES ${SUPERLU_LIBRARIES} ${AMD_LIBRARY})
  endif (AMD_LIBRARY)

  find_library(SUITESPARSE_LIBRARY SuiteSparse PATHS ${SUPERLU_LIBDIR} $ENV{SUPERLUDIR})
  if (SUITESPARSE_LIBRARY)
    set(SUPERLU_LIBRARIES ${SUPERLU_LIBRARIES} ${SUITESPARSE_LIBRARY})
  endif (SUITESPARSE_LIBRARY)

endif(SUPERLU_LIBRARIES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SUPERLU DEFAULT_MSG
                                  SUPERLU_INCLUDE_DIR SUPERLU_LIBRARIES )

mark_as_advanced(SUPERLU_INCLUDE_DIR SUPERLU_LIBRARIES AMD_LIBRARY COLAMD_LIBRARY SUITESPARSE_LIBRARY)