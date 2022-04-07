/* 19125116 Le Chau Nhat Quang
 * 19125117 Nguyen Duy Anh Quoc
 * 19125125 Ho Ngoc Ton
 */

#include "syscall.h"

int main()
{
  int i;
  for (i = 32; i <= 126; i++)
  {
    PrintString("[");
    // index
    PrintNum(i);
    PrintString("]: ");
    // character
    PrintChar(i);
    PrintChar('\n');
  }
  Halt();
}
