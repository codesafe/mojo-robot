#---------------------------------------------------------------------
#	ROBOT for Raspberry PI
#---------------------------------------------------------------------

TARGET      = robotrun

# important directories used by assorted rules and other variables
DIR_DXL    = ../..
DIR_OBJS   = .objects

# compiler options
CC			= arm-linux-gnueabihf-gcc
CX          = arm-linux-gnueabihf-g++

# dump core for debug
# ulimit -c
# ulimit -c unlimited
# gdb -c core ./robotrun
#CCFLAGS     = -O2 -O3 -DLINUX -D_GNU_SOURCE -Wall $(INCLUDES) $(FORMAT) -g -ggdb -std=gnu++11
#CXFLAGS     = -O2 -O3 -DLINUX -D_GNU_SOURCE -Wall $(INCLUDES) $(FORMAT) -g -ggdb -std=gnu++11

CCFLAGS     = -O2 -O3 -DLINUX -D_GNU_SOURCE -Wall $(INCLUDES) $(FORMAT) -g -std=gnu++11
CXFLAGS     = -O2 -O3 -DLINUX -D_GNU_SOURCE -Wall $(INCLUDES) $(FORMAT) -g -std=gnu++11
LNKCC       = $(CX)
LNKFLAGS    = $(CXFLAGS) #-Wl,-rpath,$(DIR_THOR)/lib


#---------------------------------------------------------------------
# Core components (all of these are likely going to be needed)
#---------------------------------------------------------------------
INCLUDES   += -I$(DIR_DXL)/sdk/include
LIBRARIES  += -ldxl_x86_cpp
LIBRARIES  += -lrt
LIBRARIES  += -lcurl
LIBRARIES  += -lpthread

#---------------------------------------------------------------------
# Files
#---------------------------------------------------------------------
SOURCES = robotrun.cpp \
			animation.cpp \
			device.cpp \
			display.cpp \
			joint.cpp \
			logger.cpp \
			memdb.cpp \
			network.cpp \
			part.cpp \
			socket.cpp \
			timer.cpp \
			wheel.cpp \
			xmlparser.cpp \
			commander.cpp \
			patch.cpp \

 # *** OTHER SOURCES GO HERE ***

OBJECTS  = $(addsuffix .o,$(addprefix $(DIR_OBJS)/,$(basename $(notdir $(SOURCES)))))
#OBJETCS += *** ADDITIONAL STATIC LIBRARIES GO HERE ***


#---------------------------------------------------------------------
# Compiling Rules
#---------------------------------------------------------------------
$(TARGET): make_directory $(OBJECTS)
	$(LNKCC) $(LNKFLAGS) $(OBJECTS) -o $(TARGET) $(LIBRARIES)

all: $(TARGET)

clean:
	rm -rf $(TARGET) $(DIR_OBJS) core *~ *.a *.so *.lo

make_directory:
	mkdir -p $(DIR_OBJS)/

$(DIR_OBJS)/%.o: ../%.c
	$(CC) $(CCFLAGS) -c $? -o $@

$(DIR_OBJS)/%.o: ../%.cpp
	$(CX) $(CXFLAGS) -c $? -o $@

#---------------------------------------------------------------------
# End of Makefile
#---------------------------------------------------------------------
