#!/bin/bash
set -e

PROTO_DIR="../proto"
CPP_OUT="../backend/generated"
PY_OUT="../frontend"

mkdir -p $CPP_OUT $PY_OUT

# Generate C++ proto files
protoc -I=$PROTO_DIR \
        --cpp_out=$CPP_OUT \
        --grpc_out=$CPP_OUT \
        --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` \
        $PROTO_DIR/*.proto

# Generate Python proto files
python -m grpc_tools.protoc \
        -I=$PROTO_DIR \
        --python_out=$PY_OUT \
        --grpc_python_out=$PY_OUT \
        $PROTO_DIR/*.proto

echo "Proto files generated for C++ (server) and Python (CLI)"
