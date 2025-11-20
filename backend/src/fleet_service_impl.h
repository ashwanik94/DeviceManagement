#pragma once
#include "device_management.h"
#include <grpcpp/grpcpp.h>
#include "fleet.grpc.pb.h"
#include "fleet.pb.h"


class FleetServiceImpl final : public fleet::FleetManager::Service 
{
public:
    FleetServiceImpl();

    grpc::Status RegisterDevice(grpc::ServerContext* context, const fleet::RegisterDeviceRequest* req, fleet::RegisterDeviceResponse* resp) override;

    grpc::Status SetDeviceStatus(grpc::ServerContext* context, const fleet::SetDeviceStatusRequest* req, fleet::SetDeviceStatusResponse* resp) override;

    grpc::Status GetDeviceInfo(grpc::ServerContext* context, const fleet::GetDeviceInfoRequest* req, fleet::GetDeviceInfoResponse* resp) override;

    grpc::Status ListDevices(grpc::ServerContext* context, const fleet::ListDevicesRequest* req, fleet::ListDevicesResponse* resp) override;

    grpc::Status InitiateDeviceAction(grpc::ServerContext* context, const fleet::InitiateDeviceActionRequest* req, fleet::InitiateDeviceActionResponse* resp) override;

    grpc::Status GetDeviceActionStatus(grpc::ServerContext* context, const fleet::GetDeviceActionStatusRequest* req, fleet::GetDeviceActionStatusResponse* resp) override;

private:
    DeviceManagement _store;
    void simulate_action_async(const std::string &action_id);
};
