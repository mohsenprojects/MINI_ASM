# MINI_ASM Programming Language Documentation

## Overview
MINI_ASM is a lightweight programming language designed for low-level memory control and efficient code generation. It provides direct access to different memory segments while maintaining a simple syntax.

## Variable Declaration

### Data Types
MINI_ASM supports several primitive data types:

#### Integer Types:
- `int8` (8-bit signed integer)
- `int16` (16-bit signed integer)
- `int32` (32-bit signed integer, default)
- `int64` (64-bit signed integer)

#### Other Types:
- `float` (32-bit floating point)
- `double` (64-bit floating point)
- `string` (null-terminated character sequence)
- `bool` (1-byte boolean value)

### Memory Segments
Variables can be stored in three memory segments:

1. **Stack Segment** (`stack` or `stk`):
   - Default storage location
   - Automatically managed (LIFO)
   - Fast access
   - Limited size
   - Example: `int32 stack counter = 0`

2. **Data Segment** (`data` or `dt`):
   - Persistent storage
   - Fixed memory allocation
   - Global scope
   - Example: `string data greeting = "Hello"`

3. **Heap Segment** (`heap` or `h`):
   - Dynamic memory allocation
   - Manual management required
   - Flexible size
   - Example: `int64 heap buffer = malloc(1024)`

### Syntax Examples

#### Basic Declarations:
```rust
// Default 32-bit integer on stack
int32 value = 42

// 64-bit floating point in data segment
double data pi = 3.1415926535

// 8-bit boolean on heap
bool heap flag = true
```

#### Array Declarations:
```rust
// Stack array of 10 integers
int32[10] stack numbers = 0

// Data array of strings
string[3] data messages = ["Hello", "MINI_ASM", "World"]

// Heap array of doubles
double[100] heap samples = malloc(100*8)
```

#### Special Cases:
```rust
// String with escape sequences
string data path = "C:\\Programs\\MINI_ASM\n"

// Explicit initialization
int64[4] stack matrix = [1, 2, 3, 4]

// Partial initialization (remaining elements zero)
float[10] data temps = [25.5, 26.0, 27.3]
```

## Advanced Features

### Type Aliases:
```rust
// Create type alias
type int = int32
type byte = int8
type real = double

// Use aliases
int stack count = 10
byte heap data = 0xFF
real data pi = 3.14159
```

### Constant Expressions:
```rust
// Compile-time constants
const int32 MAX_SIZE = 1024
const double TAX_RATE = 0.15

// Used in declarations
int32[MAX_SIZE] stack buffer
```

### Memory Operations:
```rust
// Explicit memory management
int64* heap ptr = malloc(256)
free(ptr)

// Memory copy operation
memcpy(dest, src, size)
```

## Best Practices

1. Use stack for small, short-lived variables
2. Use data segment for global constants and configuration
3. Use heap for large or dynamically-sized data
4. Always free heap memory when no longer needed
5. Initialize arrays explicitly when possible

## Complete Example

```rust
// Program to calculate circle properties
const double PI = 3.1415926535

function calculateArea(double radius) -> double
    return PI * radius * radius
end

function main()
    double stack radius = 5.0
    double stack area = calculateArea(radius)
    
    string data message = "Area of circle with radius "
    printf("%s%f is %f\n", message, radius, area)
end
```

This documentation covers the complete syntax for variable declaration and memory management in MINI_ASM. The language provides fine-grained control over memory usage while maintaining readable syntax for efficient low-level programming.
