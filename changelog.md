# changelog

This file contains all changes made to _phoenix_ in reverse order, meaning the newest change is listed first. This
file is updated whenever a new version of _phoenix_ is released. More information about how versioning works can be
found in [readme.md](readme.md#versioning).

---

## v1.1.0

Oh boy, this is a big one! There are a lot of additions, some changes and also some deprecations here. Also, the
performance of phoenix has improved quite a bit, especially interacting with VDFs and parsing worlds. We've also made
the first steps to making phoenix a shared library by adding support for `cmake --install` (thanks @DaDummy!) which
will be expanded on in the future. Another cool thing is the new, centralized documentation page for _phoenix_ and
_ZenGin_ internals available at https://phoenix.lmichaelis.de. It's quite bare-bones still but some interesting stuff
has already been moved over.

Anyway, here's the list of changes:

### Bugfixes

* [0c29d032, 6a64eee3] Fix issues related to stack guards and externals.
* [529137d5] Fix incorrect parsing of animation sample rotations.
* [1403f5e2] Fix undefined behavior in `trigger_mover`
* [2456b7fe] Report the end of archive objects at EOF as well.

### Features

* [397bacfa, 702c8382, b7a650af, 6dd7fe4a, 57037739, 9568b8ad, db6616f4, 15396f07, 63692ad2, d855c166, 89327216] Add
  support for parsing save-games. This feature allows phoenix to fully parse original save-games from their directory
  structure. See `save_game.hh` for additional information.
* [08bcab36] Add `archive_reader::print_structure` to print the contents of an archived object as XML. This is mainly
  useful for debugging, and it only works properly with ASCII and BIN_SAFE archives since BINARY archives don't contain
  field names.
* [f789a925] Add a safer version of `archive_reader::read_raw_bytes` which takes the number of bytes to read as a
  parameter. This works since we normally know how many bytes to parse anyway, otherwise BINARY archives would not work.
* [0d0f07db] `archive_reader` now has an API to easily check whether it's a save-game or not (`archive_reader::is_save_game`).
* [30793759, b8db4092] Add unstable APIs for retrieving and visiting archive entries and objects without a schema. See
  `archive_reader::next` and `archive_reader::visit_objects` for more details.
* [a8526c34] VM external functions may now take raw instance pointers. This allows for bypassing potentially expensive
  `std::shared_ptr` copies.
* [d0ad2da0] The default external implementation now handles instances by pushing a `null`-instance instead

### Changes

* [ae776007] `script::find_symbol_by_name` and everywhere the script and VM require a symbol name parameter now take
  a `std::string_view` instead of `std::string`.
* [0bfd3cfd] `symbol::get_string` now returns a `std::string_view` instead of a `std::string`
* [0d1b976b] Indexes into script symbol values are now represented by a `size_t` instead of a `uint8_t`
* [fd0023b7] **Internal Change**: phoenix' tests are now much easier to identify and feature additional checks for
  VObs and different game versions
* [c0ace3cb] `bsp_tree` leaf polygons are now stored in a `std::unordered_set` instead of a `std::vector` for
  performance reasons. This change improves world parse times by about 6%.
* [39319e92] If the signature of a VDF files being parsed is not recognized, phoenix now throws a `vdfs_signature_error`

### Deprecations

* [32770f8d] `way_net::waypoint(std::string const&)`: This is a broken and slow API which should not be used.
* [f789a925] `archive_reader::read_raw_bytes()`: This API is unsafe and should no longer be used.
* [1df2b5e0] `model_script::parse_binary(...)`: Use `model_script::parse(...)` instead, it now supports both binary
  and text-based scripts.
* [30de6a4e] `camera_lock_mode` and `vob::camera_alignment`: Old and incorrect names for `sprite_alignment` and
  `vob::sprite_camera_facing_mode` respectively.
* [604b61bd] `vdf_entry* vdf_entry::find_child(std::string_view) &` and `vdf_entry* vdf_file::find_child(std::string_view) &`:
  Mutating VDF entries is broken, thus these APIs should not be used. Use the `const` versions instead.

### Performance

* [ebb48be2] `messages::block_by_name` now makes use of a sorted vector instead of a map lookup
* [604b61bd, 1957cbb8] VDF entry lookups are now up to 90 times faster due to usage of `std::set` to contain the entries.
* [ee00a037] The VM now uses a flat array as a stack instead of an actual `std::stack`. This eliminates unnecessary 
  heap allocations during runtime and thus improves speed.
* [f8c69354] Improve performance of `buffer` in general by about 20%
* [5bf91b45] Improve `archive_reader::read_object_begin` performance by about 7%

### Misc

* [d2b86498, 21dbecd6, bdc8bb39, b962249a] phoenix is now also compiled with `-Wpedantic` and `-Wshadow`
* [2556143e] Add support for using CMake install (thanks, @DaDummy!)
* [de512dad] Clang 14 is now tested in CI and fully supported
* [cc00c0ea] `fmtlib` is no longer a dependency of phoenix
* [16328f8a, 44a59c1e, a61ab85e, 18464552, 8196b897, 3d79c512 - 55de3c63] Add centralized documentation site for both
  the phoenix library and ZenGin internals. It can be found at https://phoenix.lmichaelis.de.

---

## v1.0.2

In this version, some quite severe bugs regarding model script parsing have been fixed. Additionally, this release
contains a lot of smaller fixes detailed below:

### Bugfixes

* [cbe6feaa] Fix issue in `archive_reader::read_bool` which would return `false` for any value other than `1`. This
  is not the correct behaviour since it should return `true` for any non-zero value instead. This is now the case.
* [5c1c34c6] Fix typo in `#define` for log levels (thanks, @DaDummy!)
* [15f0e4f9] Fix a bug in `archive_reader_binary::skip_opject` which caused the parser to jump 4 bytes too far
* [a3a23ce0] Fix a compilation error specific to Apple Clang in `c_info::remove_choice`
* [b3b7eb21, ff6735b7] Fix incorrect parsing of enums in `BINARY` archives.
* [17dfa5af, 3abd7719, 26f53ec9, 5c628a45] Fix many issues regarding MDS parsing. MDS files seem to have a defined
  structure at first but there are typos in original files which break that structure. These fixes address various
  typos in these files to make them parse correctly again.

---

## v1.0.1

This version contains patches for many bugs as well as some performance improvements in the _VDF_ and _ZenGin Archive_
parsers. The following changes have been made since [v1.0.0](https://github.com/lmichaelis/phoenix/releases/tag/v1.0.0):

### Bugfixes

* [d9357f6] Previously, only the non-`const` version of `script::find_symbol_by_name` was case-insensitive. Now
  the `const` version is too.
* [7d87485] Non-free waypoints are now correctly included in the `way_net::waypoint` lookup
* [c8c2294] Return `null`-instances from the default VM external set by `vm::register_default_external` if needed
* [ce320ad] If a default external is set the stack guard in the `opcode::be` branch of `vm::exec` is now inhibited
  preventing stack corruption
* [272c2a3] The stack guard for the `opcode::bl` instruction in `vm::exec` used `sym` as uninitialized which caused VM
  crashes if any script function was overridden
* [612b078] Fix incorrect usage of `std::isspace` in multiple locations which could cause crashes using compilers which
  treat `char` as signed

### Performance

* [88c43eb] Improve `archive_reader::read_object_begin` performance by up to 30% by using `sscanf` instead
  of `std::stringstream` internally

### Misc

* [52e3136, 21d59b8, 6bc9967] Allow using Git submodules in addition to CMake's `FetchContent` for resolving
  dependencies
* [ccf8ce6] Updated old SHA1 hash of the lexy dependency (thanks, @thokkat!)

---

## v1.0.0 – Initial release

After about a year of development, _phoenix_ released version `1.0.0` in October 2022. After replacing
[Try/ZenLib](https://github.com/Try/ZenLib) which was forked from [ataulien/ZenLib](https://github.com/ataulien/ZenLib)
in the _Gothic_ engine re-implementation [OpenGothic](https://github.com/Try/OpenGothic) and a bit more than 500
commits, _phoenix_ has finally reached a point at which I would consider it to be stable enough to make a full release.

Thanks to everyone who helped test the implementation in [OpenGothic#271](https://github.com/Try/OpenGothic/pull/271).
I want to especially thank [@Try](https://github.com/Try) for allowing me to integrate into
[OpenGothic](https://github.com/Try/OpenGothic)! It really helped motivate me to keep working on _phoenix_ and to make
it better!
