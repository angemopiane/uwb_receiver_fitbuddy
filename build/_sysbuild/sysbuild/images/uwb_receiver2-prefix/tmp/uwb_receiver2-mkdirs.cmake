# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/ncs/v2.9.1/uwb_receiver2")
  file(MAKE_DIRECTORY "C:/ncs/v2.9.1/uwb_receiver2")
endif()
file(MAKE_DIRECTORY
  "C:/ncs/v2.9.1/uwb_receiver2/build/uwb_receiver2"
  "C:/ncs/v2.9.1/uwb_receiver2/build/_sysbuild/sysbuild/images/uwb_receiver2-prefix"
  "C:/ncs/v2.9.1/uwb_receiver2/build/_sysbuild/sysbuild/images/uwb_receiver2-prefix/tmp"
  "C:/ncs/v2.9.1/uwb_receiver2/build/_sysbuild/sysbuild/images/uwb_receiver2-prefix/src/uwb_receiver2-stamp"
  "C:/ncs/v2.9.1/uwb_receiver2/build/_sysbuild/sysbuild/images/uwb_receiver2-prefix/src"
  "C:/ncs/v2.9.1/uwb_receiver2/build/_sysbuild/sysbuild/images/uwb_receiver2-prefix/src/uwb_receiver2-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/ncs/v2.9.1/uwb_receiver2/build/_sysbuild/sysbuild/images/uwb_receiver2-prefix/src/uwb_receiver2-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/ncs/v2.9.1/uwb_receiver2/build/_sysbuild/sysbuild/images/uwb_receiver2-prefix/src/uwb_receiver2-stamp${cfgdir}") # cfgdir has leading slash
endif()
