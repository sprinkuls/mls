#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

int main(int argc, char **argv)
{
    char *dirpath;
    if (argc == 1)
        dirpath = ".";
    else
        dirpath = argv[1];

    // (try to) parse args
    //char c;
    //// trying to be clever: run the 'for' loop until
    //// the arguments don't start with '-'
    //for (int i = 1; (c = argv[i][0]) == '-'; i++) {

    //}

    long size = 1024;

    char *buf, *ptr;
    buf = ptr = NULL;

    // loops until 'ptr' stops pointing to null,
    // which should only happen when we get a successful read from getcwd()
    // copied from https://pubs.opengroup.org/onlinepubs/9699919799/functions/getcwd.html
    for (buf = ptr = NULL; ptr == NULL; size *= 2)
    {
        if ((buf = realloc(buf, size)) == NULL)
        {
            printf("ERROR!\n");
            exit(EXIT_FAILURE);
        }


        ptr = getcwd(buf, size);
        if (ptr == NULL && errno != ERANGE)
        {
            printf("ERROR!\n");
            exit(EXIT_FAILURE);
        }
    }


    DIR *d;
    struct dirent *entry;
    struct stat statbuf;
    d = opendir(dirpath);

    if (d) {
        while ((entry = readdir(d)) != NULL) {
            int dp = strlen(dirpath);
            int dn = strlen(entry->d_name);
            char *pathbuf = calloc(((dp+dn) + 2), sizeof(char)); // +1 for slash +1 nt :-)

            int next = dp; // where the next index to write is

            strcpy(pathbuf, dirpath); // copy the prefix to the path

            // append a slash to the end if there isn't one there already
            if (pathbuf[dp-1] != '/') {
                pathbuf[dp] = '/';
                next++;
            }

            strcpy(pathbuf + next, entry->d_name); // copy the path name

            // i love c!! i love c!!!
            int rc = stat(pathbuf, &statbuf);
            if (rc == 0)
                printf("%ld \t %s \n", statbuf.st_size, pathbuf);
            else
                printf("oh dearie me! %s\n", strerror(errno));

            free(pathbuf); // yes
        }
        closedir(d);
    } else {
        printf("'%s': No such file or directory\n", dirpath);
    }


    return 0;
}
