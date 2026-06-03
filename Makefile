CC       = gcc
CFLAGS   = -O3
LDFLAGS  = -lwayland-client
PROTOCOL = /usr/share/wayland-protocols/unstable/idle-inhibit/idle-inhibit-unstable-v1.xml

TARGET   = wayland-inhibit
SRC      = wayland-inhibit.c
PROTOCOL_H = idle-inhibit-unstable-v1-client-protocol.h
PROTOCOL_C = idle-inhibit-unstable-v1-protocol.c

PREFIX   = /usr/local

.PHONY: all clean install

.INTERMEDIATE: $(PROTOCOL_H) $(PROTOCOL_C)

all: $(TARGET)

$(PROTOCOL_H):
	wayland-scanner client-header $(PROTOCOL) $@

$(PROTOCOL_C):
	wayland-scanner private-code $(PROTOCOL) $@

$(TARGET): $(SRC) $(PROTOCOL_H) $(PROTOCOL_C)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(PROTOCOL_C) $(LDFLAGS)

clean:
	rm -f $(TARGET) $(PROTOCOL_H) $(PROTOCOL_C)

install: $(TARGET)
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/
	rm -f $(TARGET)
