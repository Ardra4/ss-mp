#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() // Changed void to int
{
    char opcode[10], operand[10], label[10], a[10], ad[10], symbol[10], ch;
    char code[10][10], code1[10][10] = {"33", "44", "53", "57"};
    char mnemonic[10][10] = {"START", "LDA", "STA", "LDCH", "STCH", "END"};
    char mnemonic1[10][10] = {"LDA", "STA", "LDCH", "STCH"};
    int locctr, start, length, i = 0, j = 0, k, l = 0;
    int st, diff, address = 0, add, len, actual_len, finaddr, prevaddr = 0;
    
    FILE *fp1 = fopen("input.txt", "r");
    FILE *fp2 = fopen("symtab.txt", "w");
    FILE *fp3 = fopen("intermediate.txt", "w");
    
    if (!fp1 || !fp2 || !fp3) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    fscanf(fp1, "%s%s%s", label, opcode, operand);
    
    if (strcmp(opcode, "START") == 0) {
        start = atoi(operand);
        locctr = start;
        fprintf(fp3, "%s\t%s\t%s\n", label, opcode, operand);
        fscanf(fp1, "%s%s%s", label, opcode, operand);
    } else {
        locctr = 0;
    }

    while (strcmp(opcode, "END") != 0) {
        fprintf(fp3, "%d\t%s\t%s\t%s\n", locctr, label, opcode, operand);
        
        if (strcmp(label, "**") != 0) {
            fprintf(fp2, "%s\t%d\n", label, locctr);
        }
        
        // Find the corresponding mnemonic
        j = 0; // Reset j for each new opcode
        while (j < 6 && strcmp(opcode, mnemonic[j]) != 0) {
            j++;
        }
        
        if (j < 6) { // If mnemonic found
            locctr += 3; // Assuming all opcodes take 3 bytes
        } else if (strcmp(opcode, "WORD") == 0) {
            locctr += 3;
        } else if (strcmp(opcode, "RESW") == 0) {
            locctr += (3 * atoi(operand));
        } else if (strcmp(opcode, "RESB") == 0) {
            locctr += atoi(operand);
        } else if (strcmp(opcode, "BYTE") == 0) {
            len = strlen(operand);
            actual_len = len - 3; // Assuming 'C' or 'X' format
            locctr += actual_len; // Add actual length for BYTE
        }

        fscanf(fp1, "%s%s%s", label, opcode, operand);
    }

    fprintf(fp3,"%d\t%s\t%s\t%s\n", locctr,label ,opcode ,operand);
    length = locctr - start;

    fclose(fp3);
    fclose(fp2);
    fclose(fp1);

    printf("\n\nThe contents of Input file:\n\n");
    
    fp1 = fopen("input.txt", "r");
    
    if (!fp1) {
        perror("Error opening input.txt");
        return EXIT_FAILURE;
    }
    
    ch = fgetc(fp1);
    
    while (ch != EOF) {
        printf("%c", ch);
        ch = fgetc(fp1);
    }
    
    printf("\n\nLength of the input program is %d.", length);
    
    printf("\n\nThe contents of Symbol Table:\n\n");
    
    fp2 = fopen("symtab.txt", "r");
    
    if (!fp2) {
        perror("Error opening symtab.txt");
        return EXIT_FAILURE;
    }
    
    ch = fgetc(fp2);
    
    while (ch != EOF) {
        printf("%c", ch);
        ch = fgetc(fp2);
    }

    fclose(fp2);
    fclose(fp1);

    FILE *fp4 = fopen("output.txt", "w");
    FILE *fp5 = fopen("symtab.txt", "r");
    FILE *fp6 = fopen("intermediate.txt", "r");
    FILE *fp7 = fopen("objcode.txt", "w");

    if (!fp4 || !fp5 || !fp6 || !fp7) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    fscanf(fp6,"%s%s%s", label ,opcode ,operand);

    while (strcmp(opcode,"END") != 0)
    {
        prevaddr = address; // Store previous address
        fscanf(fp6,"%d%s%s%s",&address,label ,opcode ,operand); // Read next line
       
       // Ensure we handle START properly
       if (strcmp(opcode,"START") == 0)
       {
           fprintf(fp4,"\t%s\t%s\t%s\n",label ,opcode ,operand);
           fprintf(fp7,"H^%s^00%s^00%d\n",label ,operand ,finaddr); 
           st = address; 
           diff = prevaddr - st; 
           fprintf(fp7,"T^00%d^%d",address,diff); 
       }
       
       // Handle BYTE opcode
       if (strcmp(opcode,"BYTE") == 0)
       {
           fprintf(fp4,"%d\t%s\t%s\t%s\t" ,address,label ,opcode ,operand); 
           len=strlen(operand); 
           actual_len=len-3; 
           fprintf(fp7,"^"); 
           for(k=2;k<(actual_len+2);k++) 
           {   
               sprintf(ad,"%02X",(unsigned char)operand[k]); // Convert character to hex
               fprintf(fp4,"%s" ,ad); 
               fprintf(fp7,"%s" ,ad); 
           }
           fprintf(fp4,"\n"); 
       }
       else if(strcmp(opcode,"WORD")==0)
       {
           sprintf(a,"%05d" ,atoi(operand)); // Format as a zero-padded string of length 5
           fprintf(fp4,"%d\t%s\t%s\t%s\t00000%s\n" ,address,label ,opcode ,operand ,a); 
           fprintf(fp7,"^00000%s" ,a); 
       }
       else if((strcmp(opcode,"RESB")==0)||(strcmp(opcode,"RESW")==0))
       {
           fprintf(fp4,"%d\t%s\t%s\t%s\n" ,address,label ,opcode ,operand); 
       }
       else
       {
           l=0; // Reset l for each new opcode
           
           while(strcmp(opcode,mnemonic1[l])!=0 && l<4) // Ensure we do not exceed bounds
               l++;
           
           if(l<4) { // If found within bounds
               if(strcmp(operand,"COPY")==0)
                   fprintf(fp4,"%d\t%s\t%s\t%s\t%s0000\n" ,address,label ,opcode ,operand ,code1[l]); 
               else
               {
                   rewind(fp5); 
                   fscanf(fp5,"%s%d" ,symbol,&add); 
                   while(strcmp(operand,symbol)!=0 && !feof(fp5))
                       fscanf(fp5,"%s%d" ,symbol,&add); 
                   fprintf(fp4,"%d\t%s\t%s\t%s\t%s%d\n" ,address,label ,opcode ,operand ,code1[l] ,add); 
                   fprintf(fp7,"^%s%d" ,code1[l] ,add); 
               }
           }
       }
   }

   fprintf(fp4,"%d\t%s\t%s\t%s\n" ,address,label ,opcode ,operand); 
   fprintf(fp7,"\nE^00%d" ,st); 

   printf("\nObject Program has been generated.");

   fclose(fp7);
   fclose(fp6);
   fclose(fp5);
   fclose(fp4);

   printf("\n\nObject Program:\n\n");

   fp7=fopen("objcode.txt","r"); 

   if (!fp7) {
       perror("Error opening objcode.txt");
       return EXIT_FAILURE;
   }

   ch=fgetc(fp7); 

   while(ch!=EOF)
   {
       printf("%c" ,ch); 
       ch=fgetc(fp7); 
   }

   fclose(fp7);

   return EXIT_SUCCESS; // Added return statement for main function
}


