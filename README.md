# BigInteger Implementation

A **C implementation** of a calculator that handles **arbitrary-precision integers** (up to 1024 bits / 309 decimal digits). The calculator supports basic arithmetic operations on very large numbers that exceed the limits of standard integer data types.

---

## Features

* Handles integers up to **309 decimal digits** (\~1024 bits)
* Supports both **positive and negative** numbers
* Basic arithmetic operations:

  * **Addition**
  * **Subtraction**
  * **Multiplication** (standard and optimized with **Karatsuba**)
  * **Division**
* Input validation and error handling
* Memory-efficient representation using `uint8_t` arrays

---

## Implementation Details

* Uses a **custom `BigInt` structure** to store large integers
* Numbers are stored **digit-by-digit** in `uint8_t` arrays (least significant digit at index 0)
* The **sign** is stored separately using a flag or enumeration
* Input format: Numbers must be preceded by a **sign (+/-)** or space
* Implements **standard arithmetic algorithms**, and **Karatsuba multiplication** for large inputs

---

## Arithmetic Operation Details

### ➕ Addition

* **Handling signs**:

  * Same signs → perform direct addition
  * Different signs → convert to subtraction
* **Procedure**:

  * Traverse both digit arrays from LSB to MSB
  * Add digits with carry
  * Append carry if it remains
* **Time Complexity**: O(n)

---

### ➖ Subtraction

* **Handling signs**:

  * Different signs → convert to addition
  * Same signs → perform digit-wise subtraction
* **Procedure**:

  * Subtract smaller number from the larger one
  * Handle borrow across digits
  * Set sign based on which number is larger
* **Time Complexity**: O(n)

---

### ✖️ Multiplication

Supports **two methods** depending on input size:

#### 🔹 Grade-School Multiplication (For Small Inputs)

* Multiply each digit of one number with every digit of the other
* Use proper digit shifting (like manual multiplication)
* Accumulate partial results and handle carry
* **Time Complexity**: O(n²)

#### 🔹 Karatsuba Multiplication (For Large Inputs)

* A **divide-and-conquer** algorithm that reduces the number of single-digit multiplications
* **Steps**:

  1. Split numbers `X` and `Y` into high and low parts:
     `X = X1 * B^m + X0`, `Y = Y1 * B^m + Y0`
  2. Recursively compute:

     * `Z0 = X0 * Y0`
     * `Z2 = X1 * Y1`
     * `Z1 = (X1 + X0) * (Y1 + Y0) - Z2 - Z0`
  3. Combine result:
     `Result = Z2 * B^{2m} + Z1 * B^m + Z0`
* **Advantages**:

  * Reduces multiplication count from 4 to 3
  * Much faster than traditional O(n²) for large inputs
* **Time Complexity**: O(n^log₂3) ≈ O(n^1.585)

---

### ➗ Division

* Implements **long division** algorithm
* **Procedure**:

  * Normalize inputs
  * Extract segments of the dividend
  * Repeatedly subtract divisor and count how many times it fits
  * Construct quotient one digit at a time
* **Error Handling**:

  * Division by zero is detected and reported
  * Division truncates to integer result
* **Time Complexity**: O(n²)

---

## Usage

When running the program:

1. Choose an operation (1–4)
2. Enter two numbers (each preceded by `+`, `-`, or space)
3. View the result
4. Continue with more calculations or exit (option 5)

---

## Memory Management

* Uses **dynamic memory allocation** for all `BigInt` arrays
* Includes cleanup functions to **free memory** after each operation
* All allocations checked for **null pointers** to prevent crashes
* Temporary variables and results are properly deallocated

---

## Notes on Karatsuba Usage

* Automatically switches to **Karatsuba multiplication** if input size exceeds a certain threshold (32 in our case)
* Falls back to grade-school multiplication for smaller inputs to reduce overhead

---
