#!/bin/bash
sudo mknod /dev/game c 253 0
sudo chgrp dialout /dev/game
sudo chmod +r+w /dev/game