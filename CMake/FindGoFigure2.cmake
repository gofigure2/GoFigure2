# - Find an GOFIGURE2 installation or build tree.

# When GOFIGURE2 is found, the GOFIGURE2Config.cmake file is sourced to setup the
# location and configuration of GOFIGURE2.  Please read this file, or
# GOFIGURE2Config.cmake.in from the GOFIGURE2 source tree for the full list of
# definitions.  Of particular interest is GOFIGURE2_USE_FILE, a CMake source file
# that can be included to set the include directories, library directories,
# and preprocessor macros.  In addition to the variables read from
# GOFIGURE2Config.cmake, this find module also defines
#  GOFIGURE2_DIR  - The directory containing GOFIGURE2Config.cmake.  
#             This is either the root of the build tree, 
#             or the lib/gofigure2 directory.  
#             This is the only cache entry.
#   
#  GOFIGURE2_FOUND - Whether GOFIGURE2 was found.  If this is true, 
#              GOFIGURE2_DIR is okay.
#
#  USE_GOFIGURE2_FILE - The full path to the UseGOFIGURE2.cmake file.  
#                 This is provided for backward 
#                 compatability.  Use GOFIGURE2_USE_FILE
#                 instead.

set(GOFIGURE2_DIR_STRING "directory containing GOFIGURE2Config.cmake.  This is either the root of the build tree, or PREFIX/lib/gofigure2 for an installation.")

# Search only if the location is not already known.
if(NOT GOFIGURE2_DIR)
  # Get the system search path as a list.
  if(UNIX)
    string(REGEX MATCHALL "[^:]+" GOFIGURE2_DIR_SEARCH1 "$ENV{PATH}")
  else(UNIX)
    string(REGEX REPLACE "\\\\" "/" GOFIGURE2_DIR_SEARCH1 "$ENV{PATH}")
  endif(UNIX)
  string(REGEX REPLACE "/;" ";" GOFIGURE2_DIR_SEARCH2 ${GOFIGURE2_DIR_SEARCH1})

  # Construct a set of paths relative to the system search path.
  set(GOFIGURE2_DIR_SEARCH "")
  foreach(dir ${GOFIGURE2_DIR_SEARCH2})
    set(GOFIGURE2_DIR_SEARCH ${GOFIGURE2_DIR_SEARCH} "${dir}/../lib/gofigure2")
  endforeach(dir)

  #
  # Look for an installation or build tree.
  #
  find_path(GOFIGURE2_DIR GOFIGURE2Config.cmake
    # Look for an environment variable GOFIGURE2_DIR.
    $ENV{GOFIGURE2_DIR}

    # Look in places relative to the system executable search path.
    ${GOFIGURE2_DIR_SEARCH}

    # Look in standard UNIX install locations.
    /usr/local/lib/gofigure2
    /usr/lib/gofigure2

    # Read from the CMakeSetup registry entries.  It is likely that
    # GOFIGURE2 will have been recently built.
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild1]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild2]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild3]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild4]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild5]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild6]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild7]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild8]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild9]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild10]

    # Help the user find it if we cannot.
    DOC "The ${GOFIGURE2_DIR_STRING}"
  )
endif(NOT GOFIGURE2_DIR)

# If GOFIGURE2 was found, load the configuration file to get the rest of the
# settings.
if(GOFIGURE2_DIR)
  set(GOFIGURE2_FOUND TRUE)
  include(${GOFIGURE2_DIR}/GOFIGURE2Config.cmake)

  # Set USE_GOFIGURE2_FILE for backward-compatability.
  set(USE_GOFIGURE2_FILE ${GOFIGURE2_USE_FILE})
else(GOFIGURE2_DIR)
  set(GOFIGURE2_FOUND FALSE)
  if(GOFIGURE2_FIND_REQUIRED)
    message(FATAL_ERROR "Please set GOFIGURE2_DIR to the ${GOFIGURE2_DIR_STRING}")
  endif(GOFIGURE2_FIND_REQUIRED)
endif(GOFIGURE2_DIR)
