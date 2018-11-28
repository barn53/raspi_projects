#!/bin/bash

# screen -dmS door53_server ./start_server.sh
sudo LD_LIBRARY_PATH=../send433/bin/:LD_LIBRARY_PATH node server.js

