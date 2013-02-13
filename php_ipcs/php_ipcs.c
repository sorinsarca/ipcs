#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ipcs.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <time.h>
#include "ext/standard/php_var.h"


static function_entry ipcs_functions[] = {  
    // Daemon
    PHP_FE(ipcs_daemon, NULL)
    // For all.
    PHP_FE(ipcs_del, NULL)
    // For strings.
    PHP_FE(ipcs_get, NULL)
    PHP_FE(ipcs_set, NULL)
    // For integers.
    PHP_FE(ipcs_geti, NULL)
    PHP_FE(ipcs_seti, NULL)
    PHP_FE(ipcs_inc, NULL)
    // Semaphores.
    PHP_FE(ipcs_sem, NULL)
    PHP_FE(ipcs_semrm, NULL)
    PHP_FE(ipcs_semval, NULL)
    PHP_FE(ipcs_wait, NULL)
    PHP_FE(ipcs_signal, NULL)
    // Cache
    PHP_FE(ipcs_cache_get, NULL)
    PHP_FE(ipcs_cache_set, NULL)
    PHP_FALIAS(ipcs_cache_del, ipcs_del, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry ipcs_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_IPCS_EXTNAME,
    ipcs_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_IPCS_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_IPCS
ZEND_GET_MODULE(ipcs)
#endif

#define S_SHM_PERM S_IROTH | S_IWOTH | S_IRWXU | S_IRWXG


PHP_FUNCTION(ipcs_sem)
{
    char *key;
    int key_len; int val;
    zend_bool create = 0;
    sem_t *sem;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|b", &key, &key_len, &val, &create) == FAILURE) {
        RETURN_NULL();
    }
    
    sem = create ? sem_open(key, O_CREAT | O_EXCL, S_SHM_PERM, val) : sem_open(key, 0, S_SHM_PERM, val);
    if(sem < 0) RETURN_FALSE;
    sem_close(sem);
    RETURN_TRUE;
}

PHP_FUNCTION(ipcs_semrm)
{
    char *key;
    int key_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE) {
        RETURN_NULL();
    }
    if (sem_unlink(key) < 0) RETURN_FALSE;
    RETURN_TRUE;
}

PHP_FUNCTION(ipcs_semval)
{
    char *key;
    int key_len; int val;
    sem_t *sem;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE) {RETURN_NULL();}
    sem = sem_open(key, 0, S_SHM_PERM, 1);
    if(sem < 0 || sem_getvalue(sem, &val) < 0) RETURN_FALSE;
    RETURN_LONG(val);
}

PHP_FUNCTION(ipcs_wait)
{
    char *key;
    int key_len; int val;
    sem_t *sem;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE){RETURN_NULL();}
    if (key_len < 1) RETURN_FALSE;
    sem = sem_open(key, 0, S_SHM_PERM, 1);
    val = sem_wait(sem);
    if(val < 0 || sem_getvalue(sem, &val) < 0) RETURN_FALSE;
    RETURN_LONG(val);
}

PHP_FUNCTION(ipcs_signal)
{
    char *key;
    int key_len; int val;
    sem_t *sem;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE){RETURN_NULL();}
    sem = sem_open(key, 0, S_SHM_PERM, 0);
    val = sem_post(sem);
    if(val < 0) RETURN_FALSE;
    RETURN_TRUE;
}

PHP_FUNCTION(ipcs_del)
{
    char *key, *addr;
    int key_len; int fd;
    struct stat st;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE){RETURN_NULL();}
    
    fd = shm_open(key, O_RDWR, S_SHM_PERM);
    if (fd >= 0 && fstat(fd, &st) != -1)
    {
        addr = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        close(fd);
        munmap(addr, st.st_size);
    }
    else
    {
        close(fd);
    }
    if (shm_unlink(key) == 0) RETURN_TRUE;
    RETURN_FALSE;
}

PHP_FUNCTION(ipcs_set)
{    
    char *key, *val, *addr;
    int key_len; int fd;
    unsigned long long val_len;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &key, &key_len, &val, &val_len) == FAILURE) {RETURN_NULL();}
    if (val_len < 1) RETURN_FALSE;
    fd = shm_open(key, O_RDWR | O_CREAT, S_SHM_PERM);
    if (fd < 0 || ftruncate(fd, val_len) == -1)
    {
        close(fd);
        RETURN_FALSE;
    }
    addr = mmap(NULL, val_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (addr == NULL) RETURN_FALSE;
    strncpy(addr, val, val_len);
    RETURN_TRUE;
}

