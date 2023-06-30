import threading
import socket
import time
import json
import numpy as np
from enum import Enum
from math import *
import struct


# Initialize the actuator state
class AxisState(Enum):
    AXIS_STATE_UNDEFINED = 0
    AXIS_STATE_IDLE = 1
    AXIS_STATE_STARTUP_SEQUENCE = 2
    AXIS_STATE_MOTOR_CALIBRATION = 4
    AXIS_STATE_ENCODER_OFFSET_CALIBRATION = 7
    AXIS_STATE_ENABLE = 8


# Actuator control mode
class ControlMode(Enum):
    VOLTAGE_CONTROL = 0
    CURRENT_CONTROL = 1
    VELOCITY_CONTROL = 2
    POSITION_CONTROL = 3
    TRAJECTORY_CONTROL = 4


start_time = 0
stop_time = 0

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.settimeout(2)
s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

PORT_rt = 2333  # Real-time control data port, ie. speed, position, current and other real-time data
PORT_srv = 2334  # Low priority service data port. ie, parameter setting and reading
PORT_pt = 10000  # Passthrough port

# s.bind(('', PORT_srv))

# network = '10.0.0.255'
# network = '255.255.255.255'
network = '192.168.31.255'


# AIOS enable
# Parameters: including device IP and motor number
# Each AIOS can control two motors, M0 and M1
def enable(server_ip, motor_number):
    data = {
        'method': 'SET',
        'reqTarget': '/m0/requested_state',
        'property': AxisState.AXIS_STATE_ENABLE.value
    }
    if motor_number == 0:
        data['reqTarget'] = '/m0/requested_state'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/requested_state'
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")
        return False

    if json_obj.get('status') == 'OK':
        return True
    else:
        print("Recv Data Error !")
        return False


# AIOS Disable
# Parameters: including device IP and motor number
# Each AIOS can control two motors, M0 and M1
def disable(server_ip, motor_number):
    data = {
        'method': 'SET',
        'reqTarget': '/m0/requested_state',
        'property': AxisState.AXIS_STATE_IDLE.value
    }
    if motor_number == 0:
        data['reqTarget'] = '/m0/requested_state'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/requested_state'
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))

    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS Get current status
# Parameters: including device IP
# Get AIOS Get current status
def getState(server_ip, motor_number):
    data = {
        'method': 'GET',
        'reqTarget': '/m0/requested_state',
    }
    if motor_number == 0:
        data['reqTarget'] = '/m0/requested_state'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/requested_state'
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))

    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS Get root attributes
