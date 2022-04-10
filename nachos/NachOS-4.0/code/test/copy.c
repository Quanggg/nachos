#include "syscall.h"

int main()
{
    char name_src[51], name_dest[51];
    char content[201];
    OpenFileID id_src, id_dest;

    PrintString("Please input name of source file: ");
    ReadString(name_src, 50);

    id_src = Open(name_src);
    if (id_src == -1)
    {
        PrintString("No such source file exists!");
        Halt();
    }

    Read(content, 200, id_src);
    Close(id_src);

    PrintString("Please input name of destination file: ");
    ReadString(name_dest, 50);

    id_dest = Open(name_dest);
    if (id_dest == -1)
    {
        Create(name_dest);
        id_dest = Open(name_dest);
    }

    Write(content, 200, id_dest);

    Close(id_dest);

    Halt();
}