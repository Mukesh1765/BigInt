# BigInteger Implementation

A C implementation of a calculator that handles arbitrary-precision integers (up to 1024 bits/309 decimal digits). The calculator supports basic arithmetic operations on very large numbers that exceed the standard integer data types.

## Features

- Handles integers up to 309 decimal digits (1024 bits)
- Supports both positive and negative numbers
- Basic arithmetic operations:
  - Addition
  - Subtraction
  - Multiplication
  - Division
- Input validation and error handling
- Memory-efficient representation using uint8_t arrays

## Implementation Details

- Uses a custom BigInt structure to store large numbers
- Numbers are stored digit-by-digit in uint8_t arrays
- Sign is handled separately using an enumeration
- Input format: Numbers must be preceded by a sign (+/-) or space
- Implements standard arithmetic algorithms with optimizations for large numbers

## Usage

When running the program:
1. Choose an operation (1-4)
2. Enter two numbers (each preceded by +/- or space)
3. View the result
4. Continue with new calculations or exit (option 5)

## Memory Management

The implementation includes proper memory allocation and deallocation to prevent memory leaks, with error handling for allocation failures.
