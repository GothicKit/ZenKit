# ZenGin Virtual File System

!!! abstract inline end "Quick Infos"
    **Type:** Asset Format<br/>
    **Format Name:** Virtual File System<br/>
    **File Extension:** `.VDF`<br/>
    **Class Name:** `zFILE_VDFS` [^3] <br/>
    **Encoding:** [Binary](../encodings/binary.md)<br/>

*VDF* is a container format for a file/directory structure, similar to the
[tar](https://en.wikipedia.org/wiki/Tar_(computing)) format. *VDF* files are not intended to be used for general file
transfer, and thus they don't contain metadata like permission information which can usually be found in such container
formats.

VDF files can usually be found in the `Data/` directory of Gothic installations.

## Format Description

*VDF* files consist of a [header](#header), the [catalog](#catalog) and a data section which are described below. Also
refer to the [Datatype Reference](../datatypes.md) for general information about often used datatypes.

```c title="VDF Structure"
struct zFILE_VDFS {
    zFILE_VDFS_Header header;
    zFILE_VDFS_Catalog catalog;
    byte data[];
};
```

### Header

The *VDF*s header describes the file and its contents. It always has the same physical size and can be represented as
a C struct like this.

```c title="VDF Header"
struct zFILE_VDFS_Header {
    char comment[256]; // (1)
    char signature[16]; // (2)
    uint numEntries;
    uint numFiles;
    uint timestamp; // (3)
    uint size; // (4)
    uint catalogOffset; // (5)
    uint version; // (6)
};
```

1. The comment might not fill the entire 256 bytes. If it does not, the rest of the array is filled with `'\x1A'`
   characters.
2. The signature indicates which game this VDF file was made for. If it is `"PSVDSC_V2.00\r\n\r\n"` then the VDF was
   made for *Gothic* and if it is `"PSVDSC_V2.00\n\r\n\r"` it was made for *Gothic II*.
3. The timestamp has a special format called "DOS date format" by all sources[^1]. It is described
   [below](#dos-date-format).
4. The full size of the file in bytes.
5. An offset from the beginning of the file in bytes at which the first entry of the [catalog](#catalog) is located.
   Because of a likely bug in the original `vdfs32g.dll` shipped with the game, this value must always be `296`.
6. The version of the VDF format being used. This is always `0x50` in reality.

### Catalog

The catalog describes the directory and file structure contained within the VDF file. Each entry is either a file or a
directory entry. File entries always point to an address in the data section of the container while directory entries
always point to a subsection of the catalog listing the child entries of the directory. Here's a basic example:

```
00 _WORK/  -> 01
01   DATA/    -> 03
02   CUSTOM/  -> 10
03     ANIMS/     -> 05
04     TEXTURES/  -> 07
05       ANIM1.MAN
06       ANIM2.MAN
07       TEXTURE_A.TEX
08       TEXTURE_B.TEX
09       TEXTURE_C.TEX
10     MYFILE.WAV
```

As you can see, the full path of `ANIM1.MAN` would be `_WORK/DATA/ANIMS/ANIM1.MAN`. Notice that all entries of a
directory are always listed right after each other, and their respective child entries are listed after the parent
folder's entries.

Each entry can be represented as a C struct like this.

```c title="VDF Catalog"
struct zFILE_VDFS_Catalog {
    struct {
        char name[64]; // (1)
        uint32_t offset; // (2)
        uint32_t size; // (3)
        uint32_t type; // (4)
        uint32_t attributes; // (5)
    } entries[/* zFILE_VDFS_Header.numEntries */];
};
```

1. The name might not fill the entire 64 bytes. If it does not, the rest of the array is filled with `'\x20'`
   characters.
2. If the entry is a file, this contains an offset into the file at which the entry's data is stored.
   If it is a directory, this contains the index of the first child entry in the catalog.
3. The size of the entry if it is a file, otherwise contains 0.
4. A bitmask describing the type of the entry. If bit `0x80000000` is set, the entry is a directory, otherwise it is
   a file. If bit `0x40000000` is set, the entry is the last in the parent directory[^2].
5. Extra attributes for the entry. Unused.

## DOS date format

The timestamp format used for VDF files has been called *DOS date format*[^1], however I can not find any official
reference for it online. In any case, these timestamps are saved in a 32-bit unsigned integer and formatted
like this.

```c title="DOS date format"
//                            year (1)       day         minute
//                            ---------      -----       ------
uint32_t date = 0x2D65BBB3 == 0b0010110'1011'00101'10111'011101'10011
//                                      ----       -----        -----
//                                     month (2)    hour       second (3)
```

1. The number of years after 1980, here `1980 + 0b0010110 = 2002`
2. The month starts counting a `1` (January), here `11 = November`
3. Seconds are only stored with half-precision so to get the correct number, the number has to be multiplied by two,
   here `0b10011 * 2 = 38`

This reference implementation can be used to load and create DOS timestamps. It is heavily based on the implementation
in [PhysicsFS](https://github.com/icculus/physfs/blob/6925c1067de2c9e39d626bcba84db0113f8395f2/src/physfs_archiver_vdf.c#L37-L50).

```c title="DOS date format reference implementation"
/**
 * Converts a DOS timestamp to a standard unix timestamp.
 */
time_t dos_to_unix_time(uint32_t dos) {
	struct tm t {};
	t.tm_year = ((int) ((dos >> 25) & 0x7F)) + 80;
	t.tm_mon =  ((int) ((dos >> 21) & 0x0F)) - 1;
	t.tm_mday =  (int) ((dos >> 16) & 0x1F);
	t.tm_hour =  (int) ((dos >> 11) & 0x1F);
	t.tm_min =   (int) ((dos >> 5 ) & 0x3F);
	t.tm_sec =  ((int) ((dos >> 0 ) & 0x1F)) * 2;
	return timegm(&t);
}

/**
 * Converts a unix timestamp to a DOS timestamp.
 */
uint32_t unix_time_to_dos(time_t nix) {
	struct tm* t {gmtime(&nix)};
	uint32_t dos {0};
	dos |= (t->tm_year - 80) << 25;
	dos |= (t->tm_mon + 1) << 21;
	dos |= t->tm_mday << 16;
	dos |= t->tm_hour << 11;
	dos |= t->tm_min << 5;
	dos |= (t->tm_sec / 2) << 0;
	return dos;
}
```

[^1]: References to the DOS date format can be found in [PhysicsFS](https://www.icculus.org/physfs/) where the VDF
format is [implemented](https://github.com/icculus/physfs/blob/6925c1067de2c9e39d626bcba84db0113f8395f2/src/physfs_archiver_vdf.c).
It is also discussed in the [Wiki](https://github.com/REGoth-project/REGoth/wiki/VDF-File-Format) for
[ReGoth](https://github.com/REGoth-project/REGoth) (the old repository).

[^2]: Referring back to the example, the `0x40000000` bit will be set for entries `_WORK/` (`00`), `CUSTOM/` (`02`),
`TEXTURES/` (`04`), `ANIM2.MAN` (`06`), `TEXTURE_C.TEX` (`09`) and `MYFILE.WAV` (`10`).

[^3]: It should be noted that the original implementation of VDF files was provided by a separate DLL called
`Vdfs32g.dll`. `zFILE_VDFS` is merely an implementation using that API.
