SYSTEMC_HOME    = /usr/local/systemc-2.3.3
TARGET_ARCH     = linux64

SYSTEMC_INC_DIR = $(SYSTEMC_HOME)/include
SYSTEMC_LIB_DIR = $(SYSTEMC_HOME)/lib-$(TARGET_ARCH)

FLAGS           = -g -Wall -pedantic -Wno-long-long \
                 -DSC_INCLUDE_DYNAMIC_PROCESSES -fpermissive \
                 -I$(SYSTEMC_INC_DIR) -Iinc
LDFLAGS         = -L$(SYSTEMC_LIB_DIR) -lsystemc -lm

SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)

DIR_FOR_BUILD = build

main:
	mkdir -p $(DIR_FOR_BUILD)
	g++ -o $(DIR_FOR_BUILD)/model $(LDFLAGS) $(FLAGS) $(SRCS)
