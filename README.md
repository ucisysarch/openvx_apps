# openvx_apps
Sample vision applications (currently only SIFT) implemented in OpenVX.

## Dependencies
1. OpenCV for sequential programs


## How to build
By building Openvx sample implementation, SIFT application will also be built. Two different versions of OpenVX are supported. Currently, version 1.1 has the most up-to-date implementation of SIFT. To build version 1.1 type the following commands in console. It will build OpenVX for 64bit linux machine. You can adjust these parameters according to your own set up. currently Linux and Windows are supported. Use the same commands to build version 1.0.1.

```bash
cd openvx_sample-1.1
python Build.py --arch=64 --os=linux
```

Applications will be installed in install folder in OpenVX root folder.
