#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <assert.h>

#define PWD_LEN 42  /* 40 byte SHA1 plus \r \n */



#ifndef NDEBUG
#define debug_printf(...) fprintf (stderr, __VA_ARGS__); fflush(stderr)
#else
#define debug_printf(...) 
#endif

#define EXIT_ERROR 2
#define EXIT_FOUND 0
#define EXIT_NOTFOUND 1


int main(const int argc, const char ** const argv){
  int fd = 0;
  struct stat st;
  if (argc < 3){
    fprintf(stderr,"Usage: %s passwordfile password\n",argv[0]);
    exit(EXIT_ERROR);
  }

  const char * const password = argv[2];
  const char * const inputfile = argv[1];

  fd = open(inputfile,O_RDONLY);
  if (fd < 0){
    fprintf(stderr,"Error opening %s\n",inputfile);
    exit(EXIT_ERROR);
  }

  if (fstat(fd,&st) != 0){
    fprintf(stderr,"Error stat'ing file %s\n",inputfile);
    exit(EXIT_ERROR);
  }

  debug_printf("Using collate rules of %s locale\n",
         setlocale(LC_COLLATE, "en_AU.UTF-8"));

  off_t low = 0;
  if (st.st_size % PWD_LEN != 0){
    fprintf(stderr,"file length not a multiple of %d\n",PWD_LEN);
    exit(EXIT_ERROR);
  }

  off_t high = st.st_size / PWD_LEN;

  char buf[PWD_LEN];
    
  /* low < high implies password between low and high - 1 if it exists */
  while(low < high){
    off_t mid = low + ((high - low) / 2);

    debug_printf("low: %lld, mid: %lld, high: %lld\n",low,mid,high);
    if (lseek(fd,mid*PWD_LEN,SEEK_SET) < 0){
      fprintf(stderr,"Error seek'ing\n");
      exit(EXIT_ERROR);
    }

    const ssize_t len = read(fd,buf,PWD_LEN);
    if (len < 0){
      fprintf(stderr,"Error reading\n");
      exit(EXIT_ERROR);
    }
    if (len == 0){
      printf("Password not found (zero read)\n");
      exit(EXIT_NOTFOUND);
    }
    if (len < PWD_LEN){
      fprintf(stderr,"Short read < PWD_LEN\n");
      exit(EXIT_ERROR);
    }
    ssize_t pos = 0;

    char * const str = buf;
    /* null-terminate the string  */
    assert(buf[PWD_LEN-2] == '\r');
    buf[PWD_LEN-2] = '\0';
    int res = strcasecmp(password,str);
    debug_printf("Compared %s with %s, result %d\n",password,str,res);      
    if (res < 0){
      high = mid;
    }else if(res > 0){
      low = mid+pos+1;
    }else{
      /* found it */
      printf("Password %s found in %s.\n",password,inputfile);
      exit(EXIT_FOUND);
    }
  }
  printf("Password %s not found!\n",password);
  exit(EXIT_NOTFOUND);
}
  
