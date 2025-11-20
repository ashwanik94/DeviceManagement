#!/usr/bin/env python3

import argparse
import time
import grpc

import fleet_pb2 as pb
import fleet_pb2_grpc as rpc


# ---------------------------cle
# Helpers
# ---------------------------

def create_stub(addr="localhost:50051"):
    channel = grpc.insecure_channel(addr)
    return rpc.FleetManagerStub(channel)


# ---------------------------
# CLI Commands
# ---------------------------

def cmd_register(args):
    stub = create_stub(args.addr)

    req = pb.RegisterDeviceRequest(
        device_id=args.device_id,
        initial_status=pb.DeviceStatus.IDLE
    )

    resp = stub.RegisterDevice(req)
    print("Device registered:")
    print(resp.device)


def cmd_get_device(args):
    stub = create_stub(args.addr)

    req = pb.GetDeviceInfoRequest(device_id=args.device_id)
    resp = stub.GetDeviceInfo(req)

    print("Device info:")
    print(resp.device)


def cmd_list(args):
    """Optional: only works if you implement ListDevices() RPC."""
    stub = create_stub(args.addr)

    resp = stub.ListDevices(pb.ListDevicesRequest())
    print("Devices:")
    for d in resp.devices:
        print("-", d.device_id, d.status)


def cmd_update(args):
    stub = create_stub(args.addr)

    req = pb.InitiateDeviceActionRequest(
        device_id=args.device_id,
        action_type=pb.ActionType.SOFTWARE_UPDATE,
        action_params={"version": args.version}
    )

    resp = stub.InitiateDeviceAction(req)

    print(f"Action started. Action ID = {resp.action_id}")


def cmd_action_status(args):
    stub = create_stub(args.addr)

    req = pb.GetDeviceActionStatusRequest(action_id=args.action_id)
    resp = stub.GetDeviceActionStatus(req)
    print("Action status:", pb.ActionStatus.Name(resp.action.status))


def cmd_poll_action(args):
    """Poll until action finishes."""
    stub = create_stub(args.addr)

    while True:
        req = pb.GetDeviceActionStatusRequest(action_id=args.action_id)
        resp = stub.GetDeviceActionStatus(req)

        status = pb.ActionStatus.Name(resp.action.status)
        print("Status:", status)

        if status in ("COMPLETED", "FAILED"):
            break

        time.sleep(2)


# ---------------------------
# Argparse Setup
# ---------------------------

def build_parser():
    p = argparse.ArgumentParser(description="Fleet Manager CLI")
    p.add_argument("--addr", default="localhost:50051", help="Backend Fleet Server address (default: localhost:50051)")

    sub = p.add_subparsers(dest="cmd", required=True)

    # register device
    s = sub.add_parser("register", help="Register a new device")
    s.add_argument("device_id")
    s.set_defaults(func=cmd_register)

    # get device info
    s = sub.add_parser("info", help="Get device info")
    s.add_argument("device_id")
    s.set_defaults(func=cmd_get_device)

    # list devices
    s = sub.add_parser("list", help="List devices")
    s.set_defaults(func=cmd_list)

    # trigger software update
    s = sub.add_parser("update", help="Start software update on device")
    s.add_argument("device_id")
    s.add_argument("--version", required=True)
    s.set_defaults(func=cmd_update)

    # get action status
    s = sub.add_parser("status", help="Get action status")
    s.add_argument("action_id")
    s.set_defaults(func=cmd_action_status)

    # poll until complete
    s = sub.add_parser("poll", help="Poll action status until terminal")
    s.add_argument("action_id")
    s.set_defaults(func=cmd_poll_action)

    return p


def main():
    parser = build_parser()
    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
