/*
 * Решение задачи о внешней сортировке - необходимо отсортировать строки в большом входном файле, не вмещающемся в оперативную память:

 * В файле "input.txt" содержатся строки символов, длина каждой строки не превышает 10000 байт. 
 * Файл нужно отсортировать в лексикографическом порядке и вывести результат в файл "output.txt". 
 * Вот беда, файл занимает много мегабайт, а в вашем распоряжение оказывается вычислительная система с памятью всего в 4 мегабайта. Но файл должен быть отсортирован!
 * Входной файл "input.txt" содержит неотсортированные строки символов без управляющих и пробельных (ASCII коды от 33 до 126).
 * 
 * Ограничение по времени: 4 c (отработал за 0.322 с)
 * Ограничение размера стека: 1 М
 * Ограничение памяти: 4 M

 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <assert.h>

class MemoryManager
{
    char *memory_;
    char *nextFree_;
    char *upperAllocated_;

    const int SIZE = 2 * 1024 * 1024 + 512 * 1024;

    inline int roundSize (int sz) const
    {;
        return sz - 1 + 8 - ((sz - 1) & 7);
    }

public:

    MemoryManager ()
    {
        memory_ = (char*) calloc (SIZE, 1);
        nextFree_ = memory_;
        upperAllocated_ = nullptr;
    }

    ~MemoryManager ()
    {
        free (memory_);
    }

    void* allocate (int nbytes)
    {
        nbytes = roundSize(nbytes);

        if (nextFree_ + nbytes - memory_ > SIZE)
            return nullptr;
        upperAllocated_ = nextFree_;
        nextFree_ += nbytes;
        return (void*)upperAllocated_;
    }

    inline void shrinkUpper (int new_bytes)
    {
        nextFree_ = upperAllocated_ + roundSize(new_bytes);
    }

    inline void flushUpperThan (int index)
    {
        nextFree_ = memory_ + roundSize(index);
        upperAllocated_ = memory_;
    }
};

#define PREFIX "ExternalSortBroughtToYouByDaKarakum_"
const char *STRIP [2][2] = {{PREFIX "00.txt", PREFIX "01.txt"}, {PREFIX "10.txt", PREFIX "11.txt"}};
const char *CHUNK_TERM = "\xfe" "\n";

void copyRemaining (FILE *src, FILE *dest, char *buffer, int buffer_sz)
{
    while (1)
    {
        *buffer = '\0';
        fgets (buffer, buffer_sz, src);
        if (*buffer == '\0')
            break;
        fputs (buffer, dest);
    }
}

void prepareFiles ()
{
    FILE *_;
    _ = fopen (STRIP[0][0], "w");
    fclose (_);
    _ = fopen (STRIP[0][1], "w");
    fclose (_);
    _ = fopen (STRIP[1][0], "w");
    fclose (_);
    _ = fopen (STRIP[1][1], "w");
    fclose (_);
    
}

int main ()
{
    prepareFiles ();
    MemoryManager mgr;

    char **chunk = (char**) mgr.allocate (16384 * sizeof (*chunk));

    FILE *in = fopen ("input.txt", "r");

    bool fileIsNotEmpty = true;
    int parity = 0;
    int size = 0;
    while (fileIsNotEmpty)
    {
        int strCount;

        for (strCount = 0; strCount < 16384; strCount++)
        {
            // assert (((char*)chunk)[32768 * 8 + size] == 0);
            chunk[strCount] = (char*) mgr.allocate (10002);
            if (chunk[strCount] == nullptr)
            {
                break;
            }
            int length = 0;
            fscanf (in, "%s%n", chunk[strCount], &length);
            if (length == 0)
            {
                fileIsNotEmpty = false;
                mgr.shrinkUpper (0);
                break;
            }
            mgr.shrinkUpper (length + 1);
            size += length + 1 + 7 - ((length + 1 - 1) & 7);
        
        }
        

        std::sort (chunk, chunk + strCount, [] (char *&lhs, char *&rhs) -> bool { return strcmp (lhs, rhs) < 0; });
        chunk[0]; chunk[1]; chunk[2]; chunk[3]; 

        FILE *strip = fopen (STRIP[parity][0], "a");

        for (int i = 0; i < strCount; i++)
        {
            fputs (chunk[i], strip);
            fputc ('\n', strip);
        }

        fputs (CHUNK_TERM, strip);
        // fputc ('\n', strip);

        fclose (strip);
        parity ^= 1;

        mgr.flushUpperThan (32768 * sizeof (*chunk));
    }

    fclose (in);
    parity = 1;

    /* смотрим по чанку из STRIP[0][parity^1], STRIP[1][parity^1]. Сливаем поочерёдно в STRIP[0][parity], STRIP[1][parity].
    До тех пор, пока не останется один большой чанк */

    // изначально чанки есть в [0][0] и, возможно, [1][0]
    char *string1 = (char*) mgr.allocate (10002);
    char *string2 = (char*) mgr.allocate (10002);

    int parityWriting = 0;
    int chunksCount = 0;

    while (chunksCount != 1)
    {
        chunksCount = 0;
        FILE *read1 = fopen (STRIP[0][parity^1], "r");
        FILE *read2 = fopen (STRIP[1][parity^1], "r");
        char ch;
        if ((ch = fgetc (read2)) == EOF)
        {
            fclose (read2);
            FILE *prepare = fopen (STRIP[1][parity^1], "w");
            fputs (CHUNK_TERM, prepare);
            fclose (prepare);
            read2 = fopen (STRIP[1][parity^1], "r");
        }
        else
            fseek (read2, 0, SEEK_SET);

        int wasEof = 0;
        while (!wasEof)
        {
            FILE *write = fopen (STRIP[parityWriting][parity], "a");
 
            *string1 = '\0'; *string2 = '\0';

            fgets (string1, 10002, read1);
            fgets (string2, 10002, read2);
            if (*string1 == '\0') // EOF on read1
            {
                if (*string2 != '\0')
                {
                    fputs (string2, write);
                    chunksCount++;
                    copyRemaining (read2, write, string2, 10002);
                }
                wasEof = 1;
            }
            else if (*string2 == '\0') // EOF on read2
            {
                fputs (string1, write);
                chunksCount++;
                copyRemaining (read1, write, string1, 10002);
                wasEof = 1;
            }

            else
            while (1)
            {
                int firstLower = strcmp (string1, string2);
                if (firstLower < 0)
                {
                    if (string2[0] == *CHUNK_TERM)
                    { // break
                        while (string1[0] != *CHUNK_TERM)
                        {
                            fputs (string1, write);
                            fgets (string1, 10002, read1);
                        }
                        fputs (CHUNK_TERM, write);
                        chunksCount++;
                        break;
                    }
                    else
                    {
                        fputs (string1, write);
                        fgets (string1, 10002, read1);
                    }
                }
                else if (firstLower > 0)
                {
                    if (string1[0] == *CHUNK_TERM)
                    { // break
                        while (string2[0] != *CHUNK_TERM)
                        {
                            fputs (string2, write);
                            fgets (string2, 10002, read2);
                        }
                        fputs (CHUNK_TERM, write);
                        chunksCount++;
                        break;
                    }
                    else
                    {
                        fputs (string2, write);
                        fgets (string2, 10002, read2);
                    }
                }
                else
                {
                    if (string1[0] == *CHUNK_TERM)
                    { // break
                        fputs (CHUNK_TERM, write);
                        chunksCount++;
                        break;
                    }
                    fputs (string1, write);
                    fputs (string2, write);
                    fgets (string1, 10002, read1);
                    fgets (string2, 10002, read2);
                }
            }

            fclose (write);
            parityWriting ^= 1;
        }

        fclose (read1);
        fclose (read2);
        parity ^= 1;

        fclose (fopen (STRIP[0][parity], "w"));
        fclose (fopen (STRIP[1][parity], "w"));
    }


    mgr.flushUpperThan (0);
    char *string = (char*) mgr.allocate (10002);

    FILE *sorted = fopen (STRIP[parityWriting][parity^1], "r");
    FILE *output = fopen ("output.txt", "w");

    *string = '\0';
    fgets (string, 10002, sorted);
    while (*string != '\0')
    {
        if (*string != *CHUNK_TERM) fputs (string, output);
        *string = '\0';
        fgets (string, 10002, sorted);
    }

    fclose (output);
    fclose (sorted);
    return 0;

}
