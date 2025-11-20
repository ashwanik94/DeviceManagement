#include "device_management.h"
#include <sstream>

bool DeviceManagement::RegisterDevice(const std::string &device_id, DeviceRecord &out)
{
    std::lock_guard<std::mutex> lock(_device_mtx);
    if(_devices.count(device_id))
    {
        return false;
    }

    DeviceRecord new_device;
    new_device.device_id = device_id;
    new_device.device_status = DeviceStatus::IDLE;
    new_device.last_seen = std::chrono::system_clock::now();
    _devices[device_id] = new_device;
    out = new_device;
    return true;
};

bool DeviceManagement::SetDeviceStatus(const std::string &device_id, DeviceStatus status, DeviceRecord &out)
{
    std::lock_guard<std::mutex> lock(_device_mtx);
    auto it = _devices.find(device_id);
    if (it == _devices.end())
    {
        return false;
    }

    auto& rec = it->second;
    rec.device_status = status;
    rec.last_seen = std::chrono::system_clock::now();
    out = rec;
    return true;
};

bool DeviceManagement::GetDeviceInfo(const std::string &device_id, DeviceRecord &out)
{
    std::lock_guard<std::mutex> lock(_device_mtx);
    auto it = _devices.find(device_id);
    if (it == _devices.end())
    {
        return false;
    }
    out = it->second;
    return true;
};

std::vector<DeviceRecord> DeviceManagement::ListDevices()
{
    std::lock_guard<std::mutex> lock(_device_mtx);
    std::vector<DeviceRecord> rec;
    rec.reserve(_devices.size()); 
    for(const auto &d : _devices)
    {
        rec.push_back(d.second);
    }
    return rec;
};

std::string DeviceManagement::make_action_id()
{
    auto now  = std::chrono::system_clock::now().time_since_epoch().count();
    std::ostringstream ss;
    ss << std::hex << now << "_" << ++_action_counter;
    return ss.str();
}

std::string DeviceManagement::CreateAction(const std::string &device_id, int action_type, const std::map<std::string, std::string> &params)
{
    std::lock_guard<std::mutex> lock(_device_mtx);
    ActionRecord action_r;
    action_r.action_id = make_action_id();
    action_r.device_id = device_id;
    action_r.action_type = action_type;
    action_r.action_status = 1;
    action_r.started_at = std::chrono::system_clock::now();
    action_r.params = params;

    _actions[action_r.action_id] = action_r;

    // also set device state to UPDATING (simple mapping)
    auto dit = _devices.find(device_id);
    if (dit != _devices.end()) 
    {
        dit->second.device_status = DeviceStatus::UPDATING;
        dit->second.last_seen = std::chrono::system_clock::now();
    }

    return action_r.action_id;
};

bool DeviceManagement::GetAction(const std::string &action_id, ActionRecord &out)
{
    std::lock_guard<std::mutex> lock(_device_mtx);
    auto it = _actions.find(action_id);
    if(it == _actions.end())
    {
        return false;
    }

    out = it->second;
    return true;
};

void DeviceManagement::UpdateActionStatus(const std::string &action_id, int status, const std::string &msg)
{
    std::lock_guard<std::mutex> lock(_device_mtx);
    auto it = _actions.find(action_id);
    if(it == _actions.end())
    {
        return;
    }

    auto& rec = it->second;
    rec.action_status = status;
    rec.status_message = msg;
    rec.finished_at = std::chrono::system_clock::now();

    // update device status back to IDLE or ERROR
    auto dit = _devices.find(it->second.device_id);
    if (dit != _devices.end()) 
    {
        if (status == 3) // COMPLETED
        { 
            dit->second.device_status = DeviceStatus::IDLE;
        } else 
        {
            dit->second.device_status = DeviceStatus::ERROR;
        }
        dit->second.last_seen = std::chrono::system_clock::now();
    }
};