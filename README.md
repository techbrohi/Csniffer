# Disclaimer:
This tool is for Educational and Study Purposes only. It can be used by students looking to understand networking deeply or for other legal and ethical purposes. The owner is not responsible for any act of malice and digital harm done with this tool.


## Building and Installation

### Installation:
```bash
# Clone and build
git clone https://github.com/techbrohi/Csniffer
cd Csniffer

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
