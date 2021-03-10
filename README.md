# SmartHome_Tests

## Description
This project is a simple test framework, which allows to design and run some "blackbox" tests of [SmartHome_CoreApplication](https://github.com/JacSko/SmartHome_CoreApplication).

![testcore_diagram](https://user-images.githubusercontent.com/47041583/110685182-4bd67200-81de-11eb-89a9-2ace2c2672b6.png)

**Project overview:**
- **core** - Here are placed test framework source files.
- **logs** - Output from test execution.
- **external** - some external stuff (googletest framework, SmartHome_CoreApplication API)
- **test_executables** - Here are copied all test binaries after build.
- **test_suites** - All source files with test cases.

## Details
Communication between TestCore and SmartHome_CoreApplication is based on TCP sockets.
Test framework acts as a server, CoreApplication as client respectively.
There are 3 sockets opened:
  - **logging module** - on this socket, the test framework receives the debug traces from tested binary.
  - **hw_stub** - socket to control the stubbed peripherials (I2C driver, DHT driver, etc).
  - **App_ntf** - on this socket are sent notifications (which normally are sent to [SmartHome_RPi](https://github.com/JacSko/SmartHome_RPi)
Those 3 channels allows to verify and control the behavior of tested binary.

## Building
**Before build and run this test framework, You should build the tested SmartHome binary - please follow detailed description [here](https://github.com/JacSko/SmartHome_CoreApplication)**

To build TestCore and tests:
```
cd <project_dir>
mkdir build && cd build
cmake ..
make
```
Or simply run the script build_and_run_tests.sh in project root.
All test binaries are placed in <project_dir>/test_executables - You can run ony the desired one.
## TODO
- [ ] Detailed tests of debug interface (receiving and parsing commands)
- [ ] Possibility to set DHT sensor response type to simulate e.g sensor disconnection (currently only sensor data can be set)
- [ ] Possibility to set I2C transaction result (currently only data exchange is supported)
