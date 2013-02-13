#ifndef PHP_IPCS_H
#define PHP_IPCS_H 1

#define PHP_IPCS_VERSION "1.0"
#define PHP_IPCS_EXTNAME "ipcs"

// Daemons
PHP_FUNCTION(ipcs_daemon);
// Semaphores.
PHP_FUNCTION(ipcs_sem);
PHP_FUNCTION(ipcs_semrm);
PHP_FUNCTION(ipcs_semval);
PHP_FUNCTION(ipcs_wait);
PHP_FUNCTION(ipcs_signal);
// Shmem.
PHP_FUNCTION(ipcs_del);
PHP_FUNCTION(ipcs_get);
PHP_FUNCTION(ipcs_set);
PHP_FUNCTION(ipcs_geti);
PHP_FUNCTION(ipcs_seti);
PHP_FUNCTION(ipcs_inc);
//Cache
PHP_FUNCTION(ipcs_cache_get);
PHP_FUNCTION(ipcs_cache_set);

extern zend_module_entry ipcs_module_entry;
#define phpext_ipcs_ptr &ipcs_module_entry

struct ipcs_fcall
{
  char func[50]; // function name
  char file[30]; // file to stdout
  char params[1000]; // serialized params for function
  int params_length; // serialized params length
};

#endif
