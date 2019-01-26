# G. C. Compiler

```
 _____     _       __        ver. 3.0
|   __|___| |_ ___|  |   ___ ___ ___
|  |  | -_| . | .'|  |__| .'|   | . |
|_____|___|___|__,|_____|__,|_|_|_  |
        ~                       |___|
```

Glang is a simple imperative programming language based on i have no idea what. Let me introduce a *a new georgus compiler* of this awesome language - **G. C. Compiler**.


## Building

Source files are located in `./compiler/src` directory. To build the compiler by yourself you will need:

| tool  | version |
|-------|---------|
| flex  | 2.6.4   |
| bison | 3.0.4   |
| g++   | 7.3.0   |
| make  | 4.1     |

To compile compiler from sources, just run command `make` or `make compiler` in `./compiler` folder. Your compiler will appear in `./compiler/out` directory.

## Usage

You will find language documentation in `./compiler/docs/documentation.pdf` file. Sample programs are located in `./compiler/programs/` folder. Three games - _Tic Tac Toe_, _Tetris_ and _Guessing Game_ are located in `./compiler/programs/` as well. Have fun!

To compile a program run command `./compiler input_file output_file`.
Output file should appear. You can run your program using interpreter located in `./interpreter` directory. To compile interpreter, call `make interpreter` in its catalogue. To run program, execute interpreter with your output file as an argument.

You can aslo use scripts embedded in makefile to run your programs. To compile and run the program at a time, use `make debug input_file` Input file must be located in `./programs` directory, output file will apper in `./out/out`. For testing the compiler, use `make test number`. Tests are located in `./programs/tests`.

## Licence

> ***Programmer:*** a tool which converts caffeine into code

    **"THE COFFEEWARE LICENSE" (Revision 2):**

    Copyright (c) 2018 Maciej Hajduk

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software. As long as you retain this notice you can do whatever you want with this stuff. If we meet some day, and you think this stuff is worth it, you are obligated to buy me a beer / coffee in return.

    (as if I am on every KN Solvro meeting).
