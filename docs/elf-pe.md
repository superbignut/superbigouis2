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
