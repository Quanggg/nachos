/* 19125116 Le Chau Nhat Quang
 * 19125117 Nguyen Duy Anh Quoc
 * 19125125 Ho Ngoc Ton
 */

#include "syscall.h"

int main()
{
  PrintString("Our team members: \n");
  PrintString("   19125116 Le Chau Nhat Quang\n");
  PrintString("   19125117 Nguyen Duy Anh Quoc\n");
  PrintString("   19125125 Ho Ngoc Ton\n");

  PrintString("\n");

  PrintString("--ascii--\n");
  PrintString("Print the ASCII TABLE\n");
  PrintString("Run with command: ../build.linux/nachos -x ascii \n");

  PrintString("\n");

  PrintString("--sort--\n");
  PrintString("Print sorted array of number in ascending or descending order\n");
  PrintString("Run with command: ../build.linux/nachos -x bubbleSort \n");
  Halt();
}
