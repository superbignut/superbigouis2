![Static Badge](https://img.shields.io/badge/license-N%2FA-red)
![Static Badge](https://img.shields.io/badge/Windows-True-purple)
---
Reference:

+ [onix](https://github.com/StevenBaby/onix)
---

Tools:
+ [nasm](https://www.nasm.us/)
+ [bochs](https://github.com/bochs-emu/Bochs/)
+ [dd for windows](http://www.chrysocome.net/dd)
+ [cmder](https://cmder.app/)
+ [MinGW](https://en.wikipedia.org/wiki/MinGW)
---

Tricks:

 I didn't figure out how to cross compile elf_i386 on Windows, so I resorted to some workarounds:

+  Use "make" in "Cmder.exe" to ensure Makefile works properly.
+  Utilize "bochsdbg.exe" to enable debug mode of Bochs.
+  Replace "dd" with "dd for windows".
+  Substitute "ld -m elf_i386" with "ld -m i386pe".
+  Substitute "nasm -f elf32" with "nasm -f win32".