PHP_FUNCTION(ipcs_get)
{   
    char *key, *addr;
    int key_len; int fd;
    struct stat st;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE){RETURN_NULL();}
    fd = shm_open(key, O_RDWR, S_SHM_PERM);
    if (fd < 0 || fstat(fd, &st) == -1)
    {
        close(fd);
        RETURN_FALSE;
    }
    addr = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (addr == NULL){RETURN_FALSE;}
    RETURN_STRING(addr, 1);
}

PHP_FUNCTION(ipcs_seti)
{
    char *key;
    int key_len; int fd;
    long long val, *addr;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &key, &key_len, &val) == FAILURE){RETURN_NULL();}
    fd = shm_open(key, O_RDWR | O_CREAT, S_SHM_PERM);
    if (fd < 0 || ftruncate(fd, 8) == -1)
    {
        close(fd);
        RETURN_FALSE;
    }
    addr = mmap(NULL, sizeof(long long), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (addr == NULL){RETURN_FALSE;}
    *addr = val;
    RETURN_TRUE;
}

PHP_FUNCTION(ipcs_geti)
{
    char *key;
    int key_len; int fd;
    long long *addr;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE) {RETURN_NULL();}
    fd = shm_open(key, O_RDWR, S_SHM_PERM);
    if (fd < 0){RETURN_FALSE;}
    addr = mmap(NULL, sizeof(long long), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (addr == NULL){RETURN_FALSE;}
    RETURN_LONG(*addr);
}

PHP_FUNCTION(ipcs_inc)
{
    char *key;
    int key_len; int fd; int val = 1;
    long long *addr, l = 0, r = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lll", &key, &key_len, &val, &l, &r) == FAILURE) {RETURN_NULL();}
    fd = shm_open(key, O_RDWR, S_SHM_PERM);
    if (fd < 0){RETURN_FALSE;}
    addr = mmap(NULL, sizeof(long long), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (addr == NULL){RETURN_FALSE;}
    if (l != r && *addr + val == l) {*addr = r;}else{*addr += val;}
    RETURN_LONG(*addr);
}

PHP_FUNCTION(ipcs_cache_set)
{
   
    time_t t;
    struct tm *tmp;
    char tformat[16];
    char *key, *data, *addr, *file;
    int fd; int len; int keylen; int expire;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssl", &key, &keylen, &data, &len, &expire) == FAILURE) RETURN_NULL();

    keylen = strlen(key);// what???
    if (keylen == 0 || len == 0) RETURN_FALSE;
    
    fd = shm_open(key, O_RDWR | O_CREAT, S_SHM_PERM);
    
    if (fd < 0 || ftruncate(fd, len) == -1)
    {
        close(fd);
        RETURN_FALSE;
    }
    
    addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    
    if (addr == NULL) RETURN_FALSE;
    strncpy(addr, data, len);
    
    t = time(NULL) + expire;
    tmp = localtime(&t);
    //4 + 2 + 2 + 2 + 2 + 1 + 2 = 15
    if (tmp == NULL || strftime(tformat, sizeof(tformat), "%Y%m%d%H%M.%S", tmp) == 0)
        RETURN_FALSE;
    // 5 + 1 + 1 + 9 + 1 + key_len +  1 + 3 + sizeof(tformat) = 21 + key_len + sizeof(outstr) + 1 just in case
    file = malloc(22 + keylen + sizeof(tformat));
    sprintf(file, "touch \"/dev/shm/%s\" -t %s", key, tformat);
    system(file);
    RETURN_TRUE;
}

PHP_FUNCTION(ipcs_cache_get)
{
    char *key, *addr;
    int fd; int len;
    struct stat st;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &len) == FAILURE) RETURN_NULL();
 
    if (len == 0) RETURN_FALSE;
    fd = shm_open(key, O_RDWR, S_SHM_PERM);

    if (fd < 0 || fstat(fd, &st) == -1)
    {
        close(fd);
        RETURN_FALSE;
    }
    addr = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    
    if (st.st_mtime - time(NULL) < 0) {
        munmap(addr, st.st_size);
        shm_unlink(key);
        RETURN_FALSE;
    }
    
    if (addr == NULL) RETURN_NULL();
    RETURN_STRING(addr, 1);
}


