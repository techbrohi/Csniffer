## Building and Installation

### Installation:
```bash
# Clone and build
git clone https://github.com/yourusername/packet-sniffer
cd packet-sniffer

# Build library and sniffer
make

# Install library system-wide (requires sudo)
make install

# Run the sniffer
make run


## Important Security Note
This project includes a `.gitignore` to prevent accidental commit of:
- Compiled binaries
- **Packet capture files** (which may contain sensitive network data)
- Development environment files
