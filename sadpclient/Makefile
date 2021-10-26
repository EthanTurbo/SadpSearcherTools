# Makefile for SADP SEARCH TOOLS

DEUBG = 

TARGET = SearcherClient

OBJS_PATH = objs

CROSS_COMPILE =
#CXX   = $(CROSS_COMPILE)g++
CC    = $(CROSS_COMPILE)gcc
#STRIP = $(CROSS_COMPILE)strip

INC  = -I$(shell pwd)/
INC += -I$(shell pwd)/common/ 
INC += -I$(shell pwd)/tcpclient/  
INC += -I$(shell pwd)/shell/ 

#LIB  = 

CFLAGS	  =  -g
CXX_FLAGS = 
LD_FLAGS  = -lrt -pthread -lpthread -ldl -lm $(DEBUG)

SRC1  = $(notdir $(wildcard ./tcpclient/*.c))
OBJS1 = $(patsubst %.c,$(OBJS_PATH)/%.o,$(SRC1))

SRC2  = $(notdir $(wildcard ./shell/*.c))
OBJS2 = $(patsubst %.c,$(OBJS_PATH)/%.o,$(SRC2))

SRC3  = $(notdir $(wildcard ./*.c))
OBJS3 = $(patsubst %.c,$(OBJS_PATH)/%.o,$(SRC3))


all: BUILD_DIR $(TARGET) 

BUILD_DIR:
	@-mkdir -p $(OBJS_PATH)

$(TARGET) : $(OBJS1) $(OBJS2) $(OBJS3) 
	$(CC) $^ -o $@ $(CFLAGS) $(LD_FLAGS) 

    
$(OBJS_PATH)/%.o : ./tcpclient/%.c
	$(CC) -c  $< -o  $@ $(INC)
$(OBJS_PATH)/%.o : ./shell/%.c
	$(CC) -c  $< -o  $@ $(INC)	
$(OBJS_PATH)/%.o : ./%.c
	$(CC) -c  $< -o  $@ $(INC)	
	
clean:
	-rm -rf $(OBJS_PATH) $(TARGET)
