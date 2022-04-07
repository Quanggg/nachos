/* 19125116 Le Chau Nhat Quang
 * 19125117 Nguyen Duy Anh Quoc
 * 19125125 Ho Ngoc Ton
 */

#include "syscall.h"

int main()
{
  int n, temp, i, j, order, a[100];

  // Get number of elements using ReadNum syscall
  do
  {
    PrintString("Enter number of elements n (1 <= n <= 100): ");
    n = ReadNum();
    if (n < 0 || n > 100)
      PrintString("Invalid input! n must be between 1 and 100. Please try again\n");
  } while (n < 0 || n > 100);

  // Input elements for the array
  PrintString("Enter numbers: \n");
  for (i = 0; i < n; i++)
  {
    PrintString("[");
    PrintNum(i);
    PrintString("]: ");
    a[i] = ReadNum();
  }

  // Select sort order
  do
  {
    PrintString("Enter sort order (1: ascending order, 2: descending order): ");
    order = ReadNum();
    if (order != 1 && order != 2)
      PrintString("Invalid input! Please try again\n");
  } while (order != 1 && order != 2);

  // Sort using bubble sort
  for (i = 0; i < n - 1; i++)
  {
    for (j = 0; j < n - i - 1; j++)
    {
      if (order == 1)
      {
        // ascending order
        if (a[j] > a[j + 1])
        {
          temp = a[j];
          a[j] = a[j + 1];
          a[j + 1] = temp;
        }
      }
      else
      {
        // descending order
        if (a[j] < a[j + 1])
        {
          temp = a[j];
          a[j] = a[j + 1];
          a[j + 1] = temp;
        }
      }
    }
  }

  // Print out sorted array using PrintNum and PrintChar syscall
  PrintString("Sorted array in ");
  (order == 1 ? PrintString("ascending order: ") : PrintString("descending order: "));
  for (i = 0; i < n; i++)
  {
    PrintNum(a[i]);
    PrintChar(' ');
  }
  PrintChar('\n');
  Halt();
}