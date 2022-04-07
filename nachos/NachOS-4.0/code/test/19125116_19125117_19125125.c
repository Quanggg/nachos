/* 19125116 Le Chau Nhat Quang
 * 19125117 Nguyen Duy Anh Quoc
 * 19125125 Ho Ngoc Ton
 */

#include "syscall.h"

int main()
{
  char *s;
  // PrintNum(ReadNum());
  // PrintChar(ReadChar());
  // PrintChar(300);
  // PrintNum(RandomNum());
  // char *s;
  // int len = 10;
  // ReadString(s, len);
  // PrintString("");
  OpenFileID file = Open("321.x");
  // Close(_ConsoleOutput);
  Read(s, 100, file);
  // Write("Quang Le", 100, _ConsoleOutput);
  PrintString(s);
  Close(file);
  Halt();
}