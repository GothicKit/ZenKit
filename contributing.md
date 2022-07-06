# how to contribute to _phoenix_

If you'd like to add a feature or fix a bug, you're more than welcome to help! You can grab one of the open issues and
start to work on it, too. 

There are some conventions I'd ask you to keep in mind while writing code for _phoenix_.  I've outlined them below.

## conventions

There are a few things that should be followed to keep the codebase consistent.

### git

Your commits should be small. Avoid bundling multiple changes relating (for example) multiple different issues or bug
into one commit. Rather, create multiple smaller commits, each self-contained only changing one part of the logic[^1].

Git commits should follow roughly the following format: `<class_name>: <commit_message>`. If you're working on a tool,
you can use `tools: <tool_name>: <commit_message>`. If your commit affects something about the build system or other
non-C++ related things, use `project: <commit_message>`. Take a look at existing commits to get a better idea as to
what I mean.

[^2]: For example, if there is an issue with the VM and you change something, then add a new API to the `script`, 
those should be two separate commits.

### C++ code

_phoenix_ roughly follows the naming convention of the standard library. This includes:

* Class names are `snake_case`
* Function names are `snake_case` and private member functions are prefixed with an underscore
* Member variables are `snake_case` and private member variables are prefixed with `_m_`

Use `clang-format`. While the CI pipeline will complain about noncompliance, you can avoid unnecessary commits by
checking your code before pushing.

Some miscellaneous conventions apply as well:

* **Never** write `using namespace` in any header file and avoid doing it library source code as well
* There should be no mutable global state (all global variables should be `const` or `static constexpr`)
* Constructors should only construct objects. If your constructor affects any other state not tied to the object it's
  constructing, it should probably be a function instead.
