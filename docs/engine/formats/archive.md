# ZenGin Archives

*ZenGin* archives (*ZEN* files) contain generic data for use with the *ZenGin*. Most archives store key-value pairs
organized into objects, similar to [JSON](https://en.wikipedia.org/wiki/JSON). Archives have three different encodings:
[binary](#binary) archives store unannotated bytes, [safe binary](#safe-binary) archives store binary data annotated
with field types and names and [ascii](#ascii) archives contain a human-readable form of the same data stored in
*safe binary* archives.

## General Concepts

A basic archive file might look like this.

```text title="Example ZEN archive (ASCII)"
ZenGin Archive
ver 1
zCArchiverGeneric
ASCII
saveGame 0
date 17.9.2002 15:21:1
user mario.roeske
END
objects 7        
END
[% oCWorld:zCWorld 64513 0]
	[VobTree % 0 0]
		childs0=int:1
		[% zCVob 39168 1]
			pack=int:0
			presetName=string:LIGHT
			bbox3DWS=rawFloat:334.003906 -443.912476 -7493.55371 2734.00391 1956.08765 -5093.55371 
			trafoOSToWSRot=raw:974226bf00000000a9a9423f000000000000803f00000000a9a942bf00000000974226bf
			trafoOSToWSPos=vec3:1534.00403 756.087585 -6293.55371
			vobName=string:LIGHT
			visual=string:
			showVisual=bool:0
			visualCamAlign=enum:0
			visualAniMode=enum:0
			visualAniModeStrength=float:0
			vobFarClipZScale=float:1
			cdStatic=bool:0
			cdDyn=bool:0
			staticVob=bool:1
			dynShadow=enum:0
			zbias=int:1
			isAmbient=bool:0
			[visual % 0 0]
			[]
			[ai % 0 0]
			[]
		[]
	[]
[]
```

At the beginning of the file, the [header](#header) is defined. It consists of two parts, the first one being the same
across all encoding and the second being specific to the encoding used. The data content of the file starts right after
the second `END`. The first line of the file's content, `[% oCWorld:zCWorld 64513 0]`, marks the start of a new
[object](#objects) while the last line, `[]` marks the end of an [object](#objects).

Each [object](#objects) can contain multiple [entries](#entries) alongside other [objects](#objects). Each
[entry](#entries) has a name, a type and a value. This only applies to [ascii](#ascii) and [safe binary](#safe-binary)
encodings.

## Header

The header of ZEN archives consists of two parts. The first has the same structure for every archive and the second
differs between encodings. The first part of the header looks like this. Variable elements of the header have been
replaced by `<...>` placeholders.

```text title="Example ZEN archive header"
ZenGin Archive
ver 1
<archiver>
<encoding>
saveGame <issave>
date <timestamp>
user <username>
END
```

Line breaks denote only one newline (`\n`) character. The `<archiver>` is a string value, being either
`zCArchiverGeneric` or `zCArchiverBinSafe`, in reference to the names of the ZEN archive parser classes in the
original source code. `<encoding>` specifies the type of archive in the file. It is either `ASCII`
(for [ascii](#ascii) encoded archives), `BINARY` (for [binary](#binary) encoded archives) or `BIN_SAFE`
(for [safe binary](#safe-binary) encoded archives). If the archive is a save game file `<issave>` is set to `1`,
otherwise it is `0`. `<timestamp>` denotes the date and time the archive was created. It follows the
[strptime](https://linux.die.net/man/3/strptime) format `%d.%m.%Y %H:%M:%S`. `<username>` is the name of the user
(originally the windows account name) the archive was created by. For save games this contains the name of the user
who saved the game. Refer to the [example](#general-concepts) above for a filled-out version of the header.

The encoding-specific headers follow directly after the `END\n` line of the first header as can be seen in the
example.

## Objects

Objects in ZEN archives consist of an *object name*, a *class name*, a *version* identifier and an *index*. The object
name is used to describe a unique sub-object of another while the class name is used to identify the type of the object.
A ZEN archive object `[visual zCParticleFX 0 6]` describes a specific sub-object `visual` of type `zCParticleFX`
meaning it has a set of [entries](#entries) consistent with the definition of a `zCParticleFX`. Objects without an
object name just contain a `%` instead. Objects always end with `[]`.

!!! note
    This behavior can be seen in the example [above](#general-concepts). Every object with the class name `zCVob` has
    the same set of entries as shown in the example.

!!! danger
    The object name must be unique within each sub-object. A ZEN archive like this is invalid.

    ```text
    [% oCWorld:zCWorld 64513 0]
        [VobTree % 0 0]
        []
        [VobTree % 0 0]
        []
    []
    ```

Class names originally referenced a specific class in C++-code. These classes were polymorphic so an object with a class
name `zCVobLight:zCVob` contains both [entries](#entries) of type `zCVob` and `zCVobLight`. The entries of the
super-class always precede the [entries](#entries) of the subclass, so the `zCVobLight:zCVob` would first contain all
[entries](#entries) of `zCVob` and then all [entries](#entries) of `zCVobLight`. Objects without a class name just
contain a `%` instead.

!!! warning
    Class and object names may not contain spaces.

The *version* of an object is a 32-bit unsigned number which denotes the version of the bottom-most subclass. The
version of object `[% oCMobInter:oCMOB:zCVob 35585 0]` would be `35585`. This version, however, only refers to the
`oCMobInter`-part.

!!! danger
    This leads to issues when trying to determine whether an object uses the *Gothic* or the *Gothic II* definition.
    If the structure for `oCMobInter` did not change between the games, the version will be the same. This is not
    necessarily true for `zVob` though. Its definition might have changed but this change is not reflected in the
    `version` field of the object.

The *index* field of the object is a 32-bit unsigned number which increments for every object with a class name in the
archive. In the [example](#general-concepts) this can be seen with the objects `[% oCWorld:zCWorld 64513 0]` and
`[% zCVob 39168 1]`. Objects without a class name have an *index* of `0`.

## Entries

The concept of entries only really applies to archives with [ascii](#ascii) or [safe binary](#safe-binary) encoding.
An entry originally represented a field in a C++-class which defined an [object](#objects). For this reason, each entry
has a name, a type and a value. The following entry types are supported:

| Type Name  | Type ID[^1] | Description                                                                                               |
|------------|-------------|-----------------------------------------------------------------------------------------------------------|
| `string`   | 1           | [ISO-8859-1](https://en.wikipedia.org/wiki/ISO/IEC_8859-1) encoded text                                   |
| `int`      | 2           | A 32-bit signed integer                                                                                   |
| `float`    | 3           | An [IEEE 754](https://en.wikipedia.org/wiki/IEEE_754) floating-point number                               |
| `byte`     | 4           | An 8-bit unsigned integer                                                                                 |
| `word`     | 5           | An 16-bit unsigned integer                                                                                |
| `bool`     | 6           | An boolean value[^2]                                                                                      |
| `vec3`     | 7           | A set of 3 [IEEE 754](https://en.wikipedia.org/wiki/IEEE_754) floating-point numbers                      |
| `color`    | 8           | A set of 4 8-bit unsigned integers                                                                        |
| `raw`      | 9           | A set of an arbitrary amount of 8-bit unsigned integers                                                   |
| `rawFloat` | 16          | A set of an arbitrary amount of [IEEE 754](https://en.wikipedia.org/wiki/IEEE_754) floating-point numbers |
| `enum`     | 17          | A 32-bit unsigned integer[^3]                                                                             |
| `hash`     | 18          | A 32-bit unsigned integer; specific to the [safe binary](#safe-binary) encoding                           |

---

## ASCII

ASCII-archives are human-readable ZEN archives, comparable to JSON. Contrary to its name, the file is not actually
using the [ASCII](https://en.wikipedia.org/wiki/ASCII) text encoding. Rather these files are encoded in
[ISO-8859-1](https://en.wikipedia.org/wiki/ISO/IEC_8859-1) or alternatively
[Windows-1252](https://en.wikipedia.org/wiki/Windows-1252) (because the characters used are encoded identically).

ASCII ZEN-archives use a line-based format. Every line after the header contains exactly one object start statement,
entry statement or object end token. Leading whitespace is ignored.

### Header

The encoding specific header for the *ASCII*-encoding looks like this. Variable elements of the header have been
replaced by `<...>` placeholders.

```text
objects <count>
END
```

`<count>` denotes the total number of [objects](#objects) in the archive. Refer to the [example](#general-concepts)
above for a filled-out version of the header.

### Objects

As stated above, objects in ASCII-ZENs start with a line `[% oCWorld:zCWorld 64513 0]` defined as
`[<object name> <class name> <version> <index>]`. This is explained further in the section about [objects](#objects).

### Entries

Entries in ASCII-ZENs follow the format `<name>:<type>=<value>` where `<name>` is a C++ identifier, `<type>` is one
of the type names mentioned in the [entries](#entries) section with the exclusion of the `hash` type. `<value>` depends
on the given type. It has the following formats:

| Type Name  | Format                                                         | Regex                                                  | Example                         |
|------------|----------------------------------------------------------------|--------------------------------------------------------|---------------------------------|
| `string`   | An unlimited number of characters excluding newline characters | `^[^\n\r]*`                                            | `field=string:This is a string` |
| `int`      | A decimal 32-bit integer                                       | `^[+-]?\d+$`                                           | `field=int:199`                 |
| `float`    | A decimal floating point value with decimal separator `.`      | `^[+-]?(\d+\.)?\d+$`                                   | `field=float:-1.0`              |
| `byte`     | A decimal 8-bit unsigned integer                               | `^\d{1,3}$`                                            | `field=byte:255`                |
| `word`     | A decimal 16-bit unsigned integer                              | `^\d+$`                                                | `field=word:1001`               |
| `bool`     | A decimal 1-bit integer                                        | `^[01]$`                                               | `field=bool:1`                  |
| `vec3`     | A set of three floating point values                           | `^[+-]?(\d+\.)?\d+ [+-]?(\d+\.)?\d+ [+-]?(\d+\.)?\d+$` | `field=vec3:1.0 -0.1 +100`      |
| `color`    | A set of four 8-bit unsigned integers                          | `^\d{1,3} \d{1,3} \d{1,3} \d{1,3}$`                    | `field=color:100 0 255 12`      |
| `raw`      | An unlimited number of hexadecimal bytes                       | `^([0-9A-Fa-f]{2})*`                                   | `field=raw:0fac3256b1`          |
| `rawFloat` | An unlimited number of floating point values                   | `^([+-]?(\d+\.)?\d+ )*`                                | `field=rawFloat:1.0 -100 `      |
| `enum`     | A decimal 32-bit unsigned integer                              | `^\d+$`                                                | `field=enum:10`                 |

---

## Binary

Binary archives are little more than containers for raw bytes. They don't actually contain key-value pairs.

### Header

The encoding specific header for the *Binary*-encoding is the same as that of the [ascii](#ascii) encoding. Variable
elements of the header have been replaced by `<...>` placeholders.

```text
objects <count>
END
```

`<count>` denotes the total number of [objects](#objects) in the archive.

### Objects

Objects in binary ZENs don't work like they do in the [ascii](#ascii) and [safe binary](#safe-binary) encodings. No
object end marker is stored and the beginning of an object does not follow the same format.

```c title="Binary archive object begin structure"
#pragma pack(push, 1)
struct zen_object_begin {
    uint32_t object_size; //the size of the object in bytes, including the header.
    uint16_t version; // the *version* number of the object
    uint32_t index; // the *index* of the object
    char* object_name; // The *name* of the object including a null-termination character
    char* class_name; // The *class* of the object including a null-termination character
};
#pragma pack(pop)
```

### Entries

Binary ZENs just contains packed bytes with no added type annotations. This makes it very difficult to determine the
layout of structures within it without having access to the original source code.

To read an integer from a binary ZEN for example, the next four bytes are read in literally like this (assuming a
little-endian architecture).

```c title="Binary archive entry read scalar example"
int32_t zen_binary_read_int(FILE* fp) {
    int32_t value = 0;
    fread(&value, sizeof(value), 1, fp);
    return value;
}
```

!!! note
    On big-endian architectures, the value read would have to be byte-swapped after reading.

This method of extracting values from binary archives is also used for aggregate types, like the `vec3`. Instead of
reading just one float from the input, three are read like this.

```c title="Binary ZEN entry read vector example"
typedef struct vec3 {
    float x;
    float y;
    float z;
} vec3_t;

vec3_t zen_binary_read_int(FILE* fp) {
    vec3_t value;
    fread(&value.x, sizeof(float), 1, fp);
    fread(&value.y, sizeof(float), 1, fp);
    fread(&value.z, sizeof(float), 1, fp);
    return value;
}
```

Refer to the following table for type sizes.

| Type Name  | C-equivalent                                    | Byte Count |
|------------|-------------------------------------------------|------------|
| `string`   | `char[]`                                        | `1 * n`    | 
| `int`      | `int32_t`                                       | 4          | 
| `float`    | `float`                                         | 4          | 
| `byte`     | `uint8_t`                                       | 1          | 
| `word`     | `uint16_t`                                      | 2          | 
| `bool`     | `uint8_t`                                       | 1          | 
| `vec3`     | `struct { float, float, float }`                | `4 * 3`    | 
| `color`    | `struct { uint8_t, uint8_t, uint8_t, uint8_t }` | `1 * 4`    | 
| `raw`      | `uint8_t[]`                                     | `1 * n`    | 
| `rawFloat` | `float[]`                                       | `4 * n`    | 
| `enum`     | `uint32_t`                                      | 4          | 

---

## Safe Binary

Safe binary archives store binary values annotated with field types and names. They are the most prevalent types of
archives and also the ones in which world data is stored. The values of entries as well as their prefixes are stored
in a packed binary format. This encoding is not fully reverse-engineered.

### Header

The header of safe binary archives is also binary and has the following structure.

```c title="Safe binary archive header structure"
#pragma pack(push, 1)
struct zen_binsafe_header {
    uint32_t bs_version; // A separate version identifier for the version of the safe binary encoding used.
    uint32_t object_count; // The total number of objects in the archive
    uint32_t hash_table_offset; // The offset of the hash table from the beginning of the file in bytes; discussed below.
};
#pragma pack(pop)
```

In addition to the secondary header, safe binary archives also contain a so-called hash table[^4] which stores field
name information. The hash table is a binary structure as well; it looks like this.

```c title="Safe binary ZEN hash table structure"
#pragma pack(push, 1)
struct zen_binsafe_hashtable_entry {
    uint16_t name_length; // The length of the name string at the end of the structure
    uint16_t insertion_index; // The index of this entry in the hashtable `entries` member array
    uint32_t hash_value; // The hash value of the entry. Its purpose is currently unknown.
    char name[]; // The name of the entry. This name is the field name of the entry as explained below
};

struct zen_binsafe_hashtable {
    uint32_t size; // The number of entries in the hash table. Corresponds with the length of the `entries` member array.
    zen_binsafe_hashtable_entry entries[]; // An array of all hash table entries; unordered.
};
#pragma pack(pop)
```

### Objects

In safe binary ZENs, objects are handled in almost the same way entries are. Objects definitions are preceded by the
string type identifier `\x01` and a 16-bit string length value. The value of this string is the same as the object
definition for ASCII-ZENs: Objects start with a line `[% oCWorld:zCWorld 64513 0]` defined as
`[<object name> <class name> <version> <index>]`. This is explained further in the section about [objects](#objects).

A full object definition might look like this: `\x01\x1B\x00[% oCWorld:zCWorld 64513 0]`.

!!! note
    Note that objects are never preceded by a field name like entries are.

### Entries

Every entry in safe binary ZENs is preceded by a field name index and a type identifier including a length if
applicable. File name indexes start with a `\x12` which is followed by a 32-bit unsigned integer, indicating an index
into the hash table discussed above. The name of the hash table entry at that index is the name of the field.

A boolean entry in this format might look like `\x12\x01\x00\00\x00\x06\x01\x00\x00\x00`. It hashtable index is `1`
and its value is `true`. Type IDs are always `uint8_t`s and hashtable indices are always `uint32_t`s.

Reference the table below for all types in safe binary archives.

| Type Name  | Type ID | Has Length? | C-equivalent                                    | Byte Count |
|------------|---------|-------------|-------------------------------------------------|------------|
| `string`   | 1       | Yes         | `char[]`                                        | `1 * n`    | 
| `int`      | 2       | No          | `int32_t`                                       | 4          | 
| `float`    | 3       | No          | `float`                                         | 4          | 
| `byte`     | 4       | No          | `uint8_t`                                       | 1          | 
| `word`     | 5       | No          | `uint16_t`                                      | 2          | 
| `bool`     | 6       | No          | `uint32_t`                                      | 4          | 
| `vec3`     | 7       | No          | `struct { float, float, float }`                | `4 * 3`    | 
| `color`    | 8       | No          | `struct { uint8_t, uint8_t, uint8_t, uint8_t }` | `1 * 4`    | 
| `raw`      | 9       | Yes         | `uint8_t[]`                                     | `1 * n`    | 
| `rawFloat` | 16      | Yes         | `float[]`                                       | `4 * n`    | 
| `enum`     | 17      | No          | `uint32_t`                                      | 4          |

!!! note
    Entries with a length prefix, are encoded like this: `\x12\x01\x00\00\x00\x01\x0D\x00Hello, world!`. It hashtable
    index is `1`, its length is `0x0D` and its value is `Hello, World!`. Note that the length is always a `uint16_t`
    and always follows directly after the type ID.

[^1]: The type index is relevant only for [safe binary](#safe-binary) encoded archives.
[^2]: The actual representation of this value changes between encodings. With the [safe binary](#safe-binary) encoding
a boolean value is represented as a 32-bit unsigned integer while it is represented as an 8-bit integer in the
[binary](#binary) encoding.
[^3]: Enums represent an actual enumeration value in C++-code.
[^4]: The origin of that name is unknown. It has been used in
[ZenLib](https://github.com/ataulien/ZenLib/blob/master/zenload/parserImplBinSafe.cpp#L190-L195) to describe this
element of the format.