# Parameters: including device IP
# Get all basic attributes of AIOS, including serial number, bus voltage, motor temperature, inverter temperature, version number
def getRoot(server_ip):
    data = {
        'method': 'GET',
        'reqTarget': '/',
    }
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS Get Root Config property
# Parameters: including device IP
# Get AIOS bus voltage over-voltage and under-voltage protection threshold
def getRootConfig(server_ip):
    data = {
        'method': 'GET',
        'reqTarget': '/config',
    }
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS set Root Config properties
# Parameter: The protection threshold of bus voltage overvoltage and undervoltage
# Return success or failure
def setRootConfig(dict, server_ip):
    data = {
        'method': 'SET',
        'reqTarget': '/config',
        'dc_bus_overvoltage_trip_level': 30,
        'dc_bus_undervoltage_trip_level': 10,
    }
    data['dc_bus_overvoltage_trip_level'] = dict['dc_bus_overvoltage_trip_level']
    data['dc_bus_undervoltage_trip_level'] = dict['dc_bus_undervoltage_trip_level']
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS save configuration
# Parameters: including device IP
def saveConfig(server_ip):
    data = {
        'method': 'SET',
        'reqTarget': '/',
        'property': 'save_config'
    }
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS clear configuration
# Parameters: including device IP
def eraseConfig(server_ip):
    data = {
        'method': 'SET',
        'reqTarget': '/',
        'property': 'erase_config'
    }
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS restart
# Parameters: including device IP
def reboot(server_ip):
    data = {
        'method': 'SET',
        'reqTarget': '/',
        'property': 'reboot'
    }
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS restart the motor drive
# Parameters: including device IP
def rebootMotorDrive(server_ip):
    data = {
        'method': 'SET',
        'reqTarget': '/',
        'property': 'reboot_motor_drive'
    }
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS OTA upgrade
# Parameters: including device IP
def OTAupdate(server_ip):
    data = {
        'method': 'SET',
        'reqTarget': '/',
        'property': 'OTA_update'
    }
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS Get error code
# Parameters: including server IP
def getError(server_ip, motor_number):
    data = {
        'method': 'GET',
        'reqTarget': '/m0/error',
    }
    if motor_number == 0:
        data['reqTarget'] = '/m0/error'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/error'
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS Remove error
# Parameters: including server IP
def clearError(server_ip, motor_number):
    data = {
        'method': 'SET',
        'reqTarget': '/m0/error',
        'clear_error': True
    }
    if motor_number == 0:
        data['reqTarget'] = '/m0/error'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/error'
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS Get actuator position, speed, current
# Parameters: including server ip，motor number
# Return position, speed, current in tuple
def getCVP(server_ip, motor_number):
    data = {
        'method': 'GET',
        'reqTarget': '/m0/CVP',
    }
    if motor_number == 0:
        data['reqTarget'] = '/m0/CVP'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/CVP'
    json_str = json.dumps(data)
    # print ("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_rt))
    try:
        data, address = s.recvfrom(1024)
        # print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
        if json_obj.get('status') == 'OK':
            return json_obj.get('position'), json_obj.get('velocity'), json_obj.get('current')
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS 编码器校准 (初期版本电机编码器需要先校准才能使用，后期完善后不需要此操作)
# 参数：包括设备IP 电机号
# 无返回
def encoderOffsetCalibration(server_ip, motor_number):
    data = {
        'method': 'SET',
        'reqTarget': '/m0/requested_state',
        'property': AxisState.AXIS_STATE_ENCODER_OFFSET_CALIBRATION.value
    }
    if motor_number == 0:
        data['reqTarget'] = '/m0/requested_state'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/requested_state'
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS test if the encoder is ready or not (If not ready, perform encoder calibration)
# Parameters: including server ip，motor number
# no return code
def encoderIsReady(server_ip, motor_number):
    data = {
        'method': 'GET',
        'reqTarget': '/m0/encoder/is_ready',
    }
    if motor_number == 0:
        data['reqTarget'] = '/m0/encoder/is_ready'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/encoder/is_ready'
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive data! [Timeout]")

    if json_obj.get('status') == 'OK':
        return json_obj.get('property')
    else:
        print("Recv Data Error !")


