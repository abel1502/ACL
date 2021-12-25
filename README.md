# Abel Common Library

![C++17 compatible](https://img.shields.io/badge/C%2B%2B-17-brightgreen)

The standard library for all of my projects. Might as well be the one for yours', if you so choose.

When using my other projects, reliant on this library, you will most often need to define
the following environment variables:
 - `ACL_LIB_PATH` should point to `./lib/x64/Release/` (but as the global path on your device).
   If you're building it in another mode or architecture, change the corresponding folder name.
 - `ACL_INC_PATH` should point to `./include/` (but global, again)

Alternatively, you could copy `./lib/x64/Release/ACL.lib` to the global static library cache, and
the contents of `./include/` to your global include path.
