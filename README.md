# GCCompiler

Gębalang is a simple imerative language based on i have no idea what. Let me introduce a *a new georgus compiler* of this awesome language - **GCCompiler**.

## Compile sources

Source files are located in `./compiler` directory. To compile the compiler by yourself you will need:

| tool  | version |
|-------|---------|
| flex  | 2.6.4   |
| bison | 3.0.4   |
| g++   | 7.3.0   |
| make  | 4.1     |

To compile program from sources, just run command `make` or `make compiler` in `./compiler` folder. Your compiler will appear in `./out` directory.

## Run your first program

You will find language documentation in `./data/docs.pdf` file. Sample programs are located in `./compiler/programs/` folder.

To compile a program run command `./compiler input_file output_file`.
Output file will appear. You can run your program using interpreter located in `./interpreter` directory. To compile interpreter run `make maszyna-rejestrowa`. Run it with your output file as an argument. Thats it!

## Licence

```
The Beerware License

Copyright (c) 2018 Maciej Hajduk

Permission is hereby granted, free of charge, to any person obtaining a copy of this software. As long as you retain this notice you can do whatever you want with this stuff.

MOREOVER, IF YOU MEET THE AUTHOR OF THE PROGRAM, YOU ARE OBLIGATED TO BRING HIM A BEER (as if I am on every KN Solvro meeting).
```