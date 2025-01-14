[elf-spec](https://refspecs.linuxfoundation.org/elf/elf.pdf)
[elf-fomat](https://linux.die.net/man/5/elf)
[elf-info](https://www.cnblogs.com/gongxianjin/p/16906719.html)

> The header file <elf.h> defines the format of ELF executable binary files. Amongst these files are normal executable files, relocatable object files, core files and shared libraries.

> An executable file using the ELF file format consists of an ELF header, followed by a program header table or a section header table, or both. The ELF header is always at offset zero of the file. The program header table and the section header table's offset in the file are defined in the ELF header. The two tables describe the rest of the particularities of the file.

ELF header 是一个总领位置的header 包含了 后续的program header 和section header 的偏移地址

> This header file describes the above mentioned headers as C structures and also includes structures for dynamic sections, relocation sections and symbol tables.

> The following types are used for N-bit architectures (N=32,64, ElfN stands for Elf32 or Elf64, uintN_t stands for uint32_t or uint64_t):


+ ELF header 
+ program header table ： 运行
+ section header table ： 链接


> An executable or shared object file's program header table is an array of structures, each describing a segment or other information the system needs to prepare the program for execution. An object file segment contains one or more sections. Program headers are meaningful only for executable and shared object files.     

program header table 是运行时的表，由很多segment 组成，每个segment 包含多个section

    ```cpp
    typedef struct {
        unsigned char e_ident[EI_NIDENT];
        uint16_t      e_type;
        uint16_t      e_machine;
        uint32_t      e_version;
        ElfN_Addr     e_entry;
        ElfN_Off      e_phoff;
        ElfN_Off      e_shoff;
        uint32_t      e_flags;
        uint16_t      e_ehsize;
        uint16_t      e_phentsize;
        uint16_t      e_phnum;
        uint16_t      e_shentsize;
        uint16_t      e_shnum;
        uint16_t      e_shstrndx;
    } ElfN_Ehdr;

    typedef struct {
        uint32_t   p_type;
        Elf32_Off  p_offset;
        Elf32_Addr p_vaddr;
        Elf32_Addr p_paddr;
        uint32_t   p_filesz;
        uint32_t   p_memsz;
        uint32_t   p_flags;
        uint32_t   p_align;
    } Elf32_Phdr;

    typedef struct {
        uint32_t   sh_name;
        uint32_t   sh_type;
        uint32_t   sh_flags;
        Elf32_Addr sh_addr;
        Elf32_Off  sh_offset;
        uint32_t   sh_size;
        uint32_t   sh_link;
        uint32_t   sh_info;
        uint32_t   sh_addralign;
        uint32_t   sh_entsize;
    } Elf32_Shdr;

    typedef struct {
        uint32_t      st_name;
        Elf32_Addr    st_value;
        uint32_t      st_size;
        unsigned char st_info;
        unsigned char st_other;
        uint16_t      st_shndx;
    } Elf32_Sym;

    typedef struct {
        Elf32_Addr r_offset;
        uint32_t   r_info;
    } Elf32_Rel;

    typedef struct {
        Elf32_Addr r_offset;
        uint32_t   r_info;
    } Elf32_Rel;
    ```


---

但是最后我交叉编译失败了，使用MINGW 中的 i386pe 代替了 elf_i386 来作为编译和链接部分的中间格式，
并且nasm 也选用了 -f win32 ，输出同样为 i386pe格式

为什么我找不到 在windows中 编译出 elf_i386 的方法呢？？？

---

[PE](https://learn.microsoft.com/en-us/windows/win32/debug/pe-format) 的具体格式似乎可视化程度还高一些， 有机会再了解吧


---

之前一直觉得 在汇编里写 start 函数，然后既可以写jmp跳到 c 函数里，但一直不清楚为什么start就是入口地址，今天看了一下 GND 的 ld 找到了一个说明：

[The Entry Point](https://ftp.gnu.org/pub/old-gnu/Manuals/ld-2.9.1/html_node/ld_24.html#SEC24)

> The linker command language includes a command specifically for defining the first executable instruction in an output file (its entry point). Its argument is a symbol name:

        ENTRY(symbol)

> Like symbol assignments, the ENTRY command may be placed either as an independent command in the command file, or among the section definitions within the SECTIONS command--whatever makes the most sense for your layout.

> ENTRY is only one of several ways of choosing the entry point. You may indicate it in any of the following ways (shown in descending order of priority: methods higher in the list override methods lower down).

+ the `-e' entry command-line option;
+ the ENTRY(symbol) command in a linker control script;
+ the value of the symbol start, if present;  <--------------- //  这一条是满足的，所以 start 函数被ld 连接成了 00地址， 进而使用 -Ttext 参数，start 就被编译到了合适的位置上
+ the address of the first byte of the .text section, if present;
+ The address 0.
  