#include<stdio.h>
#include<stdlib.h>
#include<string.h>

static char *basename(const char *path){
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

static int getbuf(const char *file){

    FILE* fptr = fopen(file, "r");
    if (fptr == NULL) {
        return -1;
    }
    //Using fseek to move the file pointer to the end
    fseek(fptr, 0L, SEEK_END);

    //Using ftell to find the postion of the file pointer
    //In this case the pointer is at end, after using fseek
    int res = ftell(fptr);
    fclose(fptr);
  
    return res+1;
}


static int fexists(const char * file) {

    FILE *fptr;
    if((fptr = fopen(file, "r")) != NULL) {
        fclose(fptr);
        return 1;
    }
    return 0;
}


static void cat(char*file, int lflag) {

    char *pline = NULL, *end=NULL;
    if(lflag)
        pline = "%*d  ";
    
    int buflen = getbuf(file);
    
    FILE * fptr = fopen(file, "r");
    char* buffer = (char*)malloc(sizeof(char) * buflen);
    
    if(buffer == NULL) exit(1);

    int i = 1;
    while (fgets(buffer, buflen, fptr) != NULL){
        fprintf(stdout, pline, 6, i);
        fprintf(stdout, "%s", buffer);
        ++i; 
    }
    free(buffer);
}

int arg_validate(char* arg){
    //will be adding more flags
    if(!strcmp(arg, "-n")){
            return 1;
        }
    return 0;
}

char *getf(int i, char**vec){
    if(i==1)
        return strdup(vec[2]);
    else if (i==2)
        return strdup(vec[1]);
}

int main(int argc, char**argv) {

    char *file = NULL;
    int lflag = 0, Eflag = 0;
    if (argc == 2){
        if(!arg_validate(argv[1]))
            file = argv[1];
    } else if (argc == 3){
        int i;
        for(i = 1; i < 3; i++){
            if(!strcmp(argv[i], "-n")){
                lflag = 1;
                break;
            }
        }
        file = getf(i, argv);
    }

    if(!fexists(file)) {
        fprintf(stderr, "%s: %s: No such file or directory", basename(argv[0]), file);
        return 1;
    }
    cat(file, lflag);
    return 0;

}