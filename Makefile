CC = gcc
CFLAGS = -Wall -Wextra -I./include
TARGET = sniffer
LIBRARY = libipheader.so
PREFIX = /usr/local

all: $(LIBRARY) $(TARGET)

# Build the shared library
$(LIBRARY): src/libipheader.c includes/packet_parser.h
	@echo "Building shared library..."
	$(CC) -shared -fPIC src/libipheader.c -o $(LIBRARY)

# Build the sniffer (using system-installed library)
$(TARGET): src/main.c
	@echo "Building packet sniffer..."
	$(CC) $(CFLAGS) src/main.c -o $(TARGET) -lipheader

# Build sniffer using local library (for testing)
local-build: $(LIBRARY)
	@echo "Building with local library..."
	$(CC) $(CFLAGS) src/main.c -L. -lipheader -o $(TARGET)

# Install library system-wide
install: $(LIBRARY)
	@echo "Installing $(LIBRARY) to $(PREFIX)/lib..."
	sudo cp $(LIBRARY) $(PREFIX)/lib/
	sudo ldconfig
	@echo "Installation complete! Run 'sudo ldconfig' if linker errors persist."

# Test installation
test-install: install
	@echo "Testing installation..."
	$(CC) $(CFLAGS) src/main.c -o test-sniffer -lipheader
	@echo "Compilation successful if no errors above."

# Run with sudo (required for raw sockets)
run: $(TARGET)
	sudo ./$(TARGET)
# Run with local library
run-local: local-build
	sudo LD_LIBRARY_PATH=. ./$(TARGET)

# Clean build artifacts
clean:
	rm -f $(TARGET) $(LIBRARY) test-sniffer *.o
	@echo "Cleaned build files."

# Uninstall library
uninstall:
	sudo rm -f $(PREFIX)/lib/$(LIBRARY)
	sudo ldconfig
	@echo "Library uninstalled."

.PHONY: all install clean uninstall run run-local local-build test-install
