#!/bin/bash

sudo sed -i s/us.archive.ubuntu/old-releases.ubuntu/g /etc/apt/sources.list
sudo sed -i s/security.ubuntu/old-releases.ubunut/g /etc/apt/sources.list
sudo apt-get update
sudo apt-get install libncurses5-dev libncursesw5-dev