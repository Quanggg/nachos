// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "ksyscall.h"
#include "syscall.h"

void increasePC()
{
	/* Modify return point */
	/* set previous programm counter (debugging only)*/
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

	/* set next programm counter for brach execution */
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

char *userToKernel(int stringAddr)
{
	const int LENGTH_LIMIT = 100;
	int length = 0;
	// get string length
	int tmpChar;
	while (true)
	{
		kernel->machine->ReadMem(stringAddr + length, 1, &tmpChar);
		length++;
		if (tmpChar == '\0')
			break;
	}

	char *str = new char[length];
	// copy string from userspace to kernelspace
	for (int i = 0; i < length; i++)
	{
		kernel->machine->ReadMem(stringAddr + i, 1, &tmpChar);
		str[i] = tmpChar;
	}
	return str;
}

void kernelToUser(char *str, int stringAddr)
{
	int length = strlen(str);
	// copy string from kernalspace to userspace
	for (int i = 0; i < length; i++)
		kernel->machine->WriteMem(stringAddr + i, 1, str[i]);

	kernel->machine->WriteMem(stringAddr + length, 1, '\0');
}
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------
void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2); // SYSTEM CALL type

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");
	switch (which)
	{
	case NoException:
		return;
		ASSERTNOTREACHED();
		break;

	case SyscallException:
		switch (type)
		{
		case SC_Halt:
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
			break;

		case SC_Add:
		{
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
											/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			increasePC();

			return;

			ASSERTNOTREACHED();

			break;
		}
		// ****** PROJECT 1 -- NEW SYSTEM CALL CASE ******
		case SC_ReadNum:
		{
			DEBUG(dbgSys, "ReadNum is running... \n");
			increasePC();
			char inputChar = kernel->synchConsoleIn->GetChar();
			int inputNum = 0;
			bool isNegative;

			// check if input is a negative number
			if (inputChar == '-')
			{
				isNegative = true;
				inputChar = kernel->synchConsoleIn->GetChar();
			}
			else
				isNegative = false;
			// read input from console
			while (inputChar != ' ' && inputChar != '\n')
			{
				// handle if input exceed interger limits
				if (inputNum / 100000000)
					break;
				if (inputChar >= '0' && inputChar <= '9')
					inputNum = inputNum * 10 + inputChar - 48;
				else
				{
					// input is not a number
					kernel->machine->WriteRegister(2, 0);
					return;
				}
				inputChar = kernel->synchConsoleIn->GetChar();
			}
			inputNum = isNegative ? -inputNum : inputNum;
			// write result to register 2
			kernel->machine->WriteRegister(2, (int)inputNum);
			DEBUG(dbgSys, "ReadNum returns: " << inputNum << '\n');

			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_PrintNum:
		{
			DEBUG(dbgSys, "PrintNum is running... \n");
			// get the number need to be printed
			int printNum = kernel->machine->ReadRegister(4);
			int *numberContainer = new int[9];
			int count = 0;
			// handle negative printNum
			if (printNum < 0)
			{
				kernel->synchConsoleOut->PutChar('-');
				printNum *= -1;
			}
			// check if printNum is 0
			if (!printNum)
				numberContainer[count++] = 0;
			// save printNum into an array in reverse order
			while (printNum)
			{
				numberContainer[count++] = printNum % 10;
				printNum /= 10;
			}
			// print number to console
			while (count)
				kernel->synchConsoleOut->PutChar(numberContainer[--count] + 48);

			delete[] numberContainer;
			increasePC();

			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_ReadChar:
		{
			DEBUG(dbgSys, "ReadChar is running... \n");
			// read char from console
			char inputChar = kernel->synchConsoleIn->GetChar();
			// return char
			kernel->machine->WriteRegister(2, (int)inputChar);

			DEBUG(dbgSys, "ReadNum returns: " << inputChar << '\n');

			increasePC();

			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_PrintChar:
		{
			DEBUG(dbgSys, "PrintChar is running... \n");
			// the char need to be printed
			char ch = (char)kernel->machine->ReadRegister(4);
			// print char to the console
			kernel->synchConsoleOut->PutChar(ch);

			increasePC();

			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_RandomNum:
		{
			DEBUG(dbgSys, "RandomNum is running... \n");
			// initialize random number generator
			srand(time(NULL));
			int result = rand();
			// return the random number
			kernel->machine->WriteRegister(2, (int)result);

			DEBUG(dbgSys, "ReadNum returns: " << result << '\n');

			increasePC();

			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_ReadString:
		{
			const int LENGTH_LIMIT = 100;
			DEBUG(dbgSys, "ReadString is running... \n");
			// address of the string
			int str = kernel->machine->ReadRegister(4);
			// length of the string
			int len = kernel->machine->ReadRegister(5);

			// check if the length is valid
			if (len > LENGTH_LIMIT)
			{
				DEBUG(dbgSys, "String length is not valid. Exceeds " << LENGTH_LIMIT);
				SysHalt();
			}

			char *buffer = new char[len + 1];
			// read string from console and save it to buffer
			for (int i = 0; i < len; i++)
				buffer[i] = kernel->synchConsoleIn->GetChar();
			buffer[len] = '\0';
			// bring string from kernalspace to userspace
			kernelToUser(buffer, str);

			delete[] buffer;
			increasePC();

			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_PrintString:
		{
			DEBUG(dbgSys, "PrintString is running... \n");
			// address of string
			int str = kernel->machine->ReadRegister(4);
			// move string from userspace to kernelspace
			char *buffer = userToKernel(str);
			// print the string to the console
			for (int i = 0; i < strlen(buffer); i++)
				kernel->synchConsoleOut->PutChar(buffer[i]);

			delete[] buffer;
			increasePC();

			return;
			ASSERTNOTREACHED();
			break;
		}

		// **********************************
		// ********** PROJECT 2 *************
		case SC_Create:
		{
			// address of string
			int str = kernel->machine->ReadRegister(4);
			// bring string from userspace to kernelspace
			char *fileName = userToKernel(str);

			DEBUG(dbgSys, "Creating file: " << fileName);
			// create file
			bool status = strlen(fileName) > 0 && fileName != NULL ? kernel->fileSystem->Create(fileName) : false;
			DEBUG(dbgSys, "Create " << (status ? "successfully" : "failed"));

			// return status
			kernel->machine->WriteRegister(2, status ? 0 : -1);

			delete fileName;
			increasePC();

			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Open:
		{
			// address of string
			int str = kernel->machine->ReadRegister(4);
			// bring string from userspace to kernelspace
			char *fileName = userToKernel(str);

			DEBUG(dbgSys, "Opening file: " << fileName);
			// open file
			OpenFile *fileID = kernel->fileSystem->Open(fileName);
			DEBUG(dbgSys, "Open " << (fileID != NULL ? "successfully" : "failed"));

			// return OpenFileID
			kernel->machine->WriteRegister(2, fileID != NULL ? (OpenFileID)fileID : -1);

			delete fileName;
			increasePC();

			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Close:
		{
			// get openFileId
			OpenFileID openFileId = kernel->machine->ReadRegister(4);

			int status;
			// = kernel->fileSystem->Close(openFileId);
			DEBUG(dbgSys, "Closing file at: " << openFileId);
			try
			{
				// call destructor to close file
				DEBUG(dbgSys, "Close successfully");
				OpenFile *fileID = (OpenFile *)openFileId;
				fileID->~OpenFile();
				status = 1;
			}
			catch (const std::exception &e)
			{
				DEBUG(dbgSys, "Close failed");
				status = 0;
			}

			// return OpenFileID
			kernel->machine->WriteRegister(2, status);

			increasePC();

			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Read:
		{
			// get buffer address, size and OpenFileID
			int buffer = kernel->machine->ReadRegister(4);
			int size = kernel->machine->ReadRegister(5);
			int openFileID = kernel->machine->ReadRegister(6);

			// create char* for kernel space to store file content
			char *str = new char[size + 1];
			int numRead;

			if (openFileID == _ConsoleInput)
			{
				DEBUG(dbgSys, "Reading at ConsoleInput");
				// read from console
				const int LENGTH_LIMIT = 100;
				// check if the length is valid
				if (size > LENGTH_LIMIT)
				{
					DEBUG(dbgSys, "String length is not valid. Exceeds " << LENGTH_LIMIT);
					SysHalt();
				}

				// read string from console and save it to str
				int i;
				for (i = 0; i < size; i++)
				{
					str[i] = kernel->synchConsoleIn->GetChar();
					if (str[i] == '\n')
						break;
				}
				str[i + 1] = '\0';
				numRead = i;
				// bring string from kernalspace to userspace
			}
			else
			{
				OpenFile *fileID = (OpenFile *)openFileID;
				// read from file
				DEBUG(dbgSys, "Reading file at: " << fileID);
				numRead = fileID->Read(str, size);
			}
			DEBUG(dbgSys, "Total bytes read: " << numRead);

			// move read string to user space
			kernelToUser(str, buffer);

			// return total bytes read
			kernel->machine->WriteRegister(2, numRead);

			delete[] str;
			increasePC();

			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Write:
		{
			// get buffer address, size and OpenFileID
			int buffer = kernel->machine->ReadRegister(4);
			int size = kernel->machine->ReadRegister(5);
			int openFileId = kernel->machine->ReadRegister(6);

			// bring buffer from user space to kernel space
			char *str = userToKernel(buffer);
			int numWritten;
			if (openFileId == _ConsoleOutput)
			{
				DEBUG(dbgSys, "Writing at ConsoleOutput");
				// write to console
				int i;
				for (i = 0; i < strlen(str); i++)
					kernel->synchConsoleOut->PutChar(str[i]);
				numWritten = i;
			}
			else
			{
				OpenFile *fileID = (OpenFile *)openFileId;
				// write to file
				DEBUG(dbgSys, "Writing file at: " << fileID);
				numWritten = fileID->Write(str, strlen(str) >= size ? size : strlen(str));
			}
			DEBUG(dbgSys, "Total bytes written: " << numWritten);

			// return total bytes written
			kernel->machine->WriteRegister(2, numWritten);

			delete[] str;
			increasePC();

			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Seek:
		{
			// get file
			OpenFile *fileID = (OpenFile *)kernel->machine->ReadRegister(4);

			DEBUG(dbgSys, "Closing file at: " << fileID);

			DEBUG(dbgSys, "Close " << (fileID != NULL ? "successfully" : "failed"));
			// delete to close file
			if (fileID != NULL)
				delete fileID;

			int status = fileID != NULL ? 1 : -1;
			// return OpenFileID
			kernel->machine->WriteRegister(2, status);

			increasePC();

			return;
			ASSERTNOTREACHED();
			break;
		}
		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
	case PageFaultException:
		cerr << "Exception Type: PageFaultException\n\n";
		SysHalt();
		return;

		ASSERTNOTREACHED();
		break;

	case ReadOnlyException:
		cerr << "Exception Type: ReadOnlyException\n\n";
		SysHalt();
		return;

		ASSERTNOTREACHED();
		break;

	case BusErrorException:
		cerr << "Exception Type: BusErrorException\n\n";
		SysHalt();
		return;

		ASSERTNOTREACHED();
		break;

	case AddressErrorException:
		cerr << "Exception Type: AddressErrorException\n\n";
		SysHalt();
		return;

		ASSERTNOTREACHED();
		break;

	case OverflowException:
		cerr << "Exception Type: OverflowException\n\n";
		SysHalt();
		return;

		ASSERTNOTREACHED();
		break;

	case IllegalInstrException:
		cerr << "Exception Type: IllegalInstrException\n\n";
		SysHalt();
		return;

		ASSERTNOTREACHED();
		break;

	case NumExceptionTypes:
		cerr << "Exception Type: NumExceptionTypes\n\n";
		SysHalt();
		return;

		ASSERTNOTREACHED();
		break;

	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}