PHP_FUNCTION(ipcs_daemon)
{
    // sem & shm names
    char keya[20], keyd[20], keyc[20], *name;
    // semaphores
    sem_t *sema, *semd, *semc;
    // file descriptor and some aux ints
    int fd; int len; int id; int i;
    // used zvals
    zval *ret, *arr, ***params, **data, *func;
    // shm struct
    struct ipcs_fcall *addr;
    // unserialize vars
    const unsigned char * keyu;
    php_unserialize_data_t var_hash;
    HashTable *arr_hash;
    HashPosition pointer;
    
    // daemon prefix, daemon id
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &name, &len, &id) == FAILURE) RETURN_NULL();
    
    // prefix must be max 10 chars and id positive
    if (len < 1 || len > 10 || id < 0) RETURN_FALSE;
    
    // create keys
    sprintf(keya, "%s_a_%d", name, id);
    sprintf(keyd, "%s_d_%d", name, id);
    sprintf(keyc, "%s_c_%d", name, id);

    // delete semaphores
    sem_unlink(keya); sem_unlink(keyd); sem_unlink(keyc);
    
    // semaphore for daemon availability
    sema = sem_open(keya, O_CREAT | O_EXCL, S_SHM_PERM, 1);
    // semaphore for client availability
    semd = sem_open(keyd, O_CREAT | O_EXCL, S_SHM_PERM, 0);
    // semaphore for client availability
    semc = sem_open(keyc, O_CREAT | O_EXCL, S_SHM_PERM, 0);
    
    // open shared memory
    shm_unlink(keyd);
    fd = shm_open(keyd, O_RDWR | O_CREAT, S_SHM_PERM);

    // check if all are opened ok
    if (sema == SEM_FAILED || semd == SEM_FAILED || fd < 0 || ftruncate(fd, sizeof(struct ipcs_fcall)) == -1) RETURN_FALSE;

    // delete prefix
    efree(name);
    
    // create function name as *zval
    MAKE_STD_ZVAL(func);
    
    // process requests
    wait_request:
    
        // wait client request
        sem_wait(semd);
        
        // set client data from shm
        addr = mmap(NULL, sizeof(struct ipcs_fcall), PROT_READ, MAP_SHARED, fd, 0);
        // set function name
        ZVAL_STRING(func, addr->func, 1);
        
        if (addr->params_length == 0) {
            // no params
            params = NULL;
            len = 0;
        }
        else {
            // unserialize params
            arr = (zval *) ecalloc(sizeof(zval), 1);
            keyu = (const unsigned char *) addr->params;
            
            PHP_VAR_UNSERIALIZE_INIT(var_hash);
            php_var_unserialize(&arr, &keyu, keyu + addr->params_length, &var_hash TSRMLS_CC);
            PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
            
            // get array of params
            arr_hash = Z_ARRVAL_P(arr);
            
            // get number of params
            len = zend_hash_num_elements(arr_hash);
            
            if (len == 0) {
                // no params
                params = NULL;
            }
            else {
                // have params, alloc space
                params = (zval ***) emalloc(sizeof(zval **) * len);
                // reset arr pointer
                zend_hash_internal_pointer_reset_ex(arr_hash, &pointer);
                i = 0;
                while(i < len) {
                    // get value
                    if (zend_hash_get_current_data_ex(arr_hash, (void **) &data, &pointer) == SUCCESS) {
                        params[i] = data;
                    }
                    // move pointer forward
                    zend_hash_move_forward_ex(arr_hash, &pointer);
                    i++;
                }
            }
        }
        // call user function
        if (call_user_function_ex(EG(function_table), NULL, func, &ret, len, params, 1, NULL TSRMLS_CC) == SUCCESS && ret != NULL) {            
            
            // convert to string if needed
            if (Z_TYPE_P(ret) != IS_STRING) {
                convert_to_string(ret);
            }
            
            // write to client stdout
            if ((id = open(addr->file, O_WRONLY)) != -1) {
                write(id, Z_STRVAL_P(ret), Z_STRLEN_P(ret));
                close(id);
            }
            else {
                //php_printf("%s", strerror(errno));
            }
            
            // free ret
            zval_ptr_dtor(&ret);
        }
        
        // remove data from shared memory
        munmap(addr, sizeof(struct ipcs_fcall));
        
        // signal client that all is done
        sem_post(semc);
        
        //set daemon as available again
        sem_post(sema);
        
        // free mem
        if (len > 0) {
            if (arr) efree(arr);
            if (arr_hash) efree(arr_hash);
            if (params) efree(params);
        }
        
        // next request
        goto wait_request;
}
