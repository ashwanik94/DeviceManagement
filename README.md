**📦 DeviceManagement**

DeviceManagement is a cross-platform project that implements a gRPC-based device management backend in C++, along with a Python CLI for interacting with the server.
Protocol buffer files define all APIs, and the project includes automation scripts to generate code and build/run the server.

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
    ./gen_proto.sh   # Generate Proto Files for c++ and python
    ./run_server.sh & # Build & Run the Server in background

**🐍 Python CLI Usage**

    cd frontend
    python3 cli.py register DEVICE_ID
    python3 cli.py status SOME_ID
    python3 cli.py update DEVICE_ID --version VERSION   
    python3 cli.py list

**📡 gRPC API Overview**

All API definitions are located under proto/fleet.proto.
gen_proto.sh generates the stub files for both C++ and Python.
You can also generate them manually by using the appropriate commands.

    RegisterDevice
    SetDeviceStatus
    ListDevices
    CreateAction
    Messages for devices, actions, and statuses

**🔧 CMake Notes (Cross-Platform)**

The project supports both macOS (Homebrew) and Linux.
CMake scripts auto-detect paths when possible, and can be customized or overridden for other platforms.

**🛣️ Roadmap**
  - Docker container support
  - Add authentication (JWT/TLS)
  - PostgreSQL / Redis storage
  - More CLI commands
  - Device event streaming

**📄 License**

This project is licensed under the MIT License.






  
  
