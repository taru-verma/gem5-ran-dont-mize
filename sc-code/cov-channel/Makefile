######## Variables ########

CFLAGS  += -std=gnu99 -D_ISOC11_SOURCE=1 -O1 -Winline

CC	:= gcc
OUT := cov-channel-base-rep

######## Targets ########

all: $(OUT)

rebuild: clean all

clean:
	rm -rf *.o $(OUT)

$(OUT):
	$(CC) $(CFLAGS) -o $@ $(@:=.c)
