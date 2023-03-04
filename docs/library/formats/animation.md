

Animations (also called _Model Animations_) form part of the animations system of the _ZenGin_ they contain only
animation samples, i.e. the position and orientation of each bone of the skeleton they're applied to. While there is
space for additional data within animation files, it is mostly empty.

## Overview

*phoenix'* implementation of animations lives in `include/phoenix/animation.hh` and `source/animation.cc`. The most
important part of an animation are its samples. They are stored in `animation::samples` after it has been parsed.
Animation files themselves don't contain information about when to run animation or any other effects which should be
applied during it. Those parts of the animation system are defined in [Model Script](formats/model-script.md) files
which should be loaded before animations. The `animation::events` field will always be empty for that reason.


### Loading a Font

Like most data structures in *phoenix*, animations can be loaded using the `#!cpp phoenix::animation::parse()` function.
It takes a `phoenix::buffer` as a parameter and loads the animation from it.

```cpp title="Example"
#include <phoenix/animation.hh>

int main(int, const char** argv) {
    auto anim_buffer = phoenix::buffer::mmap("A.man");
    [[maybe_unused]] auto anim = phoenix::animation::parse(anim_buffer);
    return 0;
}
```
