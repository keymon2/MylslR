#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

char* timeToString(struct tm *t) {
  static char s[20];

  sprintf(s, "%04d-%02d-%02d %02d:%02d",
              t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
              t->tm_hour, t->tm_min
          );

  return s;
}



int print_file(struct stat a,char *File_name){
    char sten[500];
    char abc[15];
    abc[0] = '\0';
    int flag= 0;
    switch (a.st_mode & S_IFMT){
        case S_IFBLK:  abc[0] = 'b';    break;
        case S_IFCHR:  abc[0] = 'c';    break;
        case S_IFDIR:  abc[0] = 'd';    break;
        case S_IFIFO:  abc[0] = 'p';    break;
        case S_IFSOCK:  abc[0] = 's';   break;
        case S_IFREG:  abc[0] = '-';    break;
        case S_IFLNK:  {abc[0] = 'l'; flag =1;   break;}
    }
//==========================================FILEMODE
    if(a.st_mode & S_IRUSR)  abc[1] = 'r';
    else   abc[1] = '-';

    if(a.st_mode & S_IWUSR)  abc[2] = 'w';
    else   abc[2] = '-';
    
    if(a.st_mode & S_IXUSR)  abc[3] = 'x';
    else   abc[3] = '-';
//==============================================USER
    if(a.st_mode & S_IRGRP)  abc[4] = 'r';
    else   abc[4] = '-';

    if(a.st_mode & S_IWGRP)  abc[5] = 'w';
    else   abc[5] = '-';
    
    if(a.st_mode & S_IXGRP)  abc[6] = 'x';
    else   abc[6] = '-';
//=============================================GRUOP
    if(a.st_mode & S_IROTH)  abc[7] = 'r';
    else   abc[7] = '-';

    if(a.st_mode & S_IWOTH)  abc[8] = 'w';
    else   abc[8] = '-';
    
    if(a.st_mode & S_IXOTH)  abc[9] = 'x';
    else   abc[9] = '-';
//============================================OHTERS

    if(a.st_mode & S_ISUID){
        if(a.st_mode & S_IXUSR) abc[3] = 'S';
        else abc[3] = 's';
    }  
//========================================SET-USER-ID

    if(a.st_mode & S_ISGID){
        if(a.st_mode & S_IXGRP) abc[6]= 'S';
        else abc[6] = 's';
    }
//=======================================SET-GROUP-ID

    if(a.st_mode & S_ISVTX) {
      if(a.st_mode & S_IXOTH) abc[9] = 'T';
    else abc[9] = 't';
    }
    abc[10] = '\0';

    struct passwd *user_name;
    struct group *group_name;

    if( (user_name = getpwuid(a.st_uid))){
        //uid has name
        if((group_name = getgrgid(a.st_gid))){//gid has name
            printf("%s  %ld %s %s %lld %s %s",
            abc, 
            (long) a.st_nlink, 
            &*user_name->pw_name,
            &*group_name->gr_name,
            (long long) a.st_size,
            timeToString(localtime(&a.st_mtime)),
            File_name
            );
        }
        else{
            
            printf("%s  %ld %s %ld %lld %s %s",
            abc, 
            (long) a.st_nlink, 
            &*user_name->pw_name,
            (long) a.st_gid,
            (long long) a.st_size,
            timeToString(localtime(&a.st_mtime)),
            File_name
            );

        }

    }else{
        // uid has no name
        if((group_name = getgrgid(a.st_gid))){//gid has name
            printf("%s  %ld %ld %s %lld %s %s",
            abc, 
            (long) a.st_nlink, 
            (long) a.st_uid,
            &*group_name->gr_name,
            (long long) a.st_size,
            timeToString(localtime(&a.st_mtime)),
            File_name
            );

        }
        else{
            printf("%s  %ld %ld %ld %lld %s %s",
            abc, 
            (long) a.st_nlink, 
            (long) a.st_uid,
            (long) a.st_gid,
            (long long) a.st_size,
            timeToString(localtime(&a.st_mtime)),
            File_name
            );
            
        }

    }
    return flag;
    

}


int reverse(const struct dirent **a, const struct dirent **b){
    return - alphasort(a,b);
}

void recursive(char * pathname, char *bef_path){
     if(  chdir(pathname)== -1){
            if(errno ==EACCES) printf("ls: cannot open directory %s: Permission denied",pathname);
            return;
      }


    DIR *dir_ptr = opendir(".");
    
    if(dir_ptr ==NULL){
        if(errno == EACCES) printf("ls: cannot open directory %s: Permission denied",pathname);
        printf("%s\n",pathname);
            return;
    }
 
    

    // change working directory

       
    struct dirent **namelist;
    struct stat a;
    int count; int idx; 
    if((count = scandir(".", &namelist, NULL, alphasort)) != -1) 
    { 
        char temp[1024];
        temp[0] = '\0';
        strcat(temp,bef_path);
        strcat(temp,pathname);
        if(strcmp(temp,".")!=0) printf("\n%s:\n",temp); 


        
        for(idx= 0; idx<count; idx++){
            if(!strcmp(namelist[idx]-> d_name ,".") || !strcmp(namelist[idx]->d_name , ".."))continue;
            lstat(namelist[idx]->d_name, &a);
    
            if(print_file(a,namelist[idx]->d_name)==1){
                char buf[1024];
    
                int n = readlink(namelist[idx]-> d_name,buf,1024);
                buf[n] ='\0';
                printf(" -> %s\n",buf);
            } else{
                printf("\n");
            }
        }

        strcat(temp,"/");
        struct stat b;
        for(idx= 0;idx<count; idx++ ){
            if(!strcmp( namelist[idx]-> d_name ,".") || !strcmp(namelist[idx]->d_name , ".."))continue;
            lstat(namelist[idx]->d_name,&b);
            if( !S_ISDIR(b.st_mode) ) continue;
         
            recursive(namelist[idx]->d_name,temp);
        }



        for(idx = 0; idx < count; idx++) 
        { free(namelist[idx]); } 
        // namelist에 대한 메모리 해제 
        free(namelist); 
        chdir("..");
        return ; 
    } else{
        if(errno == ENOENT) printf("a\n");
        if(errno == ENOMEM) printf("b\n");
        if(errno == ENOTDIR) printf("c\n");
    }
  //recursiv 
    chdir("..");
    return;
    
}







int main(int argc, char *argv[]){
    
   
   if (argc != 2) {
        
    }else{
      if(  chdir(argv[1])== -1){
            if(errno ==EACCES) printf("ls: cannot open directory %s: Permission denied",argv[1]);
            return 1;
      }
    }
    
    recursive(".","");   

   exit(EXIT_SUCCESS);


}
