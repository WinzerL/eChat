CC = gcc
CFLAGS = -Wall -Werror -pthread
SRCDIR = server client
OBJDIR = obj
BINDIR = target

SRCSERVER = $(wildcard server/*.c)
OBJSERVER = $(patsubst server/%.c,$(OBJDIR)/server/%.o,$(SRCSERVER))
TARGETSERVER = $(BINDIR)/server

SRCCLIENT = $(wildcard client/*.c)
OBJCLIENT = $(patsubst client/%.c,$(OBJDIR)/client/%.o,$(SRCCLIENT))
TARGETCLIENT = $(BINDIR)/client

.PHONY: all clean

all: $(TARGETSERVER) $(TARGETCLIENT)

$(TARGETSERVER): $(OBJSERVER)
	$(CC) $(CFLAGS) $^ -o $@ 

$(TARGETCLIENT): $(OBJCLIENT)
	$(CC) $(CFLAGS) $^ -o $@ 

$(OBJDIR)/server/%.o: server/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/client/%.o: client/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJDIR)/server/*.o $(OBJDIR)/client/*.o $(TARGETSERVER) $(TARGETCLIENT)
