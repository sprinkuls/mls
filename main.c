#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <getopt.h>

// filter out anything that begins with '.' like hidden folders, '.', '..'
// used by default
int dotfilter(const struct dirent *input) {
    if (input->d_name[0] == '.')
        return 0;
    return 1;
}

// filter out only '.' and '..'
// used with -A (almost all) flag
int relativefilter(const struct dirent *input) {
    if (strcmp(input->d_name, ".") && strcmp(input->d_name, ".."))
        return 1;
    return 0;
}

int main(int argc, char **argv)
{

    // store the filter we're using (-a, -A, or default)
    typeof(int (const struct dirent*)) *filter = dotfilter;

    char **paths;
    int numPaths = 0;

    // https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Options.html
    // https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html
    int c;
    while(1) {
        static struct option long_options[] = {
            {"all",             no_argument, 0, 'a'},
            {"almost-all",      no_argument, 0, 'A'},
            {0, 0, 0, 0}
        };
        int option_index = 0;

        c = getopt_long(argc, argv, "aA1l", long_options, &option_index);

        if (c == -1)
            break;

        switch (c) {
            case 'a':
                //puts ("option -a\n");
                filter = NULL;
                break;
            case 'A':
                //puts("option -A\n");
                filter = relativefilter;
                break;
            case '1':
                puts("option -1\n");
                break;
            case '?': // unrecognized flag
                puts("Try 'mls --help' for more information.");
                exit(EXIT_FAILURE);
                break;
            default: // non-flag argument (like a path!)
                break;
        }
    }

    // store paths (any other arguments passed)
    // would it be better to not store the paths, but rather stat objects
    // or something? idk
    // TODO: validate that the paths given are actually valid
    if (optind < argc) {
        paths = malloc((argc - optind) * sizeof(char*));

        while (optind < argc) {
            paths[numPaths] = argv[optind];
            numPaths++;
            optind++;
        }
    } else {
        paths = malloc(sizeof(char*));
        paths[0] = ".";
        numPaths = 1;
    }


    // TODO: clean this up / figure out long listing
    int lflag = 0;
    int oneflag;


    DIR *d;
    struct dirent *entry;
    struct stat statbuf;
    d = opendir(paths[0]);

    struct dirent **namelist;

    if (numPaths > 1) { // multiple paths given
        //TODO: all for single file paths to be given, not just directories
        for (int i = 0; i < numPaths; i++) {
            printf("%s:\n",paths[i]);

            struct dirent **namelist;
            int numFiles = scandir(paths[0], &namelist, filter, alphasort);
            for (int i = 0; i < numFiles; i++) {
                printf("%s\t", namelist[i]->d_name);
            }

            if (i+1 != numPaths)
                putchar('\n');
            putchar('\n');
        }

    } else { // no paths/files given, so list files for the current dir
        struct dirent **namelist;
        int numFiles = scandir(paths[0], &namelist, filter, alphasort);
        for (int i = 0; i < numFiles; i++) {
            printf("%s\t", namelist[i]->d_name);
        }
        putchar('\n');
    }

    /*
    if (d) {
        int numFiles = scandir(paths[0], &namelist, filter, alphasort);
        for (int i = 0; i < numFiles; i++) {
            // filter out '.' and '..'
            //if (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, ".."))
                printf("%s\t", namelist[i]->d_name);
        }
        //printf("\n%d files found.\n", numFiles-2); // -2 for '.' and '..'
        printf("\n%d files found.\n", numFiles); // -2 for '.' and '..'
    } else {
        printf("'%s': No such directory\n", paths[0]);
    }
    */

    /*
    if (d) {
        // so, is all of this unneccessary :)
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
    */


    return 0;
}
