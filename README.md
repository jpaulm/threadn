threadn
===

### C-based FBP implementation using longjmp and setjmp

This is a *green* thread FBP implementation - for a true multithreaded implementation, see https://github.com/jpaulm/cppfbp , which uses Boost (http://www.boost.org/ ).  

Notes:
---
Borland C compiler made successive component lines, connection lines, etc. in fixed form network definitions adjacent, so code to iterate through them reflects this. For examples, see `src\test\networks\fixedform`.  

This turns out not to be true for VC Express, so when I moved to that compiler, I decided to use chaining, so as to be more compiler-independent, but I did not change THREADN.  I believe this was the only compiler difference I ran into during the upgrade.  The version using `fibers` follows the latter convention (see below).  

This FBP implementation uses `longjmp` and `setjmp`, and creates new stacks "by hand" using `malloc`s and some register manipulation to switch to the newly created stack (at process start time).  The register manipulation is somewhat machine- and OS-dependent, so I built another implementation using Windows `fibers` to manage the stacks, but still using `longjmp` and `setjmp` to switch *between* the fibers.

If anyone is interested in the latter version, it can be found at http://www.jpaulmorrison.com/fbp/THREADS_32.zip , plus I could put it up on GitHub as well.

As far as I know, only CppFBP of the three implementations mentioned can support multiple cores.  

`dfs` services vs. `thz` services
----

This implementation was designed so that the implementation could be changed without having to recompile or relink components or networks.  The `thz` services are on the API 'side'. while the `dfs` services are on the component 'side' - they communicate by means of the 'anchor' - `thxanch.h`.

It should also be pointed out that the fixed form network definition does not use the same block layouts as those of the internal control blocks. This is also to (hopefully) preserve independence of the component control blocks from those used in the implementation. 
