# ZenGin DAT

!!! abstract inline end "Quick Infos"
    **Type:** Script Format<br/>
    **Format Name:** DAT<br/>
    **File Extension:** `.DAT`<br/>
    **Class Name:** `zCParser`<br/>
    **Encoding:** [Binary](../encodings/binary.md)<br/>

*ZenGin* DAT files contain symbols and [bytecode](./bytecode.md) used by the VM of the engine.
DAT files are the result of Daedalus script compilation process.

## Format Description

Compiled scripts are stored in a [binary](../encodings/binary.md) file which contains the following data. Also refer to the
[Datatype Reference](../datatypes.md) for general information about often used datatypes.

```c title="DAT Structure"
struct DatFile {
    byte version;

    uint symbolCount;
    uint symbolIds[/* symbolCount */]; // (1)
    zCPar_Symbol symbolTable[/* symbolCount */];

    uint bytecodeSize;
    byte bytecode[/* bytecodeSize */]; // (2)
};
```

1. Symbol IDs sorted lexicographically
2. Read [bytecode](./bytecode.md) page for more info about this data.

### Symbol

```c title=""
struct zCPar_Symbol {
    byte isNamed; // (1) 
    SymbolName name;

    SymbolParameters params;
    SymbolCodeSpan span;
    SymbolData data;
    int parent; // (2)
};
```

1. Does this symbol have a name
2. Parent type, `-1` means `None`

#### Symbol Name

```c title="If isNamed == 1"
struct SymbolName {
    string name;
};
```
```c title="If isNamed == 0"
struct SymbolName {};
```

#### Symbol Parameters

```c title=""
struct SymbolParameters {
    int offset; // (1)
    uint count: 12; // (2)
    zPAR_TYPE type: 4;
    zPAR_FLAG flags: 6;
    uint space: 1;
    uint reserved: 9;
};

enum zPAR_TYPE: uint {
    VOID = 0,
    FLOAT = 1,
    INT = 2,
    STRING = 3,
    CLASS = 4,
    FUNC = 5,
    PROTOTYPE = 6,
    INSTANCE = 7,
};

// Parameter bitflags
enum zPAR_FLAG: uint {
    CONST = 1,
    RETURN = 2,
    CLASSVAR = 4,
    EXTERNAL = 8,
    MERGED = 16
};
```

1. Depending on `type` either Offset (ClassVar), Size (Class) or ReturnType (Func)
2. How many sub items does this symbol have

#### Symbol Code Span
This is span debug information, pointing at the source daedalus script

```c title=""
struct SymbolCodeSpan {
    uint fileIndex: 19;
    uint reserved: 13;
    uint lineStart: 19;
    uint reserved: 13;
    uint lineCount: 19;
    uint reserved: 13;
    uint columnStart: 24;
    uint reserved: 8;
    uint columnCount: 24;
    uint reserved: 8;
};
```

#### Symbol Data

This depends on `params.flags`, if `CLASSVAR` flag is set this data will always be 0 sized.
=== "If `CLASSVAR` flag is not set"
    It also depends on `params.type` of the symbol.
    ```c title="For FLOAT"
    struct SymbolData {
        float value[/* params.count */];
    };
    ```
    ```c title="For INT"
    struct SymbolData {
        int value[/* params.count */];
    };
    ```
    ```c title="For STRING"
    struct SymbolData {
        string value[/* params.count */];
    };
    ```

    !!! warning
        Some mods don't terminate those strings correctly, look at this [code](https://github.com/GothicKit/ZenKit/commit/0e7e507de92e8da4ec28513e6be56e4043329990)
        and the issue related to it.
    ```c title="For CLASS"
    struct SymbolData {
        int classOffset;
    };
    ```
    ```c title="For INSTANCE, FUNCTION, PROTOTYPE"
    struct SymbolData {
        int address;
    };
    ```
    ```c title="For VOID"
    struct SymbolData {}
    ```

=== "If `CLASSVAR` flag is set"
    ```c title=""
    struct SymbolData {}
    ```

