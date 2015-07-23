# threadn
Old C-based FBP implementation using longjmp and setjmp
===

This is a *green* thread FBP implementation - for a true multithreaded implementation, see https://github.com/jpaulm/cppfbp , which uses Boost (http://www.boost.org/ ).  

Notes:
---
Borland C compiler made successive component lines, connection lines, etc. in fixed form network definitions adjacent, so code to iterate through them reflects this. See `src\test\networks\fixedform`.

When I moved to another compiler (VC Express) this ordering changed (reversed?), so I decided to use chaining - this is what CppFBP does - see https://github.com/jpaulm/cppfbp/blob/master/TestNetworks/CopyFile/CopyFile.cpp .  I believe this was the only compiler difference I ran into during the upgrade.

This FBP implementation uses longjmp and setjmp, and creates new stacks "by hand" using `malloc`s and some register manipulation to switch to the newly created stack (at process start time).  The register manipulation is somewhat machine- and OS-dependent, so I built another implementation using Windows fibers to manage the stacks, but still using longjmp and setjmp to switch *between* the fibres. 

As far as I know, only CppFBP of the three implementations mentioned can support multiple cores.  However, https://msdn.microsoft.com/en-us/library/windows/desktop/ms681917%28v=vs.85%29.aspx seems to suggest that processes and/or threads can take advantage of multiple cores - curious if this is true...?
