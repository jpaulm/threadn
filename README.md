# threadn
Old C-based FBP implementation using longjmp and setjmp
===

This is a *green* thread FBP implementation - for a true multithreaded implementation, see https://github.com/jpaulm/cppfbp .

Notes:
---
Old Borland C compiler made successive component lines, connection lines, etc. adjacent, so code to iterate through them reflects this.  See `src\test\networks\fixedform`.

When I moved to another compiler this ordering changed, so I chained them together - this is what CppFBP does - see https://github.com/jpaulm/cppfbp/blob/master/TestNetworks/CopyFile/CopyFile.cpp .
