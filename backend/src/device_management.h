#pragma once
#include <string>
#include <chrono>
#include <map>
#include <unordered_map>
#include <mutex>


enum class DeviceStatus
{
    UNKNOWN,
    IDLE,
    BUSY,
    OFFLINE,
    MAINTENANCE,
    UPDATING,
    RECOVERING,
    ERROR
};

struct DeviceRecord
{
    std::string device_id;
    DeviceStatus device_status;
    std::string metadata;
    std::chrono::system_clock::time_point last_seen;
};

struct ActionRecord
{
    std::string action_id;
    std::string device_id;
    int action_type;
    int action_status;
    std::string status_message;
    std::chrono::system_clock::time_point started_at;
    std::chrono::system_clock::time_point finished_at;
    std::map<std::string, std::string> params;
};

class DeviceManagement
{
public:
    bool RegisterDevice(const std::string &device_id, DeviceRecord &out);
    bool SetDeviceStatus(const std::string &device_id, DeviceStatus status, DeviceRecord &out);
    bool GetDeviceInfo(const std::string &device_id, DeviceRecord &out);
    std::vector<DeviceRecord> ListDevices();

    std::string CreateAction(const std::string &device_id, int action_type, const std::map<std::string, std::string> &params);
    bool GetAction(const std::string &action_id, ActionRecord &out);
    void UpdateActionStatus(const std::string &action_id, int status, const std::string &msg);

private:
    std::unordered_map<std::string, DeviceRecord> _devices;
    std::unordered_map<std::string, ActionRecord> _actions;
    std::mutex _device_mtx;
    uint64_t _action_counter{0};
    std::string make_action_id();
};
