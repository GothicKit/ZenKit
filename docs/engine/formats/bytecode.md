# ZenGin Bytecode

Bytecode is read from [`DatFile.bytecode`](./script_binaries.md#format-description) field

```c title="Bytecode Buffer"
struct Bytecode {
    Instruction instructions[/* Read until there is no bytes left in the buffer */]
};
```

## Instruction

```c title=""
struct Instruction {
    Opcode opcode,
    InstructionData data,
};
```

### Opcode

??? "enum Opcode: uint { ... }"

    Unused variants are commented out
    ```c title=""
    enum Opcode: uint {
        zPAR_OP_PLUS         = 0,
        zPAR_OP_MINUS        = 1,
        zPAR_OP_MUL          = 2,
        zPAR_OP_DIV          = 3,
        zPAR_OP_MOD          = 4,
        zPAR_OP_OR           = 5,
        zPAR_OP_AND          = 6,
        zPAR_OP_LOWER        = 7,
        zPAR_OP_HIGHER       = 8,
        zPAR_OP_IS           = 9,
        zPAR_OP_LOG_OR       = 11,
        zPAR_OP_LOG_AND      = 12,
        zPAR_OP_SHIFTL       = 13,
        zPAR_OP_SHIFTR       = 14,
        zPAR_OP_LOWER_EQ     = 15,
        zPAR_OP_EQUAL        = 16,
        zPAR_OP_NOTEQUAL     = 17,
        zPAR_OP_HIGHER_EQ    = 18,
        zPAR_OP_ISPLUS       = 19,
        zPAR_OP_ISMINUS      = 20,
        zPAR_OP_ISMUL        = 21,
        zPAR_OP_ISDIV        = 22,
        // zPAR_OP_UNARY        = 30,
        zPAR_OP_UN_PLUS      = 30,
        zPAR_OP_UN_MINUS     = 31,
        zPAR_OP_UN_NOT       = 32,
        zPAR_OP_UN_NEG       = 33,
        // zPAR_OP_MAX          = 33,
        // zPAR_TOK_BRACKETON   = 40,
        // zPAR_TOK_BRACKETOFF  = 41,
        // zPAR_TOK_SEMIKOLON   = 42,
        // zPAR_TOK_KOMMA       = 43,
        // zPAR_TOK_SCHWEIF     = 44,
        zPAR_TOK_NONE        = 45,
        // zPAR_TOK_FLOAT       = 51,
        // zPAR_TOK_VAR         = 52,
        // zPAR_TOK_OPERATOR    = 53,
        zPAR_TOK_RET         = 60,
        zPAR_TOK_CALL        = 61,
        zPAR_TOK_CALLEXTERN  = 62,
        // zPAR_TOK_POPINT      = 63,
        zPAR_TOK_PUSHINT     = 64,
        zPAR_TOK_PUSHVAR     = 65,
        // zPAR_TOK_PUSHSTR     = 66,
        zPAR_TOK_PUSHINST    = 67,
        // zPAR_TOK_PUSHINDEX   = 68,
        // zPAR_TOK_POPVAR      = 69,
        zPAR_TOK_ASSIGNSTR   = 70,
        zPAR_TOK_ASSIGNSTRP  = 71,
        zPAR_TOK_ASSIGNFUNC  = 72,
        zPAR_TOK_ASSIGNFLOAT = 73,
        zPAR_TOK_ASSIGNINST  = 74,
        zPAR_TOK_JUMP        = 75,
        zPAR_TOK_JUMPF       = 76,
        zPAR_TOK_SETINSTANCE = 80,
        // zPAR_TOK_SKIP        = 90,
        // zPAR_TOK_LABEL       = 91,
        // zPAR_TOK_FUNC        = 92,
        // zPAR_TOK_FUNCEND     = 93,
        // zPAR_TOK_CLASS       = 94,
        // zPAR_TOK_CLASSEND    = 95,
        // zPAR_TOK_INSTANCE    = 96,
        // zPAR_TOK_INSTANCEEND = 97,
        // zPAR_TOK_NEWSTRING   = 98,
        zPAR_TOK_FLAGARRAY   = zPAR_TOK_VAR + 128
    };
    ```

### Instruction Data

Depending on `opcode` the data may be different or in most cases 0 sized.

```c title="For CALL, JUMPF, JUMP"
struct InstructionData {
    uint address;
};
```

```c title="For PUSHINT"
struct InstructionData {
    int immediate;
};
```

```c title="For CALLEXTERN, PUSHVAR, PUSHINST, SETINSTANCE"
struct InstructionData {
    uint symbol;
};
```

```c title="For PUSHVAR + FLAGARRAY"
struct InstructionData {
    uint symbol;
    byte index;
};
```

```c title="For any other Opcode"
struct InstructionData {};
```

