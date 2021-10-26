# Makefile for SADP SEARCH TOOLS

DEUBG = 

TARGET1 = SearcherServer
TARGET2 = SearcherClent

OBJS_PATH = objs

CROSS_COMPILE =
#CXX   = $(CROSS_COMPILE)g++
CC    = $(CROSS_COMPILE)gcc
#STRIP = $(CROSS_COMPILE)strip

INC  = -I$(shell pwd)/
INC += -I$(shell pwd)/common/ 
INC += -I$(shell pwd)/rwqueue/ 
INC += -I$(shell pwd)/sadp/ 
INC += -I$(shell pwd)/sqlite/ 
INC += -I$(shell pwd)/tcpserver/ 
INC += -I$(shell pwd)/udpmulticast/ 

#LIB  = 

CFLAGS	  =  -g
CXX_FLAGS = 
LD_FLAGS  = -lrt -l sqlite3 -pthread -lpthread -ldl -lm $(DEBUG)

SRC1  = $(notdir $(wildcard ./rwqueue/*.c))
OBJS1 = $(patsubst %.c,$(OBJS_PATH)/%.o,$(SRC1))

SRC2  = $(notdir $(wildcard ./sadp/*.c))
OBJS2 = $(patsubst %.c,$(OBJS_PATH)/%.o,$(SRC2))

SRC3  = $(notdir $(wildcard ./sqlite/*.c))
OBJS3 = $(patsubst %.c,$(OBJS_PATH)/%.o,$(SRC3))

SRC4  = $(notdir $(wildcard ./tcpserver/*.c))
OBJS4 = $(patsubst %.c,$(OBJS_PATH)/%.o,$(SRC4))

SRC5  = $(notdir $(wildcard ./udpmulticast/*.c))
OBJS5 = $(patsubst %.c,$(OBJS_PATH)/%.o,$(SRC5))

SRC6  = $(notdir $(wildcard ./*.c))
OBJS6 = $(patsubst %.c,$(OBJS_PATH)/%.o,$(SRC6))

#SRC6  = $(notdir $(wildcard ./mainClent.c))
#OBJS6 = $(patsubst %.c,$(OBJS_PATH)/%.o,$(SRC4))

#all: BUILD_DIR $(TARGET1) $(TARGET2)
all: BUILD_DIR $(TARGET1) 

BUILD_DIR:
	@-mkdir -p $(OBJS_PATH)

$(TARGET1) : $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS5) $(OBJS6)
	$(CC) $^ -o $@ $(CFLAGS) $(LD_FLAGS) 

#$(TARGET2) : $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS6)
#	$(CC) $^ -o $@ $(CFLAGS) $(LD_FLAGS) 
    
$(OBJS_PATH)/%.o : ./rwqueue/%.c
	$(CC) -c  $< -o  $@ $(INC)
$(OBJS_PATH)/%.o : ./sadp/%.c
	$(CC) -c  $< -o  $@ $(INC)
$(OBJS_PATH)/%.o : ./sqlite/%.c
	$(CC) -c  $< -o  $@ $(INC)
$(OBJS_PATH)/%.o : ./tcpserver/%.c
	$(CC) -c  $< -o  $@ $(INC)
$(OBJS_PATH)/%.o : ./udpmulticast/%.c
	$(CC) -c  $< -o  $@ $(INC)	
$(OBJS_PATH)/%.o : ./%.c
	$(CC) -c  $< -o  $@ $(INC)	
	
clean:
	-rm -rf $(OBJS_PATH) $(TARGET1)
