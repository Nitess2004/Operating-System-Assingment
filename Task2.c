#include <stdio.h>
#include <string.h>

#define FRAMES 4
#define TOTAL 10

char cars[TOTAL][10]={
"A101",
"B202",
"C303",
"D404",
"A101",
"E505",
"B202",
"F606",
"A101",
"C303"
};

void FIFO()
{
    char frame[FRAMES][10];
    int hits=0,faults=0;
    int index=0;

    for(int i=0;i<FRAMES;i++)
        strcpy(frame[i],"-");

    printf("\n========== FIFO ==========\n\n");

    for(int i=0;i<TOTAL;i++)
    {
        int found=0;

        for(int j=0;j<FRAMES;j++)
        {
            if(strcmp(frame[j],cars[i])==0)
            {
                found=1;
                hits++;
                break;
            }
        }

        if(!found)
        {
            strcpy(frame[index],cars[i]);
            index=(index+1)%FRAMES;
            faults++;
        }

        printf("%-5s -> ",cars[i]);

        for(int j=0;j<FRAMES;j++)
            printf("%-6s ",frame[j]);

        printf("\n");
    }

    printf("\nHits        : %d\n",hits);
    printf("Page Faults : %d\n",faults);
    printf("Hit Ratio   : %.2f\n",(float)hits/TOTAL);
    printf("Miss Ratio  : %.2f\n",(float)faults/TOTAL);
}

void LRU()
{
    char frame[FRAMES][10];
    int recent[FRAMES];
    int hits=0,faults=0;

    for(int i=0;i<FRAMES;i++)
    {
        strcpy(frame[i],"-");
        recent[i]=-1;
    }

    printf("\n========== LRU ==========\n\n");

    for(int i=0;i<TOTAL;i++)
    {
        int found=0;

        for(int j=0;j<FRAMES;j++)
        {
            if(strcmp(frame[j],cars[i])==0)
            {
                hits++;
                recent[j]=i;
                found=1;
                break;
            }
        }

        if(!found)
        {
            int pos=0;

            for(int j=1;j<FRAMES;j++)
            {
                if(recent[j]<recent[pos])
                    pos=j;
            }

            strcpy(frame[pos],cars[i]);
            recent[pos]=i;
            faults++;
        }

        printf("%-5s -> ",cars[i]);

        for(int j=0;j<FRAMES;j++)
            printf("%-6s ",frame[j]);

        printf("\n");
    }

    printf("\nHits        : %d\n",hits);
    printf("Page Faults : %d\n",faults);
    printf("Hit Ratio   : %.2f\n",(float)hits/TOTAL);
    printf("Miss Ratio  : %.2f\n",(float)faults/TOTAL);
}

int main()
{
    printf("========================================\n");
    printf("   PARKING LOT MEMORY SIMULATION\n");
    printf("========================================\n");
    printf("Parking Spaces (Frames): %d\n",FRAMES);

    printf("\nCar Arrival Sequence:\n");

    for(int i=0;i<TOTAL;i++)
        printf("%s ",cars[i]);

    printf("\n");

    FIFO();

    LRU();

    return 0;
}
