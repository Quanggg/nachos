#include "syscall.h"

int main()
{
    char name[21];
    int result;
    PrintString("Please input file name (<= 20 characters): ");
    ReadString(name, 20);

    result = Create(name);
    if (result == 0)
    {
        PrintString("Completed!\n");
    }
    else
    {
        PrintString("Fail!\n");
    }

    Halt();
}