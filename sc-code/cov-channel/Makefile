######## Variables ########

CURR_PATH := $(realpath $(dir $(realpath $(firstword $(MAKEFILE_LIST)))))

ifeq ($(INST_PATH),)
    INST_PATH = $(realpath $(dir $(CURR_PATH)))
endif

CFLAGS  += -std=gnu99 -D_ISOC11_SOURCE=1 -O1 -Winline

## CacheSC
CFLAGS  += -I$(INST_PATH)/include
LDFLAGS += -L$(INST_PATH)/lib
LDLIBS  += -lcachesc -lm

CC	:= gcc
OUT := g5-cc-rep-send0 g5-cc-rep-send1

ifneq ($(NORMALIZE),)
    CFLAGS += -DNORMALIZE=$(NORMALIZE)
endif

######## Targets ########

all: $(OUT)

rebuild: clean all

clean:
	rm -rf *.o $(OUT)

$(OUT):
	$(CC) $(CFLAGS) -o $@ $(@:=.c) $(LDFLAGS) $(LDLIBS)
