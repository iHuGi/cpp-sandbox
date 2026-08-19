# C++ Engine Deployment Guide

To initialise the logic engine in the Ubuntu terminal, the engineering team (me) utilises a standardised two-step deployment process, featuring an isolated binary directory for optimal workspace hygiene.

### Step 1: Compile the Source Code

The human-readable C++ source code must be translated into a bare-metal machine code binary using the GNU C++ compiler (`g++`). This compilation is executed via the terminal, routing the payload to our dedicated build folder.

Replace `<filename>` with the target script you want to build:

```bash
g++ -std=c++20 <filename>.cpp -o bin/<filename>

```

> **Architecture Note:** The `-o bin/<filename>` flag explicitly routes the compiled executable into a dedicated `bin/` directory rather than the root folder. This isolates the binaries from the source code, adhering to strict version control guidelines and preventing workspace clutter.

### Step 2: Execute the Binary

Upon silent compilation (which indicates zero syntax or structural errors), the Linux kernel is instructed to execute the payload. In Unix-based environments, the relative directory path must be explicitly specified to run custom executables:

```bash
./bin/<filename>

```

Execution successfully routes the program's operations and memory output directly to the standard terminal stream. The CPU processes the compiled bare-metal instructions natively in milliseconds, confirming system stability and optimal execution speed.

### Step 3: Visual Debugging & Memory Inspection (VS Code Integration)

To peek inside the CPU's execution state and inspect memory allocations in real-time, the workspace is configured with a dynamic, universal debugging profile using GDB (GNU Debugger).

**Prerequisite Compilation:**
To enable visual breakpoints and memory tracking, the payload must be compiled with debug symbols attached (using the `-g` flag) and the modern C++20 standard explicitly defined:

```bash
g++ -g -std=c++20 <filename>.cpp -o bin/<filename>

```

**System Configuration (`.vscode/launch.json`):**
This universal configuration automatically detects the active editor tab and routes the debugging stream to the corresponding compiled binary in the `bin/` directory.

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(gdb) Launch Active File",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/bin/${fileBasenameNoExtension}",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "/usr/bin/gdb", 
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}

```

**Execution Protocol:**

1. Place a visual breakpoint (red dot) in the editor gutter next to the target line of code.
2. Ensure the currently active window in the editor is the target `.cpp` file you wish to debug.
3. Initialise the debugging sequence by pressing `F5`.

### Step 4: Automated Build Pipeline (The Architect's Approach)

While manual compilation is essential for understanding the compiler's underlying mechanics, maintaining individual build commands does not scale. To orchestrate the entire compilation pipeline automatically, the workspace is equipped with a dynamic `Makefile`.

Instead of manually compiling each script, you can trigger the automated build engine from the root directory:

```bash
make

```

**Workspace Hygiene (Cleanup Protocol):**
To purge all compiled binaries and reset the execution environment, run:

```bash
make clean

```

### Step 5: Automated Pipeline Scheduling (The "Set-and-Forget" Approach)

To ensure the ETL pipeline operates autonomously, we decouple the execution into two layers: a **Bash Script** (the instruction) and **Cron** (the scheduler).

> **CRITICAL PATH NOTE:** Linux Cron jobs start in a neutral system environment and do not know your project folder. Always use **absolute paths** (e.g., `/home/hugo_azevedo/...`) inside your C++ code for file I/O (CSV, .env) and inside your Bash scripts. Using relative paths like `../` will fail in production.

**1. Create the Execution Script (`weather_sync.sh`):**
Create this file in the project's root folder. It forces the kernel into the correct directory before executing:

```bash
#!/bin/bash
# ETL Pipeline Runner with absolute pathing
cd /home/hugo_azevedo/cpp_train/bin/
./weather_info_csv >> /home/hugo_azevedo/cpp_train/error.log 2>&1

```

*Don't forget to give it execution permissions:*

```bash
chmod +x weather_sync.sh

```

**2. Configure the Cron Job:**
The `cron` scheduler acts as the system manager. Open the configuration editor:

```bash
crontab -e

```

Add this line to the end of the file to trigger the sync every hour (at the top of the hour):

```bash
0 * * * * /home/hugo_azevedo/cpp_train/weather_sync.sh

```

**3. Verification (Debugging the Background Process):**
Since Cron runs in the background, you won't see terminal output. Use these commands to confirm it's alive:

* **Check if it triggered:** ```bash
grep "weather_info_csv" /var/log/syslog | tail -n 5

```
* **Check logs:** If the pipeline fails, inspect the error output: 
```bash
cat /home/hugo_azevedo/cpp_train/error.log

```