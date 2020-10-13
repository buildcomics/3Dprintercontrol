#!/usr/bin/env python3
import pigpio
import time
import datetime
import signal
import sys
import config as cfg
import requests
import gphoto2 as gp #https://pypi.org/project/gphoto2/#install-with-pip, be sure to install libgphoto2-dev!!
import flask
import shutil
from flask import request, jsonify, send_file
from octorest import OctoRest
import re
import serial

ser = serial.Serial(cfg.arm["port"], cfg.arm["baudrate"], timeout=2) # Establish the connection on a specific port

def make_client():
    try:
        client = OctoRest(url=cfg.octoprint["url"], apikey=cfg.octoprint["apikey"])
        return client
    except Exception as e:
        print(e)

def signal_handler(sig, frame):
    prd("You pressed Ctrl+C bye bye!")
    fd.close()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

def prd(msg): #print debug
    stamp = str(datetime.datetime.now())
    fd.write(stamp + msg + "\n")
    print(stamp + " DEBUG " + str(msg))
    fd.flush();

fd = open("/srv/python/3dprintercontrol/debug.log", "a")
prd("Starting 3D Printer Control")

pi = pigpio.pi()
# raspberry b pinout: https://pi4j.com/1.2/pins/model-b-rev1.html
control_pin_number = cfg.psu["control_pin_number"]
readout_pin_number = cfg.psu["readout_pin_number"]

pi.set_mode(control_pin_number, pigpio.OUTPUT)

dslr_running = False

app = flask.Flask(__name__)
app.config["DEBUG"] = False
@app.route('/', methods=['GET'])

def home():
    return "<h1>3D Printer Control!</h1><p>hello, anyone there?</p>"

@app.route('/api/v1/resources/dslr_start', methods=['GET'])
def api_dslr_start():
    global dslr_running
    if not dslr_running:
        print('Starting camera')
        global camera
        camera = gp.Camera()
        camera.init()
        dslr_running = True
    out = {'dslr': 'on'}
    return jsonify(out)

@app.route('/api/v1/resources/dslr_stop', methods=['GET'])
def api_dslr_stop():
    global dslr_running
    global camera
    if dslr_running:
        print('Stopping camera')
        camera.exit()
        del camera
        dslr_running = False
    out = {'dslr': 'off'}
    return jsonify(out)

@app.route('/api/v1/resources/dslr_snap', methods=['GET'])
def api_dslr_snap():
    global dslr_running
    global camera
    if dslr_running:
        print('Capturing image')
        file_path = camera.capture(gp.GP_CAPTURE_IMAGE)
        target = '/tmp/capture.jpg'
        print('Copying image to', target)
        camera_file = camera.file_get(file_path.folder, file_path.name, gp.GP_FILE_TYPE_NORMAL)
        camera_file.save(target)
        return send_file(target, mimetype='image/jpeg')
    else:
        return "dslr not running", 400

@app.route('/api/v1/resources/dslr_trigger', methods=['GET'])
def api_dslr_trigger():
    global dslr_running
    global camera
    if dslr_running:
        print('Capturing image')
        file_path = camera.capture(gp.GP_CAPTURE_IMAGE)
        out = {'result': 'ok'}
        return jsonify(out)
    else:
        return "dslr not running", 400
@app.route('/api/v1/resources/power', methods=['GET'])
def api_power():
    pwr = pi.read(readout_pin_number)
    if pwr == 0:
        out = {'power': 'on'}
    else:
        out = {'power': 'off'}
    return jsonify(out)

@app.route('/api/v1/resources/off', methods=['GET'])
def api_off():
    pi.write(control_pin_number, 0)
    pwr = pi.read(readout_pin_number)
    if pwr == 0:
        out = {'power': 'on'}
    else:
        out = {'power': 'off'}
    return jsonify(out)


@app.route('/api/v1/resources/on', methods=['GET'])
def api_on():
    pi.write(control_pin_number, 1)
    pwr = pi.read(readout_pin_number)
    if pwr == 0:
        out = {'power': 'on'}
    else:
        out = {'power': 'off'}
    return jsonify(out)

@app.route('/api/v1/resources/arm_continue', methods=['GET'])
def api_arm_continue():
    print('Continueing arm')
    ser.reset_input_buffer()
    ser.write("C\n".encode())
    for i in range(20):
        print(i)
        line = ser.readline()
        print(line)
        if line == b'Continue\r\n':
            print("Arm Continue");
            break
        else:
            print("waiting...")
    out = {'result': 'ok'}
    return jsonify(out)

@app.route('/api/v1/resources/arm_pause', methods=['GET'])
def api_arm_pause():
    print('Pausing arm')
    ser.reset_input_buffer()
    ser.write("P\n".encode())
    success = False
    for i in range(20):
        print(i)
        line = ser.readline()
        print(line)
        if line == b'Pause\r\n':
            print("Arm Pause");
            success = True
            break
        else:
            print("waiting...")
    if success:
        out = {'result': 'ok'}
        return jsonify(out)
    else:
        return "Could not pause arm", 400

@app.route('/api/v1/resources/arm_reset', methods=['GET'])
def api_arm_reset():
    print('Resetting arm')
    ser.reset_input_buffer()
    ser.write("R\n".encode())
    success = False
    for i in range(20):
        print(i)
        line = ser.readline()
        print(line)
        if line == b'At base position, enter step delay?\r\n':
            print("Arm Reset")
            succes = True
            break
        else:
            print("resetting...")
            ser.write("R\n".encode())
    if success:
        out = {'result': 'ok'}
        return jsonify(out)
    else:
        return "Could not reset arm", 400

@app.route('/api/v1/resources/arm_start', methods=['GET'])
def api_arm_start():
    prd('Starting arm')
    client = make_client()
    job_info = client.job_info()
    if job_info["job"]["file"]["name"] == None:
        print("no active file found!")
        return "No active print filename found", 400

    matches = re.search("_(\d)+h(\d+)m\.gcode", job_info["job"]["file"]["name"])
    if matches == None:
        prd("no time match, retrying...")
        matches = re.search("_(\d+)m\.gcode", job_info["job"]["file"]["name"])
        if matches == None:
            return "Could not get time in form of Xh_Xm.gcode or _Xm.gcode", 400
        else:
            minutes = int(matches.group(1))
    else:
        hours = int(matches.group(1))
        minutes = int(matches.group(2)) + (hours*60)
    prd("minutes:")
    prd(str(minutes))
    step_time_ms = int((minutes*60*1000)/(300*6)) #delay is print time in milliseconds / (300 steps * 6 delays per staps)
    prd("step time:")
    prd(str(step_time_ms))
    ser.reset_input_buffer()
    success = False
    for i in range(20):
        prd(str(i))
        line = ser.readline()
        prd(line.decode("utf-8"))
        if line == b'At base position, enter step delay?\r\n':
            prd("starting rotation");
            ser.write(str(step_time_ms).encode())
        elif line.decode("utf-8").startswith('starting with delay:'):
            prd("rotation has been started")
            success = True
            break
        else:
            prd("resetting...")
            ser.write("R\n".encode())
    if success:
        out = {'result': 'ok'}
        return jsonify(out)
    else:
        return "Could not start rotation", 400

app.run(host='0.0.0.0')
