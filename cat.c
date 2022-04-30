#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define BUFFER 128

static char*
basename(const char *path){
    char fsep = 0;
    #ifdef __unix__
        fsep = '/'; 
    #elif _WIN32
        fsep = '\\'; 
    #endif

    char *s = strrchr(path, fsep);
    if(!s) 
        return strdup(path);
    else 
        return strdup(s + 1);

}

static int 
purge(char*data, int index, int length){

    int i;
    for (i = index; i < (length)-1; i++){
        *(data+i) = *(data+i+1);
    }
    *(data+i) = '\0';
    length--;
    return length;

}

static long 
getbuf(const char *file){

    FILE* fptr = fopen(file, "r");
    if (fptr == NULL) {
        return -1;
    }
    long prev=ftell(fptr);
    //Using fseek to move the file pointer to the end
    fseek(fptr, 0L, SEEK_END);

    //Using ftell to find the postion of the file pointer
    //In this case the pointer is at end, after using fseek
    long res=ftell(fptr);
    fseek(fptr,prev,SEEK_SET);
    fclose(fptr);
    return res+1;
}


static int 
fexists(const char * file) {

    FILE *fptr;
    if((fptr = fopen(file, "r")) != NULL) {
        fclose(fptr);
        return 1;
    }
    return 0;
}


static void 
cat(const char*file, const int lflag, const int sflag) {

    char *pline = NULL;
    if(lflag)
        pline = "\033[31m%*d\033[0m  ";
    
    long buflen = getbuf(file);
    
    FILE * fptr = fopen(file, "r");
    char* buffer = (char*)malloc(buflen);
    
    if(buffer == NULL) exit(EXIT_FAILURE);
    memset(buffer, 0, buflen);

    int i = 0, s = 0;

    rewind(fptr);
    while (fgets(buffer, buflen, fptr) != NULL){
        if(sflag){
            int len = strlen(buffer);
            if(s >= 1){ 
                for(int j=0; j < len; j++){
                    if (buffer[j] == 13)
                        len = purge(buffer, j, len);
                }
                if(buffer[0] == 10 || buffer[0] == 13 || 
                  (buffer[0]==9 && buffer[1]==10))
                    continue;
            } s++;
        }
        fprintf(stdout, pline, 6, ++i);
        fprintf(stdout, "%s", buffer);
        if((buffer[0] != 10 && buffer[0] != 13))
            s = 0;
    } 
    free(buffer);
}

static int 
arg_validate(const char* arg){
    if(!strncmp(arg, "-n", 2)||
       !strncmp(arg, "-s", 2)){
            return 1;
        }
    return 0;
}

static void 
help(){
    puts("Usage: cat [OPTION]... [FILE]...\
        \nConcatenate FILE to standard output.\
        \n\nOptions:\
        \n    -n   number all output lines\
        \n    -s   strip repeated empty output lines\
        \n\nExamples:\
        \n    cat -n file   Output file's contents with line numbers\
        \n    cat -sn file  Output file's contents with line numbers after stripping repeated empty lines");
}

int 
main(int argc, char**argv) {

    char *file = NULL, inout[BUFFER];
    int lflag = 0, sflag = 0;

    if(argc == 1){
        while(fgets(inout, 127, stdin)){
            fputs(inout, stdout);
        }
        return 0;

    } else if (argc == 2){
        if(!strncmp(argv[1], "-h", 2)){
            help();
            return 0;
        }
        else if(!arg_validate(argv[1]))
            file = argv[1];
    } else if (argc > 2 && argc <= 4){
        for(int i = 1; i < argc; i++){
            if(!strncmp(argv[i], "-n", 2))
                lflag = 1;
            if(!strncmp(argv[i], "-s", 2))
                sflag = 1;
            if(!strncmp(argv[i], "-sn", 3) || !strncmp(argv[i], "-ns", 3)){
                sflag = 1;
                lflag = 1;
            }
            if(!arg_validate(argv[i]))
                file = argv[i];
        }
        
    } else{
        help();
        return 1;
    }
    char *exe = basename(argv[0]);
    if(!fexists(file)) {
        fprintf(stderr, "%s: %s: No such file or directory", exe, file);
        return 1;
    }
    free(exe);
    cat(file, lflag, sflag);
    return 0;

}