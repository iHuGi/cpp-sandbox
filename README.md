```markdown
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

The engine will launch the executable and freeze execution exactly at the specified breakpoint, allowing full structural analysis of local variables, vectors, and memory states before proceeding.
```