[windows下的makefile编写]()

这里建议使用 cmder作为终端，可能有些慢，但是对linux 的命令是完全支持的， 比如 rm -r build/

[makefile 特殊字符]()
    
make的一些特殊的字符在[GNU make 10.5](https://www.gnu.org/software/make/manual/make.html#Automatic-Variables)中有细致的说明：

[$<]()
> The name of the first prerequisite. If the target got its recipe from an implicit rule, this will be the first prerequisite added by the implicit rule

匹配第一个需求的参数，也就是冒号后面的第一个

[$@]()
> The file name of the target of the rule. If the target is an archive member, then ‘$@’ is the name of the archive file.

匹配规则的target，也就是：前面的目标文件

[%.*]()
> A target pattern is composed of a ‘%’ between a prefix and a suffix, either or both of which may be empty. The pattern matches a file name only if the file name starts with the prefix and ends with the suffix, without overlap.

一个匹配模式，比如 %.c


[wildcard](https://www.gnu.org/software/make/manual/html_node/Wildcard-Function.html)

> This string, used anywhere in a makefile, is replaced by a space-separated list of names of existing files that match one of the given file name patterns. If no existing file name matches a pattern, then that pattern is omitted from the output of the wildcard function. Note that this is different from how unmatched wildcards behave in rules, where they are used verbatim rather than ignored (see Pitfalls of Using Wildcards).

        ifeq ("$(wildcard build/master.img)", "")
        # 创建硬盘镜像	
            bximage -func=create -hd=16M -imgmode=flat -sectsize=512 $@ -q
        endif
匹配一个模式，返回空格间隔的字符串， 这个例子中是做了一个文件有无的判定