# AIOS set control mode
# Parameters: including server ip，motor number
# no return code
def controlMode(ctrlMode, server_ip, motor_number):
    data = {
        'method': 'SET',
        'reqTarget': '/m0/controller/config',
        'control_mode': 3
    }
    if motor_number == 0:
        data['reqTarget'] = '/m0/controller/config'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/controller/config'

    data['control_mode'] = ctrlMode
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS set input mode
# Parameters: including server ip，motor number
# no return code
def inputMode(inputMode, server_ip, motor_number):
    data = {
        'method': 'SET',
        'reqTarget': '/m0/controller/config',
        'input_mode': 1
    }
    if motor_number == 0:
        data['reqTarget'] = '/m0/controller/config'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/controller/config'

    data['input_mode'] = inputMode
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS reset linear count
# Parameters: including server ip，motor number
# no return code
def setLinearCount(set_linear_count, server_ip, motor_number):
    data = {
        'method': 'SET',
        'reqTarget': '/m0/encoder',
        'set_linear_count': 0
    }
    if motor_number == 0:
        data['reqTarget'] = '/m0/encoder'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/encoder'

    data['set_linear_count'] = set_linear_count
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS Get actuator PID controller parameters
# Parameters: including server ip，motor number
# Return to control mode in tuples Position proportional gain Speed proportional gain Speed integral gain Speed limit Speed limit tolerance
def getMotionCtrlConfig(server_ip, motor_number):
    data = {
        'method': 'GET',
        'reqTarget': '/m0/controller/config',
    }
    if motor_number == 0:
        data['reqTarget'] = '/m0/controller/config'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/controller/config'
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
        if json_obj.get('status') == 'OK':
            return json_obj.get('control_mode'), json_obj.get('pos_gain'), json_obj.get('vel_gain'), json_obj.get(
                'vel_integrator_gain'), json_obj.get('vel_limit'), json_obj.get('vel_limit_tolerance')
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS Get actuator PID controller parameters
# parameter: Position proportional gain Speed proportional gain Speed integral gain Speed limit Speed limit tolerance
# return success or fail
def setMotionCtrlConfig(dict, server_ip, motor_number):
    data = {
        'method': 'SET',
        'reqTarget': '/m0/controller/config',
        'pos_gain': 20,
        'vel_gain': 0.0005,
        'vel_integrator_gain': 0.0002,
        'vel_limit': 40000,
        'vel_limit_tolerance': 1.2,
    }
    if motor_number == 0:
        data['reqTarget'] = '/m0/controller/config'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/controller/config'
    data['pos_gain'] = dict['pos_gain']
    data['vel_gain'] = dict['vel_gain']
    data['vel_integrator_gain'] = dict['vel_integrator_gain']
    data['vel_limit'] = dict['vel_limit']
    data['vel_limit_tolerance'] = dict['vel_limit_tolerance']
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS get actuator MotorConfig parameters
# parameter: server ip, motor number
# return Parameters: motor current limit, current limit margin, inverter temperature offline, inverter temperature upper limit, current control bandwidth, including device IP, motor number as a tuple
def getMotorConfig(server_ip, motor_number):
    data = {
        'method': 'GET',
        'reqTarget': '/m1/motor/config',
    }
    if motor_number == 0:
        data['reqTarget'] = '/m0/motor/config'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/motor/config'
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
        if json_obj.get('status') == 'OK':
            return json_obj.get('current_lim'), json_obj.get('current_lim_margin'), json_obj.get(
                'inverter_temp_limit_lower'), json_obj.get('inverter_temp_limit_upper'), json_obj.get(
                'requested_current_range'), json_obj.get('current_control_bandwidth')
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS set actuator MotorConfig parameter
# Parameters: motor current limit, current limit margin, inverter temperature offline, inverter temperature upper limit, current control bandwidth, including device IP, motor number
# ruturn seccess or fail
def setMotorConfig(dict, server_ip, motor_number):
    data = {
        'method': 'SET',
        'reqTarget': '/m1/motor/config',
        'current_lim': 15,
        'current_lim_margin': 5,
        'inverter_temp_limit_lower': 100,
        'inverter_temp_limit_upper': 120,
        'requested_current_range': 30,
        'current_control_bandwidth': 1000,
    }
    if motor_number == 0:
        data['reqTarget'] = '/m0/motor/config'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/motor/config'
    data['current_lim'] = dict['current_lim']
    data['current_lim_margin'] = dict['current_lim_margin']
    data['inverter_temp_limit_lower'] = dict['inverter_temp_limit_lower']
    data['inverter_temp_limit_upper'] = dict['inverter_temp_limit_upper']
    data['requested_current_range'] = dict['requested_current_range']
    data['current_control_bandwidth'] = dict['current_control_bandwidth']
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
        if json_obj.get('status') == 'OK':
            return json_obj.get('current_lim'), json_obj.get('current_lim_margin'), json_obj.get(
                'inverter_temp_limit_lower'), json_obj.get('inverter_temp_limit_upper'), json_obj.get(
                'requested_current_range'), json_obj.get('current_control_bandwidth')
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS Get the trajectory parameters of actuator trapezoidal mode
# parameter: server ip, motor number
# ruturn trapezoidal acceleration, trapezoidal deceleration, trapezoidal speed limit as a tuple
def getTrapTraj(server_ip, motor_number):
    data = {
        'method': 'GET',
        'reqTarget': '/m0/trap_traj',
    }
    if motor_number == 0:
        data['reqTarget'] = '/m0/trap_traj'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/trap_traj'
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
        if json_obj.get('status') == 'OK':
            return json_obj.get('accel_limit'), json_obj.get('decel_limit'), json_obj.get('vel_limit')
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS Set trajectory parameters of actuator trapezoidal mode
# parameter：Including trapezoidal acceleration, trapezoidal deceleration, trapezoidal speed limit, device IP, motor number
# Return success or failure as a tuple
def setTrapTraj(dict, server_ip, motor_number):
    data = {
        'method': 'SET',
        'reqTarget': '/m0/trap_traj',
        'accel_limit': 320000,
        'decel_limit': 320000,
        'vel_limit': 200000
    }
    if motor_number == 0:
        data['reqTarget'] = '/m0/trap_traj'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/trap_traj'
    data['accel_limit'] = dict['accel_limit']
    data['decel_limit'] = dict['decel_limit']
    data['vel_limit'] = dict['vel_limit']
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS Set the target position of the trapezoidal motion track
# parameter: position,reply enable,server ip,motor number
# return position, velocity, current
def trapezoidalMove(position, reply_enable, server_ip, motor_number):
    data = {
        'method': 'SET',
        'reqTarget': '/m0/trapezoidalMove',
        'property': 0,
        'reply_enable': True
    }
    if motor_number == 0:
        data['reqTarget'] = '/m0/trapezoidalMove'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/trapezoidalMove'
    data['property'] = position
    data['reply_enable'] = reply_enable
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_rt))
    if reply_enable:
        try:
            data, address = s.recvfrom(1024)
            # print('Server received from {}:{}'.format(address, data.decode('utf-8')))
            json_obj = json.loads(data.decode('utf-8'))
            print("Position = %.2f, Velocity = %.0f, Current = %.4f \n" % (
            json_obj.get('position'), json_obj.get('velocity'), json_obj.get('current')))
        except socket.timeout:  # fail after 1 second of no activity
            print("Didn't receive anymore data! [Timeout]")


