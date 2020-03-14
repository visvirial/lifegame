CFLAGS  = -ggdb -W -Wall -O2 -std=gnu99
LDFLAGS = 
LDLIBS  = 

TARGET = lifegame
OBJS = $(TARGET).o

.PHONY: all clean

all: $(TARGET)

clean:
	$(RM) $(TARGET) *.o

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

