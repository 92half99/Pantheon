#!/bin/bash

export LD_LIBRARY_PATH=`realpath Utopia/Utopia-Modules/Utopia-Networking/vendor/GameNetworkingSockets/bin/Linux`
make -j8
