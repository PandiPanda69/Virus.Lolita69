Antivirus.BeauGosse69
==============

Purpose
---------------

This antivirus has been developped by french students of INSA Lyon engineering school. It aims to detect viruses developped by other students, using a static check (signatures, control flow graphs, ...) and dynamic scan (execution in a sandbox).

It has been coded in C++ by:
- [@PandiPanda69](https://github.com/PandiPanda69/)
- [@rfradet](https://github.com/rfradet/)
- [@clementnotin](https://github.com/clementnotin/)
- [@vbailly](https://github.com/vbailly/)

Architecture
---------------

### Introduction

The architecture is not really complicated to understand and has been designed following these considerations:
+ A virus has a fixed behaviour
+ A virus has a fixed language
+ New instructions (asm-like or higher level) might be added
+ Strings might be encrypted or might change over time
+ A virus can be contained by an archive
+ A virus can be an archive, such a JAR file

With these few informations, we understand how it could be hard to analyse statically a file and determine wether it is a virus or not, having a mistakes rate close to 0.
The easiest way to proceed for having good results is to use `regular expressions`. This way, we can specified signatures that can be flexible enough to match variants.

Dynamic scan should allow to be more precise if a static check failed.

### Core

#### Static analysis
The antivirus engine tries to match `regular expressions` with the analysed-file content. Several signatures can be contained by the database, and for each one, the core check if one of them match the file content.

So the antivirus handles binary file, but it's wrong to think signatures are binary regular expressions since it makes no sense. Signatures are written in hex. (look at the section below for having much more information) It implies to read file and then, to transform it in memory in order to allow matching.

Since virus can be contained by archive, we also implemented an inflating system which is able to walk through the contained files. Each contained file are then read and the core tries to match signatures on it.

#### Dynamic analysis

If the static scan does not report any suspicious content, a dynamic scan is performed in order to check what is done while running.

`chroot` is used to set up a sandbox to run safely the file to scan. Every dependency are established using `ldd` and placed in the sandbox, so the program can run without constraints. System calls are then traced for 3 main reasons:

+ Allowing the program to smoothly in the sandbox. Each time the program wanna access a file, an `open` call is made. So, we check the file he wants to reach and we place it into the sandbox (except files among `/dev`... sandbox only touch a new file to simulate the device...).
+ Allowing to trace the program execution by monitoring every call he does. So, we are able to determine wether its actions are suspicious, malicious, or legitimates.
+ Allowing to trace subprocess since `fork`, `clone`, ... could be invoked by program. Each time it's done, we get new pid and attach the tracer to this new pid until he exits.

For tracing, we use `ptrace` and we trace system calls before the execution, and after so we can avoid a malicious action to be runned. Anyway, the sandbox is here to avoid to compromise host system.

Signatures have been implemented the dirty way and could be rewritten to be more parametrizable. Nowadays, they are hard coded. They monitor every `write` calls. Since all viruses write in files (stdout included), it's the easiest way. We reimplemented a kind of descriptor table to know which file descriptor refers to which file on filesystem. Then, signatures are just a pattern that is matched to see, in a given file, is this pattern written. If yes, the action is malicious and this file is a virus.

A system has also been written to be able to detect multiple writes. So, if a program do such sequence:
```
write(1, "H");
write(1, "E");
write(1, "L");
write(1, "L");
write(1, "O");
```
If the pattern to match on `stdout` is **HELLO**, then it will match.

Time let us give up a more flexible system tracing `_llseek` calls to be more efficient.

### Outputs

Abiding with teacher rules, we add to print on `stdout`
+ **Virus** if the file seems to be infected by a signed virus;
+ **Legitimate** if the file seems clean.

### Debug Trace

A little macro is available to trace the execution. It can be enabled into `debug.h` by a simple definition:

```cpp
#define DEBUG
```

It allows to output such lines:
```
[TRACE] the_file.cpp:line     My trace
```

Writting Signatures
---------------

### Format

As seen before, a signature is a `regular expression`. We used `perl` format (arbitrary). If the format is not valid, a dirty error message would inform you did a mistake somewhere.

When editing `signatures database` located in `rsrc/signatures.db`, you'll also have to respect another format:
```
MY_SIGNATURE_NAME=MY_REGEX
```

#### Warning

* **Every regular expressions must be written in lower case.**
* **Lines must end by `LF` only (Unix like). `CRLF` may cause errors in your regex.**

### Examples

Considering a file containing:
```
Hello
```

We want to add a signature based upon the first `H` (0x48). Here the result:

```
Hello_sign=^48(.*)
```

Compilation
---------------

To compile the whole project and the tools, you have to run the `./configure` script.
Take care to install, if not already done, these libraries:
- libzip-dev
- libboost-regex-dev
- libboost-system-dev
- libboost-filesystem-dev
- libmagic-dev

(it has been proven to work with version 1.42 of Boost libraries, you can try with higher versions)

Then you can run `make` and it should build the antivirus into `bin/antivirus_packed`.


We provided a static linked binary to our teacher without any debug information by directly modifying `src/antivirus/Makefile`, removing `-g` option to `CXXFLAGS` and adding `-static` to `LDFLAGS`.

Packaging
---------------

Launching `make` command auto-package the antivirus by running the different provided tools. If you want to do these steps manually, here we go:
- Encrypt the `signatures database` by running
```
$ ./bin/database_packer ./rsrc/signatures.db ./bin/signatures_encrypted.db
```
- Insert the `encrypted database` into the antivirus, so it is auto-contained
```
$ cd bin
$ ./database_inject signatures_encrypted.db antivirus antivirus_packed
```

Usage
---------------

antivirus <file_to_check>
  file_to_check:  File to be checked by the antivirus
  
```
$ antivirus /bin/ls
Legitimate
```

Troobleshooting
---------------

Some bugs are known and may appear while running:
- Since files are loaded in memory, if the file is too large, an error may happen.
- Regular expressions are really ease to use but sometimes, the engine cannot evaluate wether the expression matches or not after a lot of tries. An error may appear.
