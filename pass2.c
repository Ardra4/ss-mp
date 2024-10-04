#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
void display();
 
int main()
{
    char a[10], ad[10], label[10], opcode[10], operand[10], symbol[10];
    int start, diff, i, address = 0, add, len, actual_len, finaddr, prevaddr = 0, j = 0;
    char mnemonic[15][15] = {"LDA", "STA", "LDCH", "STCH", "COPY"};
    char code[15][15] = {"33", "44", "53", "57", "00"}; // Assuming COPY has a code of 00
 
    FILE *fp1 = fopen("output.txt", "w");
    FILE *fp2 = fopen("symtab.txt", "r");
    FILE *fp3 = fopen("intermediate.txt", "r");
    FILE *fp4 = fopen("objcode.txt", "w");
 
    if (!fp1 || !fp2 || !fp3 || !fp4) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
 
    fscanf(fp3, "%s\t%s\t%s", label, opcode, operand);
 
    while (strcmp(opcode, "END") != 0)
    {
        prevaddr = address;
        fscanf(fp3, "%d%s%s%s", &address, label, opcode, operand);
    }
    finaddr = address;
 
    fclose(fp3);
    fp3 = fopen("intermediate.txt", "r");
 
    fscanf(fp3, "\t%s\t%s\t%s", label, opcode, operand);
    if (strcmp(opcode, "START") == 0)
    {
        fprintf(fp1, "\t%s\t%s\t%s\n", label, opcode, operand);
        fprintf(fp4, "H^%s^00%s^00%d\n", label, operand, finaddr);
        fscanf(fp3, "%d%s%s%s", &address, label, opcode, operand);
        start = address;
        diff = prevaddr - start;
        fprintf(fp4, "T^00%d^%d", address, diff);
    }
 
    while (strcmp(opcode, "END") != 0)
    {
        if (strcmp(opcode, "BYTE") == 0)
        {
            fprintf(fp1, "%d\t%s\t%s\t%s\t", address, label, opcode, operand);
            len = strlen(operand);
            actual_len = len - 3; // Assuming the BYTE format is 'C' or 'X'
            fprintf(fp4, "^");
            for (i = 2; i < (actual_len + 2); i++)
            {   
                sprintf(ad, "%02X", (unsigned char)operand[i]); // Convert to hex
                fprintf(fp1, "%s", ad);
                fprintf(fp4, "%s", ad);
            }
            fprintf(fp1, "\n");
        }
        else if (strcmp(opcode, "WORD") == 0)
        {
            sprintf(a,"%05d", atoi(operand)); // Format as 5 digits
            fprintf(fp1, "%d\t%s\t%s\t%s\t00000%s\n", address, label, opcode, operand, a);
            fprintf(fp4,"^00000%s", a);
        }
        else if ((strcmp(opcode,"RESB") == 0) || (strcmp(opcode,"RESW") == 0))
        {
            fprintf(fp1,"%d\t%s\t%s\t%s\n", address,label,opcode,operand);
        }
        else
        {
            j = 0; // Reset j for each new opcode
            while (j < 15 && strcmp(opcode,mnemonic[j]) != 0) j++;
            if (j < 15) { // Check if found
                if (strcmp(operand,"COPY") == 0)
                    fprintf(fp1,"%d\t%s\t%s\t%s\t%s0000\n",address,label,opcode,operand ,code[j]);
                else
                {
                    rewind(fp2);
                    fscanf(fp2,"%s%d",symbol,&add);
                    while(strcmp(operand,symbol) != 0 && !feof(fp2))
                        fscanf(fp2,"%s%d",symbol,&add);
 
                    if (!feof(fp2)) { // If symbol found
                        fprintf(fp1,"%d\t%s\t%s\t%s\t%s%d\n",address,label,opcode ,operand ,code[j],add);
                        fprintf(fp4,"^%s%d",code[j],add);
                    }
                }
            }
        }
 
        fscanf(fp3,"%d%s%s%s",&address,label ,opcode ,operand);
    }
 
    fprintf(fp1,"%d\t%s\t%s\t%s\n",address,label ,opcode ,operand);
    fprintf(fp4,"\nE^00%d",start);
 
    fclose(fp4);
    fclose(fp3);
    fclose(fp2);
    fclose(fp1);
 
    display();
 
    return 0;
}
 
void display() {
    char ch;
 
    printf("\nIntermediate file is converted into object code");
 
    printf("\n\nThe contents of Intermediate file:\n\n");
 
    FILE *fp3 = fopen("intermediate.txt","r");
 
    if (!fp3) {
        perror("Error opening intermediate.txt");
        return;
    }
 
    while ((ch = fgetc(fp3)) != EOF)
        putchar(ch); // Using putchar for better performance
 
    fclose(fp3);
 
    printf("\n\nThe contents of Symbol Table :\n\n");
 
    FILE *fp2 = fopen("symtab.txt","r");
 
    if (!fp2) {
        perror("Error opening symtab.txt");
        return;
    }
 
    while ((ch = fgetc(fp2)) != EOF)
        putchar(ch);
 
    fclose(fp2);
 
    printf("\n\nThe contents of Output file :\n\n");
 
    FILE *fp1 = fopen("output.txt","r");
 
    if (!fp1) {
        perror("Error opening output.txt");
        return;
    }
 
    while ((ch = fgetc(fp1)) != EOF)
        putchar(ch);
 
    fclose(fp1);
 
    printf("\n\nThe contents of Object code file :\n\n");
 
    FILE *fp4 = fopen("objcode.txt","r");
 
    if (!fp4) {
        perror("Error opening objcode.txt");
        return;
    }
 
    while ((ch = fgetc(fp4)) != EOF)
        putchar(ch);
 
    fclose(fp4);
}


