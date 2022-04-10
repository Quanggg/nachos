/* 19125116 Le Chau Nhat Quang
 * 19125117 Nguyen Duy Anh Quoc
 * 19125125 Ho Ngoc Ton
 */

#include "syscall.h"

int main(int argc, char **argv)
{
  char *s;
  Create("test.txt");
  Write("Quang Le Chau Nhat\n", 100, Open("test.txt"));
  Read(s, 100, Open("test.txt"));
  PrintString(s);
  Halt();
}