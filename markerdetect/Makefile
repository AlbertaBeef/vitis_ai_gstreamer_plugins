## Copyright 2019 Xilinx Inc.
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.

## NOTICE: This file has been modified from the original version.
##  The original file is https://github.com/Xilinx/Vitis-AI/blob/v1.1/mpsoc/vitis_ai_dnndk_samples/face_detection/Makefile
##  
##  TJS - Updated for GStreamer and Vitis-AI-Library support

PROJECT  = libgstmarkerdetect.so
CXX     ?= aarch64-linux-gnu-g++
CC      ?= aarch64-linux-gnu-gcc
CFLAGS  := -O2 -Wall -Wpointer-arith -Wno-unused-function -ffast-math -fPIC -shared
CFLAGS  += --sysroot=$(SYSROOT) 
CFLAGS  += -I$(SYSROOT)/usr/include/gstreamer-1.0 -I$(SYSROOT)/usr/lib/gstreamer-1.0/include
CFLAGS  += -I$(SYSROOT)/usr/include/glib-2.0 -I$(SYSROOT)/usr/lib/glib-2.0/include
CFLAGS  += -std=c++17
CFLAGS  += -I$(SYSROOT)/usr/include/opencv4
LDFLAGS := -lpthread -lrt -ldl -lcrypt -lstdc++ -lglog
LDFLAGS += -lgstbase-1.0 -lgstvideo-1.0 
LDFLAGS += -lopencv_core -lopencv_video -lopencv_videoio -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui -lopencv_ximgproc -lopencv_aruco 
#LDFLAGS += -lxilinxopencl -lvitis_ai_library-facedetect

CUR_DIR =   $(shell pwd)

BUILD    =   $(CUR_DIR)/build
C_DIR   :=   $(shell find $(SRC) -name *.c)
OBJ      =   $(patsubst %.c, %.o, $(notdir $(C_DIR)))
CPP_DIR :=   $(shell find $(SRC) -name *.cpp)
OBJ     +=   $(patsubst %.cpp, %.o, $(notdir $(CPP_DIR)))

CFLAGS +=  -mcpu=cortex-a53

SRC     =   $(CUR_DIR)

.PHONY: all clean 

all: $(BUILD) $(PROJECT) 
 
$(PROJECT) : $(OBJ) 
	$(CXX) $(CFLAGS) $(addprefix $(BUILD)/, $^) -o $@ $(LDFLAGS)
 
%.o : %.cpp
	$(CXX) -c $(CFLAGS) $< -o $(BUILD)/$@

clean:
	$(RM) -rf $(BUILD)
	$(RM) $(PROJECT) 

$(BUILD) : 
	-mkdir -p $@ 
