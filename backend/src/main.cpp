#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include "fleet.pb.h"
#include "fleet.grpc.pb.h"
#include "fleet_service_impl.h"

int main(int argc, char** argv) {
    std::string server_address("0.0.0.0:50051");
    if (argc > 1) server_address = std::string("0.0.0.0:") + argv[1];

    grpc::reflection::InitProtoReflectionServerBuilderPlugin();

    FleetServiceImpl service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
    return 0;
}
