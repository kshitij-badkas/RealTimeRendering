#include <stdio.h>

int main(void)
{
    //Step 1. Create FILE Pointer variable
    FILE *pFile = NULL;

    int iFileOpen;
    //Step 2. Create a file to write
    iFileOpen = fopen_s(&pFile, "KAB_Log.txt", "w");
    if(iFileOpen != 0)
    {
        printf("\n\nCannot Open Desired File.\n\n");
        exit(0);
    }

    else if(iFileOpen == 0) //success
    {
        printf("\n\nFile Created Successfully.\n\n");

        //Step 3. Write content to File
        fprintf(pFile, "Bharat is My Country.\n");

        //Step 4. Close File 
        fclose(pFile);

        //Step 5. Clean File Pointer
        pFile = NULL;
    }

    return (0);
}

