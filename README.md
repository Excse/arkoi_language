<h1 align="center" id="title">Arkoi Language</h1>

<p align="center" id="description">
Arkoi Language is a small toy compiler project designed to provide a hands-on experience in developing a custom programming language and its ecosystem. This project serves as both an educational tool and an exploration of compiler optimizations and language design principles.
</p>

---

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Development Goals](#development-goals)
- [Contributing](#contributing)
- [License](#license)

---

## Introduction
Arkoi Language is a work-in-progress toy compiler written primarily in C++. It is aimed at developers and enthusiasts who are interested in:
- Understanding compiler internals.
- Experimenting with custom language features and optimizations.
- Building a small custom ecosystem around a toy programming language.

While the project is in its early stages, it is designed to be a learning platform for anyone curious about language and compiler design.

---

## Features
- **Custom Syntax**: A unique and intuitive syntax design for the Arkoi Language.
- **Compiler Development**: Learn how a compiler is structured, from parsing to code generation.
- **Optimization Techniques**: Explore basic optimization strategies for compiled code.
- **Assembly Integration**: Includes low-level Assembly code for understanding machine-level interactions.

---

## Installation
To build and run the Arkoi compiler, follow the steps below:

### Prerequisites
- A C++23 compatible compiler (e.g., GCC, Clang, or MSVC).
- CMake 3.29.6 or higher.
- A supported build system (e.g., make, Ninja).

### Steps
1. Clone the repository:
   ```bash
   git clone https://github.com/Excse/arkoi_language.git
   cd arkoi_language
   ```
2. Create a build directory and configure the project:
   ```bash
   mkdir build && cd build
   cmake ..
   ```
3. Build the project:
   ```bash
   cmake --build .
   ```

4. Run the compiler:
   ```bash
   ./arkoi_language <input_file>
   ```

---

## Usage
Once the Arkoi compiler is built, you can use it to compile Arkoi source files into executable binaries or intermediate representations.

### Example
```bash
./arkoi_language ../example/hello_world/main.ark
```

### Output
The compiler will generate the corresponding executable or intermediate output in the same directory.

---

## Project Structure
Here is an overview of the repository structure:

```
arkoi_language/
├── src/                # The source directory of the repository
│   └── main.cpp        # Main entry of the compiler
├── include/            # Same structure as the source directory
│   ├── ast/            # Abstract Syntax Tree (nodes, visitor)
│   ├── front/          # Frontend (parser, scanner, tokens)
│   ├── sem/            # Semantic Analysis (name and type resolution)
│   ├── il/             # Intermediate Language (dataflow, control flow graph, generator, printer, instructions, operands, visitor)
│   ├── opt/            # Optimization Passes
│   ├── x86_64/         # x86_64 Code Generation (generator, mapper, operands)
│   └── utils/          # Some useful utility functions
│── test/               # Unit tests for specific code parts
│   └── snapshot/       # A suit for snapshot testing (lexer, parser, etc.)
└── example/            # Some examples to showcase the Arkoi Language
    ├── hello_world/    # The main hello world program
    ├── test/           # An example that demonstrates every Arkoi feature
    └── ...             # Some other examples (test for calling convention etc.)
```

---

## Development Goals
The following goals are planned for the Arkoi Language project:

1. **Core Compiler**: Implement basic lexical analysis, parsing, and code generation.
2. **Custom Syntax and Semantics**: Define a unique set of rules and syntax for the Arkoi Language.
3. **Error Handling**: Provide detailed error messages for syntax and semantic issues.
4. **Optimization**: Explore optimization opportunities in the generated code.
5. **Ecosystem**: Build supporting tools and libraries for the Arkoi Language.

---

## Contributing
We welcome contributions to improve the Arkoi Language project! Here’s how you can get involved:

1. Fork the repository and create a new branch for your feature or bugfix.
2. Write clear and concise commit messages explaining your changes.
3. Submit a pull request to the `main` branch.

Please make sure to follow the [contribution guidelines](CODE_OF_CONDUCT.md).

---

## License
This project is licensed under the BSD 3-Clause License. See the [LICENSE](LICENSE) file for details.

---

<p align="center">
Happy coding with Arkoi Language! 🚀
</p>
