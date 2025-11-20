#include "fleet_service_impl.h"
#include <thread>
#include <chrono>
#include <random>
#include <iostream>
#include "google/protobuf/timestamp.pb.h"

FleetServiceImpl::FleetServiceImpl() {}

grpc::Status FleetServiceImpl::RegisterDevice(grpc::ServerContext*, const fleet::RegisterDeviceRequest* req, fleet::RegisterDeviceResponse* resp) 
{
    DeviceRecord rec;
    bool ok = _store.RegisterDevice(req->device_id(), rec);
    if (!ok) 
    {
        return grpc::Status(grpc::ALREADY_EXISTS, "Device already exists");
    }
    // fill proto Device
    fleet::Device *d = resp->mutable_device();
    d->set_device_id(rec.device_id);
    d->set_status(fleet::DeviceStatus::IDLE);
    auto ts = d->mutable_last_seen();
    google::protobuf::Timestamp now;
    now.set_seconds(std::chrono::duration_cast<std::chrono::seconds>(rec.last_seen.time_since_epoch()).count());
    *ts = now;
    return grpc::Status::OK;
}

grpc::Status FleetServiceImpl::SetDeviceStatus(grpc::ServerContext*, const fleet::SetDeviceStatusRequest* req, fleet::SetDeviceStatusResponse* resp) 
{
    DeviceStatus s = DeviceStatus::UNKNOWN;
    // map proto enum to internal enum
    switch (req->status()) 
    {
        case fleet::IDLE: 
            s = DeviceStatus::IDLE; 
            break;
        case fleet::OFFLINE: 
            s = DeviceStatus::OFFLINE; 
            break;
        default: 
            s = DeviceStatus::IDLE; 
            break;
    }

    DeviceRecord rec;
    if (!_store.SetDeviceStatus(req->device_id(), s, rec))
    {
        return grpc::Status(grpc::NOT_FOUND, "device not found");
    }
    fleet::Device *d = resp->mutable_device();
    d->set_device_id(rec.device_id);
    d->set_status(fleet::DeviceStatus::IDLE);
    google::protobuf::Timestamp now;
    now.set_seconds(std::chrono::duration_cast<std::chrono::seconds>(rec.last_seen.time_since_epoch()).count());
    *d->mutable_last_seen() = now;
    return grpc::Status::OK;
}

grpc::Status FleetServiceImpl::GetDeviceInfo(grpc::ServerContext*, const fleet::GetDeviceInfoRequest* req, fleet::GetDeviceInfoResponse* resp) 
{
    DeviceRecord rec;
    if (!_store.GetDeviceInfo(req->device_id(), rec)) 
    {
        return grpc::Status(grpc::NOT_FOUND, "device not found");
    }
    fleet::Device *d = resp->mutable_device();
    d->set_device_id(rec.device_id);
    // simple mapping: set proto status as IDLE or UPDATING etc.
    if (rec.device_status == DeviceStatus::UPDATING)
    {
        d->set_status(fleet::UPDATING);
    }
    else 
    {
        d->set_status(fleet::IDLE);
    }
    google::protobuf::Timestamp now;
    now.set_seconds(std::chrono::duration_cast<std::chrono::seconds>(rec.last_seen.time_since_epoch()).count());
    *d->mutable_last_seen() = now;
    d->set_metadata(rec.metadata);
    return grpc::Status::OK;
}

grpc::Status FleetServiceImpl::ListDevices(grpc::ServerContext*, const fleet::ListDevicesRequest*, fleet::ListDevicesResponse* resp) 
{
    auto list = _store.ListDevices();
    for (auto &rec : list) 
    {
        fleet::Device *d = resp->add_devices();
        d->set_device_id(rec.device_id);
        if (rec.device_status == DeviceStatus::UPDATING)
        { 
            d->set_status(fleet::UPDATING);
        }
        else 
        {
            d->set_status(fleet::IDLE);
        }
        google::protobuf::Timestamp ts;
        ts.set_seconds(std::chrono::duration_cast<std::chrono::seconds>(rec.last_seen.time_since_epoch()).count());
        *d->mutable_last_seen() = ts;
    }
    return grpc::Status::OK;
}

grpc::Status FleetServiceImpl::InitiateDeviceAction(grpc::ServerContext*, const fleet::InitiateDeviceActionRequest* req, fleet::InitiateDeviceActionResponse* resp) 
{
    // simple validate device exists
    DeviceRecord rec;
    if (!_store.GetDeviceInfo(req->device_id(), rec)) 
    {
        return grpc::Status(grpc::NOT_FOUND, "device not found");
    }
    // convert params
    std::map<std::string,std::string> params;
    for (const auto &kv : req->action_params())
    { 
        params[kv.first] = kv.second;
    }

    std::string action_id = _store.CreateAction(req->device_id(), static_cast<int>(req->action_type()), params);
    resp->set_action_id(action_id);

    // simulate asynchronously
    std::thread(&FleetServiceImpl::simulate_action_async, this, action_id).detach();

    return grpc::Status::OK;
}

grpc::Status FleetServiceImpl::GetDeviceActionStatus(grpc::ServerContext*, const fleet::GetDeviceActionStatusRequest* req, fleet::GetDeviceActionStatusResponse* resp) 
{
    ActionRecord r;
    if (!_store.GetAction(req->action_id(), r)) 
    {
        return grpc::Status(grpc::NOT_FOUND, "action not found");
    }
    auto *a = resp->mutable_action();
    a->set_action_id(r.action_id);
    a->set_device_id(r.device_id);
    a->set_action_type(static_cast<fleet::ActionType>(r.action_type));
    a->set_status(static_cast<fleet::ActionStatus>(r.action_status));
    a->set_status_message(r.status_message);
    google::protobuf::Timestamp started, finished;
    started.set_seconds(std::chrono::duration_cast<std::chrono::seconds>(r.started_at.time_since_epoch()).count());
    finished.set_seconds(std::chrono::duration_cast<std::chrono::seconds>(r.finished_at.time_since_epoch()).count());
    *a->mutable_started_at() = started;
    *a->mutable_finished_at() = finished;
    return grpc::Status::OK;
}

void FleetServiceImpl::simulate_action_async(const std::string &action_id) 
{
    // Mark RUNNING
    _store.UpdateActionStatus(action_id, 2, "running");

    // sleep random 10-20 seconds
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(10, 20);
    int s = dist(mt);
    std::this_thread::sleep_for(std::chrono::seconds(s));

    // Randomly succeed (80%) or fail (20%)
    std::uniform_int_distribution<int> p(1, 100);
    int pr = p(mt);
    if (pr <= 80) 
    {
        _store.UpdateActionStatus(action_id, 3, "completed");
    } 
    else 
    {
        _store.UpdateActionStatus(action_id, 4, "failed");
    }
}
