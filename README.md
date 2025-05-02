# Yet Another Entertainment System

> A simple portable NES emulator written in C++.

The emulator currently supports the following mapper types:
- NROM
- (TODO)

It was mainly targeted for a simple custom operating system to run on the bare-metal. Because of this, it does not use
any standard libraries and it does not do dynamic memory allocations. It also supports using a modified NES controller
via serial port.

## Using the Emulator

The emulator comes with its own GUI allowing the user to change some settings and select a ROM using its built-in file
browser. There is only some platform-specific functionality which needs to be implemented. Some frontends can be found
in the [app](app) directory:
- A macOS port using `GameController.framework` and `SpriteKit.framework`.
- A (WIP) SDL port using SDL 3.

In addition, you can find two more helper applications:
- `controller` for testing a controller connected using serial port (see below).
- `encrypt` for encrypting ROMs using AES (might one day be useful in some cases)

You can build all of the applications using CMake:
```
mkdir build
cd build
cmake ..
make
```

Dependencies on Linux can also be installed using Nix, by running a development shell:
```
nix develop .
```

## Using the Controller

It is also possible to modify an original NES controller to work with this emulator using an Arduino.

You can find the Arduino sketch for it in [controller](controller).

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
