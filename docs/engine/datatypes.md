# Datatype Reference

This page contains a set of commonly used datatypes and their definition.

| Datatype   | Size | Description                                                                                                                                                            |
|------------|------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `byte`     | 1    | A single, unsigned 8-bit integer                                                                                                                                       |
| `char`     | 1    | A single, 8-bit integer representing a string character                                                                                                                |
| `short`    | 2    | A single, signed 16-bit integer                                                                                                                                        |
| `ushort`   | 2    | A single, unsigned 16-bit integer                                                                                                                                      |
| `int`      | 4    | A single, signed 32-bit integer                                                                                                                                        |
| `uint`     | 4    | A single, unsigned 32-bit integer                                                                                                                                      |
| `float`    | 4    | A single, [IEEE 754](https://en.wikipedia.org/wiki/IEEE_754) floating point number                                                                                     |
| `string`   | *    | A set of [Windows-1252](https://en.wikipedia.org/wiki/Windows-1252) encoded characters of dynamic length,<br/>terminated by `\0` or `0x0A` (exceptions are documented) |
| `zVEC2`    | 8    | A set of two `float`s describing a 2-dimensional vector of the form `(x, y)`                                                                                           |
| `zVEC3`    | 12   | A set of three `float`s describing a 3-dimensional vector of the form `(x, y, z)`                                                                                      |
| `zVEC4`    | 16   | A set of four `float`s describing a 4-dimensional vector or quaternion of the form `(x, y, z, w)`                                                                      |
| `zTBBox3D` | 24   | A set of two `zVEC3`s describing a 3-dimensional bounding box of the form `(min, max)`                                                                                 |
| `zDATE`    | 16   | A structure describing a date: `#!c struct {int year; ushort month, day, hour, minute, second; byte _padding[2];}`                                                     |
