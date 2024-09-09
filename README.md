# sortkey_for_tex_system
Sanskrit Sort key generator for TeX Index System. ver 1.0(3/5/2004)
Sanskrit Sort key generator for TeX Index System. for utf-8 ver 2.1(3/30/2021)


This program is designed to generate sort key of Sanskrit Dictionary order in TeX system.
This program presupposes TeX index system. If you have generated XX.idx file by TeX \makeindex command,
you can add  the sort codes to XX.idx file through this program.
For example, if the following line, 
 
\indexentry {pramāṇa\(means of knowledge\)}{8}
\indexentry {pram\={a}\d {n}a\(means of knowledge)}{8}


is written in original .idx file (hyphenation mark "\-" and meaningless space in index entry will 
be neglected), my program will re-write as follows:


\indexentry {vrwsbvvctvb@pramāṇa\(means of knowledge\)}{8}
\indexentry {vrwsbvvctvb@pram\={a}\d{n}a\(means of knowledge\)}{8}

"vr" is a code of "p" and "tv" is that of "ṇ". If you use Makeindex or other index program, you can get
XX.ind file, where the each entry has sort key in Sanskrit dictionary order. 
In the above example, the sort code for the field bracked with "\(" and "\)" is not generated. You can make setting such a field. If you need such a field in your index entry, please read the comment in sktcode.tbl file.  

Besides sutra or karika numbers are transformed to be placed in ascending order as follows:
From \indexentry {Ny\={a}yas\={u}tra!1. 1. 12}{8} and \indexentry {Ny\={a}yas\={u}tra!1. 1. 2}{9}
To   
\indexentry {uvwrcwrbxtgurwsb@Ny\={a}yas\={u}tra!001001012@1. 1. 12}{8} and
\indexentry {uvwrcwrbxtgurwsb@Ny\={a}yas\={u}tra!001001002@1. 1. 2}{9}

In this conversion the original idx file is
remaining as XX.idx.org

The version with "sktcode.tbl" is written 3/21/2005

This program is written by Yasuhiro Okazaki Ph. D.
(Hiroshima Prefectural Takamiya Senior High School)

E-mail kappa-y@nn.iij4u.or.jp
 
This program is re-written for utf-8 2021/03/27

1) Files in this package

usktsorkey.c: C-source file of Sanskrit Sort key generator
sktcode.tbl: Alphabet table for Sanskrit romanization. You can modify this table in accordance to the explanation in this file.
Readme.rtf: This file
sktsk: Binary file executable for MacOSX
sktsk.exe: Binary file executable for Windows System

2) Installation

a) Compilation: "usktsortkey.c" does not need any special include file. If you have standard C-compiler, for example gcc, you can easilly compile it. For example, 

$ gcc -o sktsk usktsortkey.c

b) Installation: Copy the executable file "usktsortkey" to "bin,"  For example

$ sudo cp sktsk /usr/local/bin/

Place "sktcode.tbl" on the same directory as that of target "xx.idx"-file. 

3) usage

If you type as follows;

$ sktsk xx.idx

you acquire the new xx.idx whose each index entry has Sanskrit sort code. Other usage can be seen through typing as follows:

$ sktsk -h

