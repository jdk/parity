/******************************************************************************/
/* 
 * Project		: Parity Generator Command Line App
 * File			: main.c
 *				:
 * Author		: Jason Kotzin
 * License		: This software, is distributed under the GPL.   
 *				:
 * Created		: 11-16-2009 (MM-DD-YYYY)
 * Tabsize		: 4
 *				: 
 * Description 	: Generates Even and Odd Paritiy in specified string.
 *				: Will Also remove even and odd parity in a given string.
 * 				
 * Notes		: input must be in bytes divisible by 8
 *
 * Copyright (C) 2009 claylogic
 */

/******************************************************************************/

// Include Libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <usb.h>
#include <unistd.h>

// Include Local Libraries
#include <prjutil.h>

// Macro so code is cleaner
#define odd_arg		(strcmp(argv[1], "odd")==0)
#define even_arg	(strcmp(argv[1], "even")==0)
#define remove_arg	(strcmp(argv[1], "remove")==0)

#define VERSION_MAJOR 	0
#define VERSION_MINOR	2

unsigned char DecryptKey[16] ={
    0xf7, 0x21, 0x73, 0x98, 0xbd, 0x96, 0x75, 0x6b,
	0xd2, 0x74, 0x98, 0x02, 0x0d, 0xd9, 0x6b, 0x9d,
};

// Local Functions
char odd_parity(unsigned char value);
char even_parity(unsigned char value);
void add_odd_parity(unsigned char * input, int inputSize, unsigned char * output, int outputSize);
void add_even_parity(unsigned char * input, int inputSize, unsigned char * output, int outputSize);
void remove_parity(unsigned char * input, int inputSize, unsigned char * output, int outputSize);

// Macro to make code look cleaner
int arguments;
static inline void enough_args(int amount_expected)
{   
	if(! ((arguments - 2) >= amount_expected))
	{
		printf("Not Enough Arguments\n");
		exit(0);
	}
}

static void usage(char *name)
{
	fprintf(stderr, "\nParity Version %d.%d\n",VERSION_MAJOR,VERSION_MINOR);
	fprintf(stderr, "\nusage:\n");
    fprintf(stderr, "  %s odd    XX XX XX XX              (Adds odd parity to argument)\n", name);
    fprintf(stderr, "  %s even   XX XX XX XX              (Adds even parity to argument)\n", name);
    fprintf(stderr, "  %s remove XX XX XX XX              (Removes parity from argument)\n\n", name);
    fprintf(stderr, "  %s where  XX XX XX XX              (represents HEX)\n\n", name);
}

int main(int argc, char **argv)
{
	//Make sure there are enough args	
	if(argc <2)
	{
		usage(argv[0]);
		exit(0);
	}

	int i;
	//if we are creating
	// Buffers for input and output
	int inputSize = argc-2;	
	unsigned char * input = (unsigned char*) malloc(inputSize);
	if (input == NULL) {fputs ("Memory error",stderr); exit (2);}	

	int argument_count = 0;
	
	for (argument_count = 0; argument_count < argc-2; argument_count++)
	{
		input[argument_count] = strtol(argv[argument_count+2], NULL, 16);
	}

	// For Local Loops
	printf("\n\tUser Input: ");
	for (i = 0; i < argc-2; i++)
	{
		printf("%02X ", input[i]);
	}
	printf("\n");

	if(remove_arg)
	{
		if( !((argc-2)/8 == ((argc-2)%8)) )
		{
			printf("%X, : %X", (argc-2)/8, (argc-2)%8);
			printf("\nError: You sure you entered enough numbers?\n\n");
			exit(0);
		}

		int outputSize = inputSize - inputSize/9;
		unsigned char * output = (unsigned char*) malloc(outputSize);
		if (output == NULL) {fputs ("Memory error",stderr); exit (2);}	

		//Create
		remove_parity(input, inputSize, output, outputSize);

		// Print Result
		printf("\tResult:     ");
		for (i = 0; i < outputSize; i++)
		{
			printf("%02X ", output[i]);
		}
		printf("\n\n");
#ifdef CONFIG_DEBUG
		printf("Actual: F7 21 73 98 BD 96 75 6B D2 74 98 02 0D D9 6B 9D 6B A5 DE F5 4B 4C 23 EF 0E fC DB 8f 40 FD 10 91\n");
#endif
		free (output);
	}
	else if(odd_arg)
	{
		if( (argc-2)%8 != 0)
		{
			printf("\nError: You have to enter numbers in multiples of 8\n\n");
			exit(0);
		}

		int outputSize = inputSize + inputSize/8;
		unsigned char * output = (unsigned char*) malloc(outputSize);
		if (output == NULL) {fputs ("Memory error",stderr); exit (2);}	

		//Create
		add_odd_parity(input, inputSize, output, outputSize);

		// Print Result
		printf("\tResult:     ");
		for (i = 0; i < outputSize; i++)
		{
			printf("%02X ", output[i]);
		}
		printf("\n\n");
#ifdef CONFIG_DEBUG
		printf("Actual: F7 10 DC D3 0B DC B5 D4 D6 D2 BA 66 00 40 D6 C9 AD 3A 6B 52 F7 BE B4 BA 60 8D DE 0E 7E 76 F1 E4 07 E8 41 22\n");
#endif
		free (output);
	}
	else if(even_arg)
	{
		if( (argc-2)%8 != 0)
		{
			printf("\nError: You have to enter numbers in multiples of 8\n\n");
			exit(0);
		}

		int outputSize = inputSize + inputSize/8;
		unsigned char * output = (unsigned char*) malloc(outputSize);
		if (output == NULL) {fputs ("Memory error",stderr); exit (2);}	

		//Create
		add_even_parity(input, inputSize, output, outputSize);

		// Print Result
		printf("\tResult:     ");
		for (i = 0; i < outputSize; i++)
		{
			printf("%02X ", output[i]);
		}
		printf("\n\n");
		free (output);
	}
	else
	{
		usage(argv[0]);
	}
	free (input);
	
	return 0;
}

