#!/bin/bash

LOG_FILE="/home/hugo_azevedo/cpp_train/pipeline.log"

cd /home/hugo_azevedo/cpp_train/bin/ || exit 1

echo "--- Run: $(date '+%Y-%m-%d %H:%M:%S') ---" >> "$LOG_FILE"

./weather_info_csv >> "$LOG_FILE" 2>&1

echo "Exit code: $?" >> "$LOG_FILE"