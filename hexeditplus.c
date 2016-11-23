#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define buffer_size 4*1024
#define input_size 100

char* buffer;
int size = 1;
char filename[100] = {0};
int debug = 0;

struct fun_desc
{
	char *name;
	void (*fun)();
};
void Toggle_Debug_Mode()
{
	if (debug)
	{
		debug = 0;
		printf("Debug flag now off\n");
	}
	else
	{
		debug = 1;
		printf("Debug flag now on\n");		
	}
}

void Set_File_Name()
{
	int len;
	printf("enter file name:\n");
	fgets(filename, 100, stdin);
	len = strlen(filename);
	filename[len-1]='\0';
	if (debug){printf("Debug: file name set to %s\n", filename);}
}

void Set_Unit_Size()
{
	char str[10];
	int input = 0;
	printf("enter a number:\n");
	fgets(str, 10, stdin);
	if (isdigit(*str) == 0)
	{
		printf("unvaild input\n");
		exit(0);
	}
	input = atoi(str);

	if (input == 1 || input == 2 || input == 4)
	{
		size = input;
		if (debug){printf("Debug: set size to %i\n", size);}
	}
	else{fprintf(stderr,"input number not vaild\n");}
}

void Mem_Display()
{
	char* input = (char*) calloc(input_size, 1);
	char* address;
	int length = 1;
	int i, j;
	int idx = 0;
	printf("Please enter <address> <length>\n");
	fgets(input, input_size, stdin);
	sscanf(input, "%p %d", &address, &length);
	if (debug){printf("Default value: %p\n", buffer);}

	if (address == 0) {address = buffer;}

	for (i = 0; i < length; ++i)
	{
		for (j = idx; j < size + idx; ++j)
		{
			printf("%02hhX", (*(address+j)));
		}
		idx = j;
		printf(" ");
	}
	printf("\n");
	free(input);
}

void Load_Into_Memory()
{
	char* input = (char*) calloc(input_size, 1);
	char* mem_address;
	int location;
	int length = 1;
	FILE *file;
	/*char* p;*/
	/*int i, j;
	int idx = 0;*/
	if (!filename[0])
	{
		fprintf(stderr,"file is NULL\n");
		return;
	}
	file = fopen(filename,"r");
	if (file == NULL)
	{
		fprintf(stderr, "can't open file\n");
		return;
	}	
	printf("Please enter <mem-address> <location> <length>\n");
	fgets(input, input_size, stdin);
	sscanf(input, "%p %x %d", &mem_address, &location, &length);
	if (debug)
	{
		printf("File Name: %s\n", filename);
		printf("Memory address: %p\n", mem_address);
		printf("Location in file: %p\n", (void*)location);
		printf("length: %i\n", length);
	}

	lseek(fileno(file), location, SEEK_SET);

	if (mem_address==0)
		mem_address = buffer;

	read(fileno(file), mem_address, size * length);

	close(fileno(file));
	printf("Loaded %d units into %p\n", length, mem_address);
	free(input);
}

void Save_Into_File(){
	char* input = (char*) calloc(input_size, 1);
	void* p;
	int s_address, t_location, length, file_size, file;

	if (!filename[0])
	{
		fprintf(stderr,"file is NULL\n");
		return;
	}
	file = open(filename, O_WRONLY);
	if (file == -1)
	{
		fprintf(stderr, "can't open file\n");
		return;
	}

	printf("Please enter <s-address> <t-location> <length>\n");
	fgets(input, input_size, stdin);
	sscanf(input, "%x %x %d", &s_address, &t_location, &length);

	if (debug)
	{
		printf("File Name: %s\n", filename);
		printf("Memory address: %p\n", (void*)s_address);
		printf("Location in file: %p\n", (void*)t_location);
		printf("length: %i\n", length);
	}

	file_size = lseek(file, 0, SEEK_END);

	if (file_size < t_location)
	{
		fprintf(stderr,"file size is smaller then t_location\n");
		return;
	}
	lseek(file, t_location, SEEK_SET);

	if(s_address == 0)
	{
		p = (void*)buffer;
	}
	else
	{
		p = (void*)(s_address);
	}  

	write(file, p, length*size);
	printf("Loaded %d units into %s file\n", length, filename);
	close(file);
	free(input);
}


void Mem_Modify()
{
	char* input = (char*) calloc(input_size, 1);
	int s_address;
	int val;
	void* p;
	void* to_copy;
	int i = 0;
	int temp_size = size;
	printf("Please enter <address><val>\n");
	fgets(input, input_size, stdin);
	sscanf(input, "%x %x", &s_address, &val);
	if (debug)
	{
		printf("Memory address: %p\n", (void*)s_address);
		printf("Value: %p\n", (void*)val);
	}

	to_copy = (void*)(&val);

	if (s_address == 0)
		p = buffer;
	else
		p = (void*)(s_address);

	for (i = 0; i < size; ++i)
	{
		*((char*)(p+temp_size-1)) = *((char*)(to_copy+i));
		--temp_size;
	}
	free(input);
}


void Quit()
{
	if (debug){printf("quitting\n");}
	free(buffer);
	exit(0);
}

void print_Global()
{
	printf("Unit Size: %i\n", size);
	printf("File Name: %s\n", filename);
	printf("Buffer Address: %p\n", buffer);
}

int main(int argc, char **argv)
{
	buffer = (char*) calloc(buffer_size, 1);
	/*int step = 1;*/
	char str[10];
	int pick;
	static char* fun_names[] = {"Toggle Debug Mode","Set File Name","Set Unit Size","Mem Display","Load Into Memory","Save Into File","Mem Modify","Quit",NULL,NULL};
	typedef void (*funp)(void);
	static funp fun_p_array[] = {&Toggle_Debug_Mode,&Set_File_Name,&Set_Unit_Size,&Mem_Display,&Load_Into_Memory,&Save_Into_File,&Mem_Modify,&Quit,NULL,NULL};
	int i;
	struct fun_desc fun_array[10];
	int bound = (sizeof(fun_array)/sizeof(fun_array[0]))-3;

	for (i = 0; fun_names[i] != NULL ; ++i)
	{
		fun_array[i].name = fun_names[i];
		fun_array[i].fun = fun_p_array[i];
    	/*printf("%p\n", fun_p_array[i]);
    	printf("%p\n", fun_array[i].fun);*/
	}
	fun_array[i].name = fun_names[i];
	fun_array[i].fun = fun_p_array[i];

	while(1)
	{
		if (debug)
		{
			print_Global();
		}
		printf("Please choose a function:\n");  
		for (i = 0; fun_names[i] != NULL ; ++i)
		{
			printf("%i) %s\n", i,fun_array[i].name);
		}
		fgets(str, input_size, stdin);
		if (isdigit(*str) == 0)
		{
			printf("unvaild input\n");
			continue;
		}
		pick = atoi(str);
		printf("Function Number: %i\n", pick);

		if (pick <= bound)
		{
			printf("within bounds\n");
			(fun_array[pick].fun)();
			printf("DONE.\n");
		}

		else
		{
			printf("not within bounds\n");
			continue;
		} 
	}


	return 0;  
}


	/*
	char* copy = "1111";
	memcpy(buffer, copy, 4);
	*/