# AIOS position control
# paramter: position,velocity,current feedforward,serverIP,motor number
# return position, velocity, current
def setPosition(position, velocity_ff, current_ff, reply_enable, server_ip, motor_number):
    data = {
        'method': 'SET',
        'reqTarget': '/m0/setPosition',
        'reply_enable': False,
        'position': 0,
        'velocity_ff': 0,
        'current_ff': 0
    }
    data['reply_enable'] = reply_enable
    if motor_number == 0:
        data['reqTarget'] = '/m0/setPosition'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/setPosition'
    data['position'] = position
    data['velocity_ff'] = velocity_ff
    data['current_ff'] = current_ff
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_rt))
    # if reply_enable:
    #     try:
    #         data, address = s.recvfrom(1024)
    #         # print('Server received from {}:{}'.format(address, data.decode('utf-8')))
    #         json_obj = json.loads(data.decode('utf-8'))
    #         print("Position = %.2f, Velocity = %.0f, Current = %.4f \n" %(json_obj.get('position'), json_obj.get('velocity'), json_obj.get('current')))
    #     except socket.timeout: # fail after 1 second of no activity
    #         print("Didn't receive anymore data! [Timeout]")


# AIOS velocity control
# paramter: velocity,current feedforward,serverIP,motor number
# return position, velocity, current
def setVelocity(velocity, current_ff, reply_enable, server_ip, motor_number):
    data = {
        'method': 'SET',
        'reqTarget': '/m0/setVelocity',
        'velocity': 0,
        'current_ff': 0
    }
    data['reply_enable'] = reply_enable
    if motor_number == 0:
        data['reqTarget'] = '/m0/setVelocity'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/setVelocity'
    data['velocity'] = velocity
    data['current_ff'] = current_ff
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_rt))
    if reply_enable:
        try:
            data, address = s.recvfrom(1024)
            # print('Server received from {}:{}'.format(address, data.decode('utf-8')))
            json_obj = json.loads(data.decode('utf-8'))
            print("Position = %.2f, Velocity = %.0f, Current = %.4f \n" % (
            json_obj.get('position'), json_obj.get('velocity'), json_obj.get('current')))
        except socket.timeout:  # fail after 1 second of no activity
            print("Didn't receive anymore data! [Timeout]")


