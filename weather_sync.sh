#!/bin/bash

# LOG_FILE="/home/hugo_azevedo/cpp_train/pipeline.log"

# cd /home/hugo_azevedo/cpp_train/bin/ || exit 1

# echo "--- Run: $(date '+%Y-%m-%d %H:%M:%S') ---" >> "$LOG_FILE"

# ./weather_info_csv >> "$LOG_FILE" 2>&1

# echo "Exit code: $?" >> "$LOG_FILE"

#!/bin/bash

# Configuration: Sets base directory and log location using the user's home path
BASE_DIR="$HOME/cpp_train"
LOG_FILE="$BASE_DIR/pipeline.log"

# Navigate to binary directory; aborts script if directory is missing
cd "$BASE_DIR/bin/" || exit 1

# Logs the current execution timestamp
echo "--- Run: $(date '+%Y-%m-%d %H:%M:%S') ---" >> "$LOG_FILE"

# Executes binary and redirects both standard output and errors to the log file
./weather_info_csv >> "$LOG_FILE" 2>&1

# Appends the exit status of the previous command to track success or failure
echo "Exit code: $?" >> "$LOG_FILE"