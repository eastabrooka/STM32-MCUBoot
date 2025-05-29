#!/bin/bash

echo "Looking for ST-Link device..."

# Get list of shared devices from usbipd
DEVICE_INFO=$(powershell.exe -Command "usbipd list" | grep -i 'st-link' | grep -i 'shared')

if [ -z "$DEVICE_INFO" ]; then
    echo "❌ No shared ST-Link device found."
    exit 1
fi

# Extract the BUSID (first column)
BUSID=$(echo "$DEVICE_INFO" | awk '{print $1}' | tr -d '\r')

echo "Attaching ST-Link (busid $BUSID) to WSL..."

powershell.exe -Command "usbipd attach --busid $BUSID --wsl"

if [ $? -eq 0 ]; then
    echo "✅ ST-Link attached successfully."
else
    echo "❌ Failed to attach ST-Link. It might already be attached or an error occurred."
fi