# AIOS current control
# parameter：current, server IP, motor number
# return position, speed, current
def setCurrent(current, reply_enable, server_ip, motor_number):
    data = {
        'method': 'SET',
        'reqTarget': '/m0/setCurrent',
        'current': 0,
    }
    data['reply_enable'] = reply_enable
    if motor_number == 0:
        data['reqTarget'] = '/m0/setCurrent'
    elif motor_number == 1:
        data['reqTarget'] = '/m1/setCurrent'
    data['current'] = current
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_rt))
    if reply_enable:
        try:
            data, address = s.recvfrom(1024)
            # print('Server received from {}:{}'.format(address, data.decode('utf-8')))
            json_obj = json.loads(data.decode('utf-8'))
            print("Position = %.2f, Velocity = %.0f, Current = %.4f \n" % (
            json_obj.get('position'), json_obj.get('velocity'), json_obj.get('current')))
        except socket.timeout:  # fail after 1 second of no activity
            print("Didn't receive anymore data! [Timeout]")


def dum_func(server_ip):
    data = {
        'method': 'XET',
        'reqTarget': '/',
    }
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_rt))


def receive_func():
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        return True
    except socket.timeout:  # fail after 1 second of no activity
        return False
        print("Didn't receive anymore data! [Timeout]")


# IO_Module set IO_State status
# parameter：PWM0_CH PWM1_CH SERVO0 SERVO1
# Parameter value range: PWM0_CH,PWM1_CH[0~65535], SERVO0,SERVO1[0~180]
# return AI0 AI1 DI0 DI1
def setIOState(dict, reply_enable, server_ip):
    data = {
        'method': 'SET',
        'reqTarget': '/IO_State',
        'reply_enable': True
    }
    data['reply_enable'] = reply_enable
    data['PWM0_CH'] = dict['PWM0_CH']
    data['PWM1_CH'] = dict['PWM1_CH']
    data['SERVO0'] = dict['SERVO0']
    data['SERVO1'] = dict['SERVO1']
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_rt))
    if reply_enable:
        try:
            data, address = s.recvfrom(1024)
            print('Server received from {}:{}'.format(address, data.decode('utf-8')))
            # json_obj = json.loads(data.decode('utf-8'))
            # print("Position = %.2f, Velocity = %.0f, Current = %.4f \n" %(json_obj.get('position'), json_obj.get('velocity'), json_obj.get('current')))
        except socket.timeout:  # fail after 1 second of no activity
            print("Didn't receive anymore data! [Timeout]")


# IO_Module get IO_Staten status
# return data PWM0_CH,PWM1_CH[0~65535], SERVO0,SERVO1[0~180]
# return AI0[0~4096] AI1[0~4096] DI0[0,1] DI1[0,1] PWM0_CH,PWM1_CH[0~65535], SERVO0,SERVO1[0~180]
def getIOState(server_ip):
    data = {
        'method': 'GET',
        'reqTarget': '/IO_State',
    }
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_rt))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        # json_obj = json.loads(data.decode('utf-8'))
        # print("Position = %.2f, Velocity = %.0f, Current = %.4f \n" %(json_obj.get('position'), json_obj.get('velocity'), json_obj.get('current')))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# aios get network_setting status
# return data
def getNetworkSetting(server_ip):
    data = {
        'method': 'GET',
        'reqTarget': '/network_setting',
    }
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        # json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# aios set network_setting status
# return data
def setNetworkSetting(dict, server_ip):
    data = {
        'method': 'SET',
        'reqTarget': '/network_setting',
        'DHCP_enable': 'True',
    }
    data['DHCP_enable'] = dict['DHCP_enable']
    data['SSID'] = dict['SSID']
    data['password'] = dict['password']
    data['name'] = dict['name']
    if dict['DHCP_enable'] == False:
        data['staticIP'] = dict['staticIP']
        data['gateway'] = dict['gateway']
        data['subnet'] = dict['subnet']
        data['dns_1'] = dict['dns_1']
        data['dns_2'] = dict['dns_2']
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        # json_obj = json.loads(data.decode('utf-8'))
        # print("Position = %.2f, Velocity = %.0f, Current = %.4f \n" %(json_obj.get('position'), json_obj.get('velocity'), json_obj.get('current')))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS Get encoder info
