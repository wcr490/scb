savedcmd_/home/rm/work/scb/bdd/czblk.o := ld -m elf_x86_64 -z noexecstack --no-warn-rwx-segments   -r -o /home/rm/work/scb/bdd/czblk.o @/home/rm/work/scb/bdd/czblk.mod  ; ./tools/objtool/objtool --hacks=jump_label --hacks=noinstr --hacks=skylake --ibt --orc --retpoline --rethunk --sls --static-call --uaccess --prefix=16  --link  --module /home/rm/work/scb/bdd/czblk.o

/home/rm/work/scb/bdd/czblk.o: $(wildcard ./tools/objtool/objtool)
