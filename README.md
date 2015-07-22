# threadn
Old C-based FBP implementation using longjmp and setjmp
===

Notes:
---
Old Borland C compiler made successive component lines, connection lines, etc. adjacent, so code to iterate through them reflects this.  See `src\test\networks\fixedform`.

When I moved to another compiler this ordering changed, so I chained them together - this is what CppFBP does - see .
