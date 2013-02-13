#include <stdio.h>
//#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>

#define S_SHM_PERM 0777


static const char  table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int isbase64(char c) {
   return c && strchr(table, c) != NULL;
}

inline char value(char c) {
   const char *p = strchr(table, c);
   if (p) {
      return p - table;
   }
   else {
      return 0;
   }
}

int decode_base64(unsigned char *dest, const unsigned char *src, int srclen) {
   *dest = 0;
   if(*src == 0) {
      return 0;
   }
   unsigned char *p = dest;
   do {
        char a = value(src[0]);
        char b = value(src[1]);
        char c = value(src[2]);
        char d = value(src[3]);
        *p++ = (a << 2) | (b >> 4);
        *p++ = (b << 4) | (c >> 2);
        *p++ = (c << 6) | d;
        if (!isbase64(src[1])) {
           p -= 2;
           break;
        } 
        else if (!isbase64(src[2])) {
           p -= 2;
           break;
        } 
        else if (!isbase64(src[3])) {
           p--;
           break;
        }
        src += 4;
        while (*src && (*src == 13 || *src == 10)) src++;
   } while(srclen -= 4);
   *p = 0;
   return p - dest;
}

// ipcs_fcall struct
struct ipcs_fcall {
  char func[50]; // function name
  char file[30]; // file to stdout
  char params[1000]; // serialized params for function.
  int params_length; // serialized params length
};

/**
 * Returns 0 if cache miss, 1 if delivered.
 */
int checkcache(char *key, char *file) {
   struct stat st;
   int fd = shm_open(key, O_RDWR, S_SHM_PERM);
   if (fd < 0 || fstat(fd, &st) == -1)
   {
      close(fd);
      return 0;
   }
   // we might have some cache
   char *data = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
   close(fd);
   if (data != NULL) {
      if (st.st_mtime - time(NULL) < 0) {
         // cache expired, delete it
         munmap(data, st.st_size);
         shm_unlink(key);
      }
      // use cache
      else if ((fd = open(file, O_WRONLY)) != -1) {
        write(fd, data, st.st_size);
        close(fd);
        return 1;
      }
   }
   return 0;
}

/*
 * 1 - cache
 * 2 - prefix
 * 3 - func
 * [4 - params]
 */
    
int main(int argc, char *argv[]) {
    sem_t *sema, *semd, *semc;
    int fd, len;
    unsigned long long total, current, *val;
    struct ipcs_fcall *addr, p;
    char keya[20], keyd[20], keyc[20], dt[15], dc[15];
    // set file name (this is parent process stdout file descriptor)
    // usually it is /proc/{pid}/fd/1
    sprintf(p.file, "/proc/%d/fd/1", getppid());
    
    // check for cache
    if (argc > 1 && *argv[1] != '0') {
      if (checkcache(argv[1], p.file) || argc == 2) {
         return 0;
      }
    }
   
    switch (argc) {
        case 4: // we don't have params, skip decoding
            p.params_length = 0;
            break;
        case 5: // we have params so decode them
            // set params, parames are base64_encoded so first must decode them
            decode_base64(p.params, argv[4], strlen(argv[4]));
            // set params length
            p.params_length = strlen(p.params);
            break;
        default: // wrong number of parameters
            return 1;
            break;
    }
    // set function name
    strcpy(p.func, argv[3]);

    // get names for total number fo deamons and current daemon    
    sprintf(dt, "%s_t", argv[2]); // total daemons
    sprintf(dc, "%s_c", argv[2]); // current daemon
    
    // get total.
    fd = shm_open(dt, O_RDWR, S_SHM_PERM);
    val = mmap(NULL, sizeof(long long), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    total = *val;
    close(fd);
    
    // get current
    fd = shm_open(dc, O_RDWR, S_SHM_PERM);
    val = mmap(NULL, sizeof(long long), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    // set next
    *val += 1;
    current = (*val) % total;
    
    // get keys
    sprintf(keya, "%s_a_%llu", argv[2], current); // available
    sprintf(keyd, "%s_d_%llu", argv[2], current); // daemon
    sprintf(keyc, "%s_c_%llu", argv[2], current); // client
    
    // semaphore for daemon availability
    sema = sem_open(keya, 0, S_SHM_PERM, 1);
    // semaphore for client availability
    semd = sem_open(keyd, 0, S_SHM_PERM, 0);
    // client semaphore which waits until daemon job is done
    semc = sem_open(keyc, 0, S_SHM_PERM, 0);
    // open shared memory
    fd = shm_open(keyd, O_RDWR, S_SHM_PERM);
    
    // check all semaphores
    if (sema == SEM_FAILED || semd == SEM_FAILED || semc == SEM_FAILED || fd < 0) {
        // cannot continue if something is not good
        return 1;
    }

    // wait for daemon to be available
    sem_wait(sema);
    
    // get mem address
    addr = mmap(NULL, sizeof(struct ipcs_fcall), PROT_WRITE, MAP_SHARED, fd, 0);
    // save struct to memory
    *addr = p;
    // close file descriptor
    close(fd);

    // say to daemon that data is available
    sem_post(semd);
    
    // wait for daemon to finish job
    sem_wait(semc);

    // close semaphores
    sem_close(sema); sem_close(semd); sem_close(semc);
    
    // all good
    return 0;
}