# Parameters: including device IP
# Return: encoder information
def getEncoderInfo(server_ip):
    data = {
        'method': 'GET',
        'reqTarget': '/encoder_info',
    }
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS Get absolute encoder position
# Parameters: including device IP
# Return: encoder information
def getAbsEncoder(server_ip):
    data = {
        'method': 'GET',
        'reqTarget': '/abs_encoder',
    }
    json_str = json.dumps(data)
    print("Send JSON Obj:", json_str)
    s.sendto(str.encode(json_str), (server_ip, PORT_srv))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        json_obj = json.loads(data.decode('utf-8'))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS passthrough pt_port
# 参数：包括设备IP 电机号
# 无返回
def passthrough_pt(server_ip, tx_messages):
    print("Send Data:", tx_messages)
    s.sendto(tx_messages.encode('ascii'), (server_ip, PORT_pt))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data.decode('utf-8')))
        # print('Server received from {}:{}'.format(address, data.hex()))
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS passthrough pt_port binary
# 参数：包括设备IP 电机号
# 无返回
def passthrough_pt_bin(server_ip, cmd, position, velocity, torque):
    tx_messages = struct.pack('<Bfhh', cmd, position, velocity, torque)
    print("Send Data:", tx_messages)
    s.sendto(tx_messages, (server_ip, PORT_pt))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data))
        feedback = struct.unpack('<fffi', data[1:17])
        return feedback
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


##################################### For fw-v2.0  #################################
# AIOS set input position through pt port
# 参数：包括设备IP 电机号
# 无返回
def setInputPosition_pt(server_ip, position, velocity, torque):
    velocity = int(velocity * 1000)
    torque = int(torque * 1000)
    tx_messages = struct.pack('<Bfhh', 0x0c, position, velocity, torque)
    print("Send Data:", tx_messages)
    s.sendto(tx_messages, (server_ip, PORT_pt))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data))
        feedback = struct.unpack('<fffi', data[1:17])
        return feedback
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS set input velocity through pt port
# 参数：包括设备IP 电机号
# 无返回
def setInputVelocity_pt(server_ip, velocity, torque):
    tx_messages = struct.pack('<Bff', 0x0d, velocity, torque)
    print("Send Data:", tx_messages)
    s.sendto(tx_messages, (server_ip, PORT_pt))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data))
        feedback = struct.unpack('<fffi', data[1:17])
        return feedback
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS set input torque through pt port
# 参数：包括设备IP 电机号
# 无返回
def setInputTorque_pt(server_ip, torque):
    tx_messages = struct.pack('<Bf', 0x0e, torque)
    print("Send Data:", tx_messages)
    s.sendto(tx_messages, (server_ip, PORT_pt))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data))
        feedback = struct.unpack('<fffi', data[1:17])
        return feedback
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


# AIOS get CVP through pt port
# 参数：包括设备IP 电机号
# 无返回
def getCVP_pt(server_ip):
    tx_messages = struct.pack('<B', 0x1a)
    print("Send Data:", tx_messages)
    s.sendto(tx_messages, (server_ip, PORT_pt))
    try:
        data, address = s.recvfrom(1024)
        print('Server received from {}:{}'.format(address, data))
        feedback = struct.unpack('<fffi', data[1:17])
        return feedback
    except socket.timeout:  # fail after 1 second of no activity
        print("Didn't receive anymore data! [Timeout]")


#######################################################################################


# 广播查询局域网下的全部 AIOS
# 参数：无
# 返回 成功 失败 超时
def broadcast_func():
    print('Listening for broadcast at ', s.getsockname())
    timeout = 3
    found_server = False
    address_list = []

    start = time.time()
    s.sendto('Is any AIOS server here?'.encode('utf-8'), (network, PORT_srv))
    print('\n')

    while True:
        try:
            data, address = s.recvfrom(1024)
            latency = time.time() - start
            print(latency * 1000)
            address_list.append(address[0])
            print('Server received from {}:{}'.format(address, data.decode('utf-8')))
            # json_obj = json.loads(data.decode('utf-8'))
            found_server = True
        except socket.timeout:  # fail after 1 second of no activity
            if found_server:
                print('\n')
                print('found servers')
                print(address_list)
                print('lookup Finished! \n')
                time.sleep(2)
                return address_list
            else:
                print("Do not have any server! [Timeout] \n")
                return False
            break

    print('\n')
