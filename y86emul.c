#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y86emul.h"

char * Reg;
unsigned int RegisterBytes;
unsigned long jumpAdr;
char* test;
unsigned int progc; 
int strt;int finish;
unsigned long stacksize;
unsigned char * ptr;
int ZF = 0; 
int SF = 0;
int OF = 0;
unsigned long Regs[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void sizeDirective(char * token)
{
    token = strtok(NULL, "\t\"");
    stacksize = hextodecUnsigned(token, ptr);
    ptr = (char *) calloc(1, stacksize + 1);
}
void stringDirective(char * token)
{
    token = strtok(NULL, "\t\"");
    unsigned long place = hextodecUnsigned(token, ptr);
    token = strtok(NULL, "\t\"");


    int i;
    for(i = 0; i < strlen(token); i++)
    {
        ptr[place + i] = token[i];
    }
}
void longDirective(char * token)
{

    token = strtok(NULL, "\t\"");
    unsigned long place = hextodecUnsigned(token, ptr);
    token = strtok(NULL, "\t\"");

    *((int *) &ptr[place]) = atoi(token);
}

void bssDirective(char * token)
{
        token = strtok(NULL, "\t\"");
        unsigned long place = hextodecUnsigned(token, ptr);
        token = strtok(NULL, "\t\"");


        int size = atoi(token);int i;
        for(i = 0; i <= size; i++)
        {
            ptr[place + i] = (long) NULL;
        }
}
void byteDirective(char * token)
{
    token = strtok(NULL, "\t\"");
    unsigned long place = hextodecUnsigned(token, ptr);
    token = strtok(NULL, "\t\"");


    char byte[3];
    byte[0] = token[0];
    byte[1] = token[1];
    byte[2] = '\0';


    unsigned long byteValue = hextodecUnsigned(byte, ptr);
    ptr[place] = (unsigned char)byteValue;
}

void textDriective(char * token)
{
    token = strtok(NULL, "\t\"");
    unsigned long place = hextodecUnsigned(token, ptr);

    progc = place;
    strt = progc;
    token = strtok(NULL, "\t\"");


    int i;
    char onebyte[3];
    for(i = 0; i < strlen(token); i+=2)
    {
        onebyte[0] = token[i];
        onebyte[1] = token[i+1];
        onebyte[2] = '\0';
        ptr[place] = hextobyte(onebyte, ptr);
        place++;
    }

    finish = place;
}

void execute()
{
    while(progc < finish)
    {
        unsigned int instruction = (unsigned int) ptr[progc];
        if(progc < strt)
        {
            exit(1);
        }
        switch (instruction)
        {
            case 0x00: 
                    progc++; break;

            case 0x10:
                       printf("\ninstruction set stopped at 0x%x. Status: HLT\n", progc);
                       exit(1);
                       progc++; 
                       break;

            case 0x20:
                       test ="yeah";
                       RegisterBytes = ptr[progc + 1];
                       Reg = getByteChars(RegisterBytes, ptr);

                       if(Reg[0] >= 'a' || Reg[1] >= 'a')
                       {
                           printf("Wrong Register Identifier Used.\n");
                           exit(1);
                       }
                       else{
                           Regs[Reg[1] - '0'] = Regs[Reg[0] - '0'];
                       }

                       progc += 2; 
                       break;

            case 0x30:
                       RegisterBytes = ptr[progc + 1];
                       Reg = getByteChars(RegisterBytes, ptr);
                       if(Reg[0] != 'f' || Reg[1] >= 'a')
                       {
                           printf("Wrong Register Identifier Used.\n");
                           exit(1);
                       }
                       else{
                           long immediate = getImmediateValue(progc + 2, ptr);
                           Regs[Reg[1] - '0'] = immediate;
                       }
                       progc += 6;
                       break;

            case 0x40:
                       RegisterBytes = ptr[progc + 1];
                       Reg = getByteChars(RegisterBytes, ptr);
                       if(Reg[0] >= 'a' || Reg[1] >= 'a')
                       {
                           printf("Wrong Register Identifier Used.\n");
                           exit(1);
                       }
                       else{
                           long disp = getImmediateValue(progc + 2, ptr);
                           *((int *) &ptr[disp + Regs[Reg[1] -'0']]) = Regs[Reg[0] - '0'];
                       }
                       progc += 6;
                       break;

            case 0x50:
                       RegisterBytes = ptr[progc + 1];
                       Reg = getByteChars(RegisterBytes, ptr);
                       if(Reg[0] >= 'a' || Reg[1] >= 'a')
                       {
                           printf("Wrong Register Identifier Used.\n");
                           exit(1);
                       }
                       else{
                           long disp = getImmediateValue(progc + 2, ptr);
                           Regs[Reg[0] - '0'] = *((int *) &ptr[disp + Regs[Reg[1] - '0']]);
                       }
                       progc += 6; 
                       break;
            //all the op1 commands
            case 0x60:
                       RegisterBytes = ptr[progc + 1];
                       Reg = getByteChars(RegisterBytes, ptr);
                       OF = 0;
                       SF = 0;
                       ZF = 0;
                       if(Reg[0] >= 'a' || Reg[1] >= 'a'){
                           printf("Wrong Register Identifier Used.\n");
                           exit(1);}
                       else{
                           long long sum = Regs[Reg[1] - '0'] + Regs[Reg[0] - '0'];
                           if((Regs[Reg[0]] > 0 && Regs[Reg[1]] > 0 && sum < 0) ||
                                   (Regs[Reg[0]] < 0 && Regs[Reg[1]] < 0 && sum > 0) ||
                                   (sum > (power(2, 31, ptr) - 1)) || (sum < (-1 * power(2,31, ptr))))
                           {
                               OF = 1;
                               printf("Overflow Detected at 0x%x\n", progc);
                           }
                           if(sum == 0)
                           {
                               ZF = 1;
                           }
                           else if(sum < 0)
                           {
                               SF = 1;
                           }
                           Regs[Reg[1] - '0'] = (long) sum;}
                       progc += 2;
                       break;

            case 0x61:
                       RegisterBytes = ptr[progc + 1];
                       Reg = getByteChars(RegisterBytes, ptr);
                       ZF = 0;
                       SF = 0;
                       OF = 0;
                       if(Reg[0] >= 'a' || Reg[1] >= 'a')
                       {
                           printf("Wrong Register Identifier Used.\n");
                           exit(1);
                       }
                       else{
                           long long difference = Regs[Reg[1] - '0'] - Regs[Reg[0] - '0'];
                           unsigned char duh = 86;
                           if((Regs[Reg[0]] < 0 && Regs[Reg[1]] > 0 && difference < 0) ||
                                   (Regs[Reg[0]] > 0 && Regs[Reg[1]] < 0 && difference > 0) ||
                                   (difference > power(2, 31, ptr) - 1) || (difference < (-1 * power(2,31, &duh)))){
                               OF = 1;
                               printf("Overflow Detected at 0x%x\n", progc);}
                           if(difference == 0){
                               ZF = 1;}
                           else if(difference < 0){
                               SF = 1;}
                           Regs[Reg[1] - '0'] = (long) difference;
                       }
                       progc += 2;
                       break;

            case 0x62:
                       RegisterBytes = ptr[progc + 1];
                       Reg = getByteChars(RegisterBytes, ptr);
                       ZF = 0;
                       SF = 0;
                       if(Reg[0] >= 'a' || Reg[1] >= 'a')
                       {
                           printf("Incorrect Register\n");
                           exit(1);
                       }
                       else{
                           long and = Regs[Reg[1] - '0'] & Regs[Reg[0] -'0'];
                           if(and == 0){
                               ZF = 1;	}
                           else if (and < 0){
                               SF = 1;}
                           Regs[Reg[1] - '0'] = and;
                       }
                       progc += 2;
                       break;

            case 0x63:
                       RegisterBytes = ptr[progc + 1];
                       Reg = getByteChars(RegisterBytes, ptr);
                       ZF = 0;
                       SF = 0;
                       if(Reg[0] >= 'a' || Reg[1] >= 'a')
                       {
                           printf("Wrong Register Identifier Used.\n");
                           exit(1);
                       }
                       else{
                           long xor = Regs[Reg[1] - '0'] ^ Regs[Reg[0] -'0'];
                           if(xor == 0)
                           {
                               ZF = 1;
                           }
                           else if (xor < 0)
                           {
                               SF = 1;
                           }
                           Regs[Reg[1] - '0'] = xor;
                       }
                       progc += 2;
                       break;

            case 0x64:
                       RegisterBytes = ptr[progc + 1];
                       Reg = getByteChars(RegisterBytes, ptr);
                       ZF = 0;
                       SF = 0;
                       OF = 0;
                       if(Reg[0] >= 'a' || Reg[1] >= 'a')
                       {
                           printf("Wrong Register Identifier Used \n");
                           exit(1);
                       }
                       else	
                       {
                           long long product = Regs[Reg[0] - '0'] * Regs[Reg[1] - '0'];
                           if((Regs[Reg[0]] > 0 && Regs[Reg[1]] > 0 && product < 0) ||
                                   (Regs[Reg[0]] < 0 && Regs[Reg[1]] < 0 && product > 0) ||
                                   (Regs[Reg[0]] < 0 && Regs[Reg[1]] > 0 && product > 0) ||
                                   (Regs[Reg[0]] > 0 && Regs[Reg[1]] < 0 && product > 0) ||
                                   product > power(2, 32, ptr) - 1 || product < (-1 * power(2, 31, ptr)))
                           {
                               OF = 1;
                               printf("Overflow Detected at 0x%x\n", progc);
                           }
                           if(product == 0)	
                           {
                               ZF = 1;
                           }
                           else if(product < 0)
                           {
                               SF = 1;
                           }
                           Regs[Reg[1] - '0'] = (long) product;
                       }
                       progc += 2; 
                       break;

            case 0x65:
                       RegisterBytes = ptr[progc + 1];
                       Reg = getByteChars(RegisterBytes, ptr);
                       ZF = 0;
                       SF = 0;
                       OF = 0;
                       if(Reg[0] >= 'a' || Reg[1] >= 'a')
                       {
                           printf("Wrong Register Identifier Used.\n");
                           exit(1);
                       }
                       else{
                           long difference = Regs[Reg[1] - '0'] - Regs[Reg[0] - '0'];
                           unsigned char gotem = 86;
                           if((Regs[Reg[0]] < 0 && Regs[Reg[1]] > 0 && difference < 0) ||
                                   (Regs[Reg[0]] > 0 && Regs[Reg[1]] < 0 && difference > 0) ||
                                   (difference > power(2, 31, ptr) - 1) || (difference < (-1 * power(2,31, &gotem))))
                           {
                               OF = 1;
                               printf("Overflow Detected at 0x%x\n", progc);
                           }
                           if(difference == 0)
                           {
                               ZF = 1;
                           }
                           else if(difference < 0)
                           {
                               SF = 1;
                           }
                       }
                       progc += 2; 
                       break;
            //all the jump commands
            case 0x70:
                       jumpAdr = getplaceValue(progc + 1, ptr);
                       if(jumpAdr < strt || jumpAdr > finish)
                       {
                           printf("Incorrect instructions");
                           exit(1);
                       }
                       else
                       {
                           progc = jumpAdr;
                       }
                       break;

            case 0x71:
                       jumpAdr = getplaceValue(progc + 1, ptr);
                       if(jumpAdr < strt || jumpAdr > finish)
                       {
                           printf("Incorrect Instructiond Detected");
                           exit(1);
                       }
                       else
                       {
                           if((SF ^ OF) | ZF)
                               progc = jumpAdr;
                           else
                               progc += 5;
                       }
                       break;

            case 0x72:
                       jumpAdr = getplaceValue(progc + 1,ptr);
                       if(jumpAdr < strt || jumpAdr > finish)
                       {
                           printf("Incorrect Insturctions Detected");
                           exit(1);
                       }
                       else
                       {
                           if(SF ^ OF)
                               progc = jumpAdr;
                           else
                               progc += 5;
                       }
                       break;

            case 0x73:
                       jumpAdr = getplaceValue(progc + 1, ptr);
                       if(jumpAdr < strt || jumpAdr > finish)
                       {
                           printf("Incorrect Insturctions Detected");
                           exit(1);
                       }
                       else
                       {
                           if(ZF == 1)
                               progc = jumpAdr;
                           else
                               progc += 5;
                       }
                       break;

            case 0x74:
                       jumpAdr = getplaceValue(progc + 1, ptr);
                       if(jumpAdr < strt || jumpAdr > finish)
                       {
                           printf("Incorrect Insturctions Detected");
                           exit(1);
                       }
                       else{
                           if(ZF == 0)
                               progc = jumpAdr;
                           else
                               progc += 5;
                       }
                       break;

            case 0x75:
                       jumpAdr = getplaceValue(progc + 1, ptr);
                       if(jumpAdr < strt || jumpAdr > finish)
                       {
                           printf("Incorrect Insturctions Detected");
                           exit(1);
                       }
                       else{
                           if((SF^OF) == 0)
                               progc = jumpAdr;
                           else
                               progc += 5;}
                       break;

            case 0x76:
                       jumpAdr = getplaceValue(progc + 1, ptr);
                       if(jumpAdr < strt || jumpAdr > finish)
                       {
                           printf("Incorrect Insturctions Detected");
                           exit(1);
                       }
                       else{
                           if(~(SF ^ OF) & ~ZF)
                               progc = jumpAdr;
                           else
                               progc += 5;
                       }
                       break;

            case 0x80:
                       test = "yeaher";
                       unsigned long place = getplaceValue(progc + 1, ptr);
                       progc += 5;
                       if(place < strt || place > finish)
                       {
                           printf("Incorrect Insturctions Detected");
                           exit(1);
                       }
                       else{
                           Regs[4] -= 4;
                           *((int *) &ptr[Regs[4]]) = progc;
                           progc = place;
                       }
                       break;

            case 0x90:
                       progc = *((int*)&ptr[Regs[4]]);
                       Regs[4] += 4; break;

            case 0xa0:
                       RegisterBytes = ptr[progc + 1];
                       Reg = getByteChars(RegisterBytes, ptr);
                       if(Reg[0] >= 'a' || Reg[1] != 'f')
                       {
                           printf("Incorrect Register Detected");
                           exit(1);
                       }
                       else{
                           Regs[4] -= 4;
                           *((int*) &ptr[Regs[4]]) = Regs[Reg[0] - '0'];
                       }
                       progc += 2; break;

            case 0xb0:
                       RegisterBytes = ptr[progc + 1];
                       Reg = getByteChars(RegisterBytes, ptr);

                       if(Reg[0] >= 'a' || Reg[1] != 'f')
                       {
                           printf("Incorrect Register Detected");
                           exit(1);
                       }
                       else{
                           Regs[Reg[0] - '0'] = *((int *) &ptr[Regs[4]]);
                           Regs[4] += 4;
                       }

                       progc += 2;
                       break;
            //read and write
            case 0xc0:
                       RegisterBytes = ptr[progc + 1];
                       Reg = getByteChars(RegisterBytes, ptr);
                       ZF = 0;

                       if(Reg[0] >= 'a' || Reg[1] != 'f')
                       {
                           printf("Incorrect Register Detected");
                           exit(1);
                       }
                       else{
                           long immediate = getImmediateValue(progc + 2, ptr);
                           char byte[3];
                           byte[2] = '\0';

                           if(scanf("%hhx", byte) != EOF)
                           {
                               long place = Regs[Reg[0] - '0'] + immediate;
                               if(place < finish)
                               {
                                   printf("ERROR");
                                   exit(1);
                               }
                               else{
                                   if(byte[0] > 'f' || byte[1] > 'f')
                                   {
                                       printf("ERROR:Wrong format. Please enter proper format\n");
                                   }
                                   else{
                                       ptr[place] = hextodecUnsigned(byte, ptr);
                                       if(ptr[place] == 0)
                                       {
                                           ZF = 1;
                                       }
                                   }
                               }
                           }
                           else{
                               ZF = 1;
                           }
                       }
                       progc += 6;
                       break;

            case 0xc1:
                       RegisterBytes = ptr[progc + 1];
                       Reg = getByteChars(RegisterBytes, ptr);
                       ZF = 0;

                       if(Reg[0] >= 'a' || Reg[1] != 'f')
                       {
                           printf("Incorrect Register Detected");
                           exit(1);	
                       }
                       else{
                           long immediate = getImmediateValue(progc + 2, ptr);
                           unsigned long int word[32];
                           word[31] = '\0';
                           if(scanf("%ld", word) != EOF)
                           {
                               *((unsigned long int *) (&ptr[Regs[Reg[0] - '0'] + immediate])) = *word;
                               if(*((unsigned long int *) (&ptr[Regs[Reg[0] - '0'] + immediate])) == 0 )
                               {
                                   ZF = 1;
                               }
                           }
                           else
                           {
                               ZF = 1;
                           }
                       }
                       progc += 6; 
                       break;

            case 0xd0:
                       RegisterBytes = ptr[progc + 1];
                       Reg = getByteChars(RegisterBytes, ptr);
                       if(Reg[0] >= 'a' || Reg[1] != 'f')
                       {
                           printf("Incorrect Register Detected");
                           exit(1);
                       }
                       else
                       {
                           long immediate = getImmediateValue(progc + 2, ptr);
                           unsigned char byte = (unsigned char) ptr[Regs[Reg[0] - '0'] + immediate];
                           printf("%c", byte);
                       }
                       progc += 6; 
                       break;

            case 0xd1:
                       RegisterBytes = ptr[progc + 1];
                       Reg = getByteChars(RegisterBytes, ptr);

                       if(Reg[0] >= 'a' || Reg[1] != 'f')
                       {
                           printf("Incorrect Register Detected");
                           exit(1);
                       }
                       else{
                           long immediate = getImmediateValue(progc + 2, ptr);
                           unsigned long int word = *((int *) &ptr[Regs[Reg[0] - '0'] + immediate]);
                           printf("%ld", word);
                       }
                       progc += 6;
                       break;

            case 0xe0:
                       RegisterBytes = ptr[progc + 1];
                       Reg = getByteChars(RegisterBytes, ptr);

                       if(Reg[0] >= 'a' || Reg[1] >= 'a')
                       {
                           printf("Wrong Register Identifier Used.\n");
                           exit(1);
                       }
                       else
                       {
                           long disp = getImmediateValue(progc + 2, ptr);
                           Regs[Reg[0] - '0'] =  ptr[disp + Regs[Reg[1] - '0']];
                       }
                       progc += 6; 
                       break;


            default: fprintf(stderr, "Instructions were stopped with the program counter at 0x%x. Status: INS\n" , progc); exit(1); break;
        }
    }
}

void main(int argc, char * argv[]){
        if(strcmp(argv[1], "-h") == 0){
        printf("This is a y86 emulator. it'll take a textfile, parse through it, and execute the equivalent commands which out to be given as ASCII y86 instructions. prog1.y86 can be used as an example\n");
        exit(1);}
    FILE * ptrfile = fopen(argv[1], "r");
    if(ptrfile == NULL){
        fprintf(stderr, "ERROR 404:file was not found\n");
        exit(1);} char line[90000];
    fgets(line, sizeof(line)/sizeof(char), ptrfile);
    char * tokenized = strtok(line, "\t\"");
    if(strcmp(tokenized, ".size") != 0){
        fprintf(stderr, "ERROR: No directions found. Please fix file and try again.\n");
        exit(1);}
    else{
        sizeDirective(tokenized);}
    int textFlag = 0;
    memset(tokenized, 0, sizeof(tokenized)/sizeof(char));
    while(fgets(line, sizeof(line)/sizeof(char), ptrfile)){
        tokenized = strtok(line, "\t\"");
        if(strcmp(tokenized, ".string") == 0){
            stringDirective(tokenized);}
        else if(strcmp(tokenized, ".long") == 0){
            longDirective(tokenized);}
        else if(strcmp(tokenized, ".bss") == 0){
            bssDirective(tokenized);}
        else if(strcmp(tokenized, ".byte") == 0){
            byteDirective(tokenized);}
        else if(strcmp(tokenized, ".text") == 0){
            if(textFlag == 0){
                textFlag == 1;
                textDriective(tokenized);}
            else{
                fprintf(stderr, "ERROR: An error was detected in the file format, the diretives must be wrong\n");
                exit(1);}	}
        memset(tokenized, 0, sizeof(tokenized)/sizeof(char));}
    execute();
    free(ptr);
}
