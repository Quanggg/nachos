#include "syscall.h"

int main()
{
    char name[51];
    char content[201];
    OpenFileID id;

    PrintString("Please input name of file you want to read: ");
    ReadString(name, 50);

    id = Open(name);

    if (id == -1)
    {
        PrintString("No such file exists!\n");
        Halt();
    }

    Read(content, 200, id);
    PrintString(content);
    PrintString("\n");

    Close(id);

    Halt();
}