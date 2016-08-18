#
# Copyright (c) 2015-2016 The Khronos Group Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and/or associated documentation files (the
# "Materials"), to deal in the Materials without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Materials, and to
# permit persons to whom the Materials are furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Materials.
#
# MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
# KHRONOS STANDARDS. THE UNMODIFIED, NORMATIVE VERSIONS OF KHRONOS
# SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT
#    https://www.khronos.org/registry/
#
# THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
#


# Make a bzip2 tarball containing just the sample implementation.

OPENVX_SAMPLE_DIR_NAMES = \
 cmake_utils concerto debug examples helper include kernels libraries raw sample scripts tools \
 Android.mk BUILD_DEFINES Build.py CMakeLists.txt LICENSE Makefile NEWKERNEL.txt README VERSION

OPENVX_SAMPLE_DIRS = $(patsubst %, %, $(OPENVX_SAMPLE_DIR_NAMES))

# This is just the default destination, expected to be overridden.
OPENVX_SAMPLE_DESTDIR = $(HOST_ROOT)/out

OPENVX_SAMPLE_PACKAGE_NAME = \
 openvx_sample_$(VERSION)-$(strip $(shell date '+%Y%m%d')).tar.bz2
OPENVX_SAMPLE_PACKAGE_TOPDIR_NAME = openvx_sample

# For OPENVX_SAMPLE_PACKAGE_TOPDIR_NAME to work, we need
# someplace to put a symbolic link when packaging, as tar does
# not have an option to set a "virtual" top directory name.
OPENVX_SAMPLE_TMPDIR = $(HOST_ROOT)/out/packagetmp

# Only do this for the same host environments where the concerto
# "tar" package type is available.
ifneq ($(filter $(HOST_OS),LINUX CYGWIN DARWIN),)

.PHONY: openvx_sample_package
openvx_sample_package: \
 $(OPENVX_SAMPLE_DESTDIR)/$(OPENVX_SAMPLE_PACKAGE_NAME)

# The exclusion of .svn directories is there for the benefit of
# svn clients before 1.7, where there's a .svn directory in
# every directory in the working copy.
$(OPENVX_SAMPLE_DESTDIR)/$(OPENVX_SAMPLE_PACKAGE_NAME): \
 $(patsubst %, $(HOST_ROOT)/%, $(OPENVX_SAMPLE_DIRS))
	@$(MKDIR) $(OPENVX_SAMPLE_DESTDIR)
	@$(MKDIR) $(OPENVX_SAMPLE_TMPDIR)
	@$(LINK) $(HOST_ROOT) $(OPENVX_SAMPLE_TMPDIR)/$(OPENVX_SAMPLE_PACKAGE_TOPDIR_NAME)
	@tar --exclude=.svn -cjf $@ -C $(OPENVX_SAMPLE_TMPDIR) \
	 $(patsubst %, $(OPENVX_SAMPLE_PACKAGE_TOPDIR_NAME)/%, $(OPENVX_SAMPLE_DIRS))
	@$(CLEANDIR) $(OPENVX_SAMPLE_TMPDIR)

endif # Host environments.