char odd_parity(unsigned char value)
{
	int i;
	int numofones = 0;
	for (i = 0; i < 8; i++)
	{
		if (value&1<<i)
			numofones ++;	
	}

	char result = ~numofones&0x01;
	
	return (result);
}	

char even_parity(unsigned char value)
{
	int i;
	int numofones = 0;
	for (i = 0; i < 8; i++)
	{
		if (value&1<<i)
			numofones ++;	
	}

	//Debug
	//printf("There are %d ones\n", numofones);

	char result = numofones&0x01;
	
	//Debug
	//printf("Result %d: \n", result);
	
	return (result);
}	

void add_odd_parity(unsigned char * input, int inputSize, unsigned char * output, int outputSize)
{
	int i, j;
	unsigned char parity_val;
	int loop = 0;	

	//Print New Value with Odd Parity 
	for (i = 0; i < outputSize; i+=8)
	{
		output[i+loop] = input[i];
		int savebits = 0;
		char mask = 0xFE;
	
		for (j = 1; j < 9; j++)
		{
			parity_val = odd_parity (input[i+j-1]);
			output[i+j+loop] =  ( savebits | input[i+j]>>j | (parity_val<<(8-j)) );
		
			savebits = (input[i+j]&~(mask));
			savebits = savebits << (8-j);
			mask = mask << 1;
		}
		
		loop++;	
	}
		
}

void add_even_parity(unsigned char * input, int inputSize, unsigned char * output, int outputSize)
{

	int i,j;
	unsigned char parity_val;
	int loop = 0;	
	
	//Print New Value with Even Parity 
	for (i = 0; i < outputSize; i+=8)
	{
		output[i+loop] = input[i];
		int savebits = 0;
		char mask = 0xFE;
	
		for (j = 1; j < 9; j++)
		{
		
			parity_val = even_parity (input[i+j-1]);
			output[i+j+loop] =  ( savebits | input[i+j]>>j | (parity_val<<(8-j)) );
		
			savebits = (input[i+j]&~(mask));
			savebits = savebits << (8-j);
			mask = mask << 1;
		}
		
		loop++;	
	}
	
}

void remove_parity(unsigned char * input, int inputSize, unsigned char * output, int outputSize)
{
	int i, j;
	
	int loop = 0;
	output[0] = input[0];
	
	for (i = 8; i < inputSize; i+=9)
	{
		output[i-loop] = input[i+1];
		loop ++;
	}
	
	j = 0;
	loop = 0;
	
	for (j = 0; j < inputSize; j+=9)
	{
		char mask = 0x7F;
		for (i = 1; i < 8; i++)
		{
			output[i+j-loop] = input[i+j]<<i | (input[i+j+1]&~mask)>>(8-i);
			mask = mask>>1;
		}

		loop ++;
	}
}
