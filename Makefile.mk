#==================================================================================================
# PROJECT: GEMMapper
# FILE: Makefile.mk
# DATE: 02/10/2012
# AUTHOR(S): Santiago Marco-Sola <santiagomsola@gmail.com>
# DESCRIPTION: Makefile definitions' file
#==================================================================================================

###############################################################################
# System
###############################################################################
PLATFORM=$(shell uname)

# Utilities
CC=gcc
AR=ar

###############################################################################
# Paths
###############################################################################

# Folders
FOLDER_BIN=$(ROOT_PATH)/bin
FOLDER_BUILD=$(ROOT_PATH)/build
FOLDER_DATASETS=$(ROOT_PATH)/datasets
FOLDER_INCLUDE=$(ROOT_PATH)/include
FOLDER_LIB=$(ROOT_PATH)/lib
FOLDER_RESOURCES=$(ROOT_PATH)/resources
FOLDER_RESOURCES_BUILD=$(ROOT_PATH)/resources/build

FOLDER_GEMGPU_BUILD=$(ROOT_PATH)/resources/gpu_modules/build

FOLDER_RESOURCES_INCLUDE=$(ROOT_PATH)/resources/include
FOLDER_SOURCE=$(ROOT_PATH)/src
FOLDER_TEST=$(ROOT_PATH)/test
FOLDER_TOOLS=$(ROOT_PATH)/tools

###############################################################################
# Configure flags
###############################################################################

HAVE_ZLIB = 1
HAVE_BZLIB = 1
HAVE_OPENMP = 1
HAVE_CUDA = 0
HAVE_LTO = 1
HAVE_GOLD = 1

###############################################################################
# General Flags
###############################################################################

# GEM Flags
LIB_GEM_CORE=$(FOLDER_LIB)/libgemcore_c.a
FLAGS_GEM_PROFILE=-DGEM_PROFILE
FLAGS_GEM_DEBUG=-DGEM_DEBUG
ifeq ($(VTUNE),true)
  FLAGS_GEM_VTUNE=-DGEM_VTUNE
endif

FLAGS_GCC_WARNINGS0=-Wcast-align \
                    -Wlogical-op \
                    -Wmaybe-uninitialized \
                    -Wshadow \
                    -Wstrict-aliasing=1 \
                    -Wtype-limits \
                    -Wuninitialized \
                    -Wunused
                    
FLAGS_GCC_WARNINGS1=-Wconversion \
                    -Wdouble-promotion \
                    -Wfloat-conversion \
                    -Wsign-compare \
                    -Wsign-conversion \
                    -Wcast-qual \
                    -Wunused-parameter
                   
FLAGS_GCC_WARNINGS2=-Waggregate-return \
                    -Wempty-body \
                    -Wfloat-equal \
                    -Wignored-qualifiers \
                    -Wimplicit-fallthrough \
                    -Wjump-misses-init \
                    -Wmissing-field-initializers \
                    -Wmissing-format-attribute \
                    -Wno-aggressive-loop-optimizations \
                    -Wpadded \
                    -Wredundant-decls \
                    -Wswitch-default \
                    -Wswitch-enum \
                    -Wunsafe-loop-optimizations \
                    -Wuseless-cast

FLAGS_GCC_WARNINGS3=-pedantic \
                    -Wextra \
                    -Wdangling-else \
                    -Wtautological-compare \
                    -Wduplicated-cond \
                    -Wnull-dereference \
                    -Wswitch-bool \
                    -Wswitch-unreachable \
                    -Wshift-overflow=2 \
                    -Wshift-negative-value \
                    -Wtraditional \
                    -Wtraditional-conversion 
                   
## Base flags (Generic Compiler)
FLAGS_GENERAL=-std=c99 -Wall # $(FLAGS_GCC_WARNINGS0)
FLAGS_SUPPRESS_CHECKS=-DNDEBUG
FLAGS_OPT=-O4
FLAGS_DEVEL=-g
FLAGS_DEBUG=-O0
FLAGS_PROFILE=-rdynamic

## GCC Compiler 
ifeq ($(CC),gcc)
  ifeq ($(HAVE_LTO),1)
    OPT_LTO=-flto
  endif
  ifeq ($(HAVE_GOLD),1)
    AR=gcc-ar
    OPT_AR=-fuse-linker-plugin
  endif
  FLAGS_OPT=-Ofast -march=native $(OPT_LTO)
  FLAGS_LINK=$(OPT_AR)
endif

## ICC Compiler
ifeq ($(CC),icc)
  FLAGS_OPT=-Ofast -msse4.2 -ipo
  FLAGS_DEBUG=-g
endif

###############################################################################
# Library Flags
###############################################################################

# Libs/Include Flags
PATH_INCLUDE=-I$(FOLDER_INCLUDE)
PATH_LIB=-L$(FOLDER_LIB)

# Link Libs
LIBS=-lpthread -lm $(LIBS_ZLIB) $(LIBS_BZLIB) # -lgemcore_c
ifeq ($(PLATFORM),Linux)
  LIBS+=-lrt
endif

###############################################################################
# System dependent flags
###############################################################################

# OpenMP
ifeq ($(HAVE_OPENMP),1)
  DEF_OPENMP=-DHAVE_OPENMP
  LIB_OPENMP+=-fopenmp
  LIBS+=$(LIB_OPENMP)
endif

# GZip
ifeq ($(HAVE_ZLIB),1)
  DEF_ZLIB=-DHAVE_ZLIB
  LIB_ZLIB+=-lz
  LIBS+=$(LIB_ZLIB)
endif

# BZip
ifeq ($(HAVE_BZLIB),1)
  DEF_BZLIB=-DHAVE_BZLIB
  LIB_BZLIB+=-lbz2
  LIBS+=$(LIB_BZLIB)
endif

###############################################################################
# CUDA
###############################################################################
ifeq ($(HAVE_CUDA),1)
  # Paths
  DEF_CUDA=-DHAVE_CUDA
  PATH_CUDA=/usr/local/cuda
  NVCC=$(PATH_CUDA)/bin/nvcc
  # CUDA Libs
  CUDA_PATH_INCLUDE+=-I$(PATH_CUDA)/include
  CUDA_PATH_LIB+=-L$(PATH_CUDA)/lib64
  CUDA_PROFILE_LIB=-lnvToolsExt
  CUDA_LIB=-lcuda -lcudart -lstdc++
endif

###############################################################################
# VTune
###############################################################################
ifeq ($(VTUNE),true)
# Path
VTUNE_PATH=/opt/intel/vtune_amplifier_xe/
ifeq ("$(wildcard $(VTUNE_PATH))","")
  VTUNE_PATH=/usr/local/software/intel/vtune_amplifier_xe_2016.3.0.463186/
endif
# Libs
VTUNE_PROFILE_LIB=-Wl,--no-as-needed -ldl
LIBITTNOTIFY=$(VTUNE_PATH)/lib64/libittnotify.a
ifeq ($(wildcard LIBITTNOTIFY),)
  VTUNE_PROFILE_LIB+=$(LIBITTNOTIFY)
endif
# Includes
VTUNE_PROFILE_INCLUDE=-I$(VTUNE_PATH)/include/
endif

