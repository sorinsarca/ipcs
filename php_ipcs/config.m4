PHP_ARG_ENABLE(ipcs, whether to enable ipcs support, [ --enable-ipcs   Enable ipcs support])
if test "$PHP_IPCS" != "no"; then
  AC_DEFINE(HAVE_IPCS, 1, [Whether you have ipcs])
  PHP_NEW_EXTENSION(ipcs, php_ipcs.c, $ext_shared)
fi