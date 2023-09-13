# How to contribute to _ZenKit_

If you'd like to add a feature or fix a bug, you're more than welcome to help! You can grab one of the open issues and
start to work on it, too. 

There are some conventions I'd ask you to keep in mind while writing code for _ZenKit_.  I've outlined them below.

## Conventions

There are a few things that should be followed to keep the codebase consistent.

### Git

Your commits should be small. Avoid bundling multiple changes relating to (for example) multiple different issues or bugs
into one commit. Rather, create multiple smaller commits, each self-contained only changing one part of the logic[^1].

[^1]: For example, if there is an issue with the VM and you change something, then add a new API to the `DaedalusScript`, 
those should be two separate commits.

### C++ code

* Class names are `PascalCase`
* Function names are `snake_case` and private member functions are prefixed with an underscore
* Member variables are `snake_case` and private member variables are prefixed with `_m_`

Use `clang-format`. While the CI pipeline will complain about noncompliance, you can avoid unnecessary commits by
checking your code before pushing.

Some miscellaneous conventions apply as well:

* **Never** write `using namespace` in any header file and avoid doing it library source code as well
* There should be no mutable global state (all global variables should be `const` or `static constexpr`)
* Constructors should only construct objects. If your constructor affects any other state not tied to the object it's
  constructing, it should probably be a function instead.
