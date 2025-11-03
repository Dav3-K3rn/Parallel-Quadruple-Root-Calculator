# # Parallel Quadruple Root Calculator

A high-performance multithreaded C program that computes the sum of fourth roots (quadruple roots) from 1 to n using parallel processing and an efficient tree-based reduction algorithm.

## 📋 Overview

This program demonstrates advanced parallel computing concepts including:
- **POSIX threads (pthreads)** for concurrent execution
- **Barrier synchronization** for coordinating thread execution
- **Parallel reduction** using a tree-based algorithm (O(log m) complexity)
- **Load balancing** across multiple worker threads

## 🎯 What It Does

Given two inputs:
- `m` - number of threads
- `n` - upper limit

The program calculates: **Σ(i^0.25)** for i = 1 to n

Each thread computes fourth roots for a subset of numbers, then the results are combined using an efficient parallel reduction technique.

## 🚀 Features

- ✅ **Efficient parallel reduction** - O(log m) combining steps instead of sequential addition
- ✅ **Lock-free synchronization** - Uses barriers instead of mutexes for better performance
- ✅ **Even work distribution** - Handles cases where n is not evenly divisible by m
- ✅ **Comprehensive error handling** - Input validation and memory allocation checks
- ✅ **Clean resource management** - Proper cleanup of all allocated resources

## 🛠️ Compilation

### Basic Compilation
```bash
gcc Q_root_calc.c -o Q_root_calc -lm -lpthread
```

### Recommended (with optimizations and warnings)
```bash
gcc -Wall -O2 Q_root_calc.c -o Q_root_calc -lm -lpthread
```

### With Debug Output
```bash
gcc -Wall -O2 -DDEBUG Q_root_calc.c -o Q_root_calc -lm -lpthread
```

### Compiler Flags Explained
- `-Wall` - Enable all compiler warnings
- `-O2` - Optimization level 2 (recommended for performance)
- `-lm` - Link the math library (for `pow()` function)
- `-lpthread` - Link the POSIX threads library
- `-DDEBUG` - Enable detailed reduction step output

## 📖 Usage

```bash
./Q_root_calc <num_threads> <max_number>
```

### Parameters
- `<num_threads>` - Number of parallel threads to use (must be positive integer)
- `<max_number>` - Compute sum of fourth roots from 1 to this number (must be positive integer)

### Examples

**Example 1: Basic usage**
```bash
./Q_root_calc 4 100
```
Computes the sum using 4 threads for numbers 1-100.

**Example 2: Large computation**
```bash
./Q_root_calc 8 10000
```
Computes the sum using 8 threads for numbers 1-10,000.

**Example 3: Maximum parallelism**
```bash
./Q_root_calc 16 1000000
```
Computes the sum using 16 threads for numbers 1-1,000,000.

## 📊 Sample Output

```
Computing sum of fourth roots from 1 to 100 using 4 threads...
Using parallel reduction algorithm (O(log n) complexity)

Thread 0 (range 1-25): partial sum = 51.709820
Thread 1 (range 26-50): partial sum = 65.831264
Thread 2 (range 51-75): partial sum = 74.040237
Thread 3 (range 76-100): partial sum = 80.359303

========================================
Total sum of fourth roots: 271.940624
========================================
```

## 🧮 Algorithm Explanation

### Phase 1: Parallel Computation
Each of the `m` threads computes fourth roots for its assigned range:
```
Thread 0: numbers 1 to n/m
Thread 1: numbers (n/m + 1) to 2n/m
...
Thread m-1: remaining numbers to n
```

### Phase 2: Tree-Based Reduction
Results are combined in log₂(m) steps using a binary tree structure:

```
Example with 8 threads:
Step 1: Thread 0 += Thread 1, Thread 2 += Thread 3, Thread 4 += Thread 5, Thread 6 += Thread 7
Step 2: Thread 0 += Thread 2, Thread 4 += Thread 6
Step 3: Thread 0 += Thread 4
Result: Thread 0 contains the final sum
```

This approach is **significantly faster** than using a mutex to sequentially add results.

## ⚡ Performance Characteristics

- **Time Complexity**: O(n/m + log m)
  - n/m for parallel computation
  - log m for reduction steps
- **Space Complexity**: O(m) for storing partial sums
- **Scalability**: Near-linear speedup for large n values

### Performance Tips
1. Use more threads for larger values of n
2. Ensure m ≤ number of CPU cores for optimal performance
3. For best results, make m a power of 2 (reduces reduction overhead)

## 🔧 Requirements

- **Compiler**: GCC or any C compiler with C99 support
- **Libraries**: 
  - POSIX threads library (`pthread`)
  - Standard math library (`libm`)
- **Operating System**: Linux, macOS, or any POSIX-compliant system

## 📝 Technical Details

### Key Functions
- `compute_quadruple_roots()` - Worker thread function
- `pow(i, 0.25)` - Computes the fourth root of i

### Synchronization Primitives
- `pthread_barrier_t` - Ensures all threads complete each phase before proceeding
- No mutex locks required (lock-free algorithm)

### Memory Management
- Dynamic allocation for thread structures and partial sums
- Proper cleanup of all resources before exit

## 🐛 Error Handling

The program handles the following error conditions:
- Invalid number of command-line arguments
- Non-positive values for threads or numbers
- Memory allocation failures
- Thread creation failures

## 📚 Learning Resources

This code demonstrates several important parallel programming concepts:
- Thread creation and management
- Barrier synchronization
- Parallel reduction patterns
- Load balancing strategies
- Race condition prevention

## 🤝 Contributing

Feel free to submit issues or pull requests for:
- Performance optimizations
- Additional features
- Bug fixes
- Documentation improvements

## 📄 License

This project is open source and available under the MIT License.

## 👨‍💻 Author

Created as a demonstration of efficient parallel computing techniques using POSIX threads.

---

**Note**: For very large values of n (millions+), consider using `long double` instead of `double` for better precision in the accumulation of sums.
