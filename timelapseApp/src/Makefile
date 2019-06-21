#Author: 

TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# The following gets rid of the -fno-implicit-templates flag on vxWorks, 
# so we get automatic template instantiation.
# This is what we want for the source files that define template functions.
ifeq (vxWorks,$(findstring vxWorks, $(T_A)))
	CODE_CXXFLAGS=
endif

DBD += NDPluginTimelapse.dbd

INC += NDPluginTimelapse.h

LIBRARY_IOC += NDPluginTimelapse

NDPluginTimelapse_SRCS += NDPluginTimelapse.cpp

ifdef OPENCV_INCLUDE
  USR_INCLUDES += $(addprefix -I, $(OPENCV_INCLUDE))
endif
ifdef OPENCV_LIB
  #NDPluginTimelapse_DIR += $(OPENCV_LIB)
  LIB_LIBS += C:\Users\micha_000\Downloads\opencv\build\lib\Release\opencv_core410
  LIB_LIBS += C:\Users\micha_000\Downloads\opencv\build\lib\Release\opencv_imgproc410
  LIB_LIBS += C:\Users\micha_000\Downloads\opencv\build\lib\Release\opencv_highgui410
  LIB_LIBS += C:\Users\micha_000\Downloads\opencv\build\lib\Release\opencv_video410
  LIB_LIBS += C:\Users\micha_000\Downloads\opencv\build\lib\Release\opencv_videoio410
endif

include $(AREA_DETECTOR)/ADCore/ADApp/commonLibraryMakefile

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE