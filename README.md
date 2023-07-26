# esp32-shutter-speed-tester

[![ESP32](https://img.shields.io/badge/ESP-32-000000.svg?longCache=true&style=flat&colorA=AA101F)](https://www.espressif.com/en/products/socs/esp32)
<br>
[![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)

## Setup


### Libraries

All required libraries this project are kept as git submodules within `./libraries`.

To initialise these, run `git submodule init && git submodule update` in the project root.

To update specific libraries to newer versions, run `git submodule update --remote libraries/TFT_eSPI`.

Once cloned, go into TFT_eSPI and edit the contents of `User_Setup.h`:
 - Comment lines L170-176 and L363
 - Copy the content of tft-setup.h into it (I dont know why i couldn't get this include working here only)

# Author

[Jack Burgess](https://jackburgess.dev)

# License

MIT
