# C++ Engine Deployment Guide

To initialise the logic engine in the Ubuntu terminal, the engineering team (me) utilises a standardised two-step deployment process, featuring an isolated binary directory for optimal workspace hygiene.

### Step 1: Compile the Source Code
The human-readable C++ source code must be translated into a bare-metal machine code binary using the GNU C++ compiler (`g++`). This compilation is executed via the terminal, routing the payload to our dedicated build folder. 

Replace `<filename>` with the target script you want to build:

```bash
g++ <filename>.cpp -o bin/<filename>
```
> **Architecture Note:** The `-o bin/<filename>` flag explicitly routes the compiled executable into a dedicated `bin/` directory rather than the root folder. This isolates the binaries from the source code, adhering to strict version control guidelines and preventing workspace clutter.

### Step 2: Execute the Binary
Upon silent compilation (which indicates zero syntax or structural errors), the Linux kernel is instructed to execute the payload. In Unix-based environments, the relative directory path must be explicitly specified to run custom executables:

```bash
./bin/<filename>
```

Execution successfully routes the program's operations and memory output directly to the standard terminal stream. The CPU processes the compiled bare-metal instructions natively in milliseconds, confirming system stability and optimal execution speed.
```