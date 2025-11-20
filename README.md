**📦 DeviceManagement**

DeviceManagement is a cross-platform project implementing a gRPC-based device management backend in C++, along with a Python CLI for interacting with the server.
Proto files define all APIs, and the project includes automation scripts to generate code and build/run the server.

**🚀 Features**
  - gRPC backend for device management
  - C++17 server implementation
  - Python CLI client
  - Auto-generation of gRPC stubs via scripts
  - Clean CMake-based build setup
  - Modular and extendable architecture

**🔧 Requirements**
  - CMake ≥ 3.16
  - gRPC (>=1.5x)
  - Protocol Buffers (protoc)
  - C++17 compiler
  - Python 3.8+

**🛠️ Building & Running**

    cd scripts
    ./gen_protos.sh # Generate Proto Files for c++ and python
    ./run_server.sh # Build & Run the Server

**🐍 Python CLI Usage**

    cd frontend
    python cli.py register DEVICE_ID
    python cli.py status SOME_ID
    python cli.py update DEVICE_ID --version VERSION   
    python cli.py list

**📡 gRPC API Overview**

All API definitions live under proto/fleet.proto. **gen_protos.sh** generates the proto file for both cpp and pyhton. It can be generated manually by using correct command.
It Includes:

    RegisterDevice
    SetDeviceStatus
    ListDevices
    CreateAction
    Messages for devices, actions, and statuses

**🔧 CMake Notes (Cross-Platform)**

The project supports both macOS (Homebrew) and Linux.

The CMake scripts auto-detect paths when possible, but can be added/override for other platform.

**🛣️ Roadmap**
  - Docker container support
  - Add authentication (JWT/TLS)
  - PostgreSQL / Redis storage
  - More CLI commands
  - Device event streaming

**📄 License**

This project is licensed under the MIT License.






  
  
