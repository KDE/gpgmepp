#ifndef __GPGMEPP_CONFIG_GPGMEPP_H__
#define __GPGMEPP_CONFIG_GPGMEPP_H__

/* Define to 1 if your gpgme has the gpgme_data_{set,get}_file_name functions */
#cmakedefine HAVE_GPGME_DATA_SET_FILE_NAME 1

/* Define to 1 if your gpgme has the GPGME_INCLUDE_CERTS_DEFAULT macro */
#cmakedefine HAVE_GPGME_INCLUDE_CERTS_DEFAULT 1

/* Define to 1 if your gpgme has the GPGME_KEYLIST_MODE_SIG_NOTATIONS macro */
#cmakedefine HAVE_GPGME_KEYLIST_MODE_SIG_NOTATIONS

/* Define to 1 if your gpgme's gpgme_key_sig_t has the notations member */
#cmakedefine HAVE_GPGME_KEY_SIG_NOTATIONS

/* Define to 1 if your gpgme's gpgme_key_t has the is_qualified flag */
#cmakedefine HAVE_GPGME_KEY_T_IS_QUALIFIED 1

/* Define to 1 if your gpgme's gpgme_sig_notation_t has the critical flag */
#cmakedefine HAVE_GPGME_SIG_NOTATION_CRITICAL

/* Define to 1 if your gpgme's gpgme_sig_notation_t has the flags member */
#cmakedefine HAVE_GPGME_SIG_NOTATION_FLAGS_T

/* Define to 1 if your gpgme's gpgme_sig_notation_t has the human_readable flag */
#cmakedefine HAVE_GPGME_SIG_NOTATION_HUMAN_READABLE

/* Define to 1 if your gpgme's gpgme_subkey_t has the is_qualified flag */
#cmakedefine HAVE_GPGME_SUBKEY_T_IS_QUALIFIED 1

/* Define to 1 if your gpgme's gpgme_engine_info_t has the home_dir member */
#cmakedefine HAVE_GPGME_ENGINE_INFO_T_HOME_DIR 1

/* Define to 1 if your gpgme has the gpgme_ctx_{get,set}_engine_info() functions */
#cmakedefine HAVE_GPGME_CTX_GETSET_ENGINE_INFO 1

/* Define to 1 if your gpgme has the gpgme_sig_notation_{clear,add,get}() functions */
#cmakedefine HAVE_GPGME_SIG_NOTATION_CLEARADDGET 1

/* Define to 1 if your gpgme's gpgme_decrypt_result_t has the file_name member */
#cmakedefine HAVE_GPGME_DECRYPT_RESULT_T_FILE_NAME 1

/* Define to 1 if your gpgme's gpgme_decrypt_result_t has the recipients member */
#cmakedefine HAVE_GPGME_DECRYPT_RESULT_T_RECIPIENTS 1

/* Define to 1 if your gpgme's gpgme_verify_result_t has the file_name member */
#cmakedefine HAVE_GPGME_VERIFY_RESULT_T_FILE_NAME 1

/* Define to 1 if your gpgme's gpgme_signature_t has the pka_{trust,address} fields */
#cmakedefine HAVE_GPGME_SIGNATURE_T_PKA_FIELDS 1

/* Define to 1 if your gpgme's gpgme_signature_t has the {hash,pubkey}_algo fields */
#cmakedefine HAVE_GPGME_SIGNATURE_T_ALGORITHM_FIELDS 1

/* Define to 1 if your gpgme has gpgme_get_fdptr (it won't be in gpgme.h, though!) */
#cmakedefine HAVE_GPGME_GET_FDPTR 1

/* Define to 1 if your gpgme has the gpgme_op_getauditlog function */
#cmakedefine HAVE_GPGME_OP_GETAUDITLOG 1

/* Define to 1 if your gpgme has GPGME_PROTOCOL_GPGCONF */
#cmakedefine HAVE_GPGME_PROTOCOL_GPGCONF 1

/* Define to 1 if your gpgme has gpgme_cancel_async */
#cmakedefine HAVE_GPGME_CANCEL_ASYNC 1

/* Define to 1 if your gpg-error has GPG_ERR_NO_PASSPHRASE */
#cmakedefine HAVE_GPG_ERR_NO_PASSPHRASE 1

/* Define to 1 if your gpg-error has GPG_ERR_ALREADY_SIGNED */
#cmakedefine HAVE_GPG_ERR_ALREADY_SIGNED 1

#ifndef HAVE_GPG_ERR_NO_PASSPHRASE
# define GPG_ERR_NO_PASSPHRASE GPG_ERR_INV_PASSPHRASE
#endif

#ifndef HAVE_GPG_ERR_ALREADY_SIGNED
# define GPG_ERR_ALREADY_SIGNED GPG_ERR_USER_1
#endif

/* Define to 1 if your gpgme has GPGME_ENCRYPT_NO_ENCRYPT_TO */
#cmakedefine HAVE_GPGME_ENCRYPT_NO_ENCRYPT_TO 1

/* Define to 1 if your gpgme has gpgme_subkey_t->card_number and gpgme_subkey_t->is_cardkey */
#cmakedefine HAVE_GPGME_SUBKEY_T_IS_CARDKEY

/* Define to 1 if your gpgme has ASSUAN protocol support (in the interface, not only in the implementation */
#cmakedefine HAVE_GPGME_ASSUAN_ENGINE

/* Define to 1 if your gpgme has GPGME_KEYLIST_MODE_EPHEMERAL */
#cmakedefine HAVE_GPGME_KEYLIST_MODE_EPHEMERAL

/* Define to 1 if your gpgme has gpgme_op_import_keys */
#cmakedefine HAVE_GPGME_OP_IMPORT_KEYS

/* Define to 1 if your gpgme has support for G13 VFS operations */
#cmakedefine HAVE_GPGME_G13_VFS

/* Define to 1 if your gpgme has gpgme_op_passwd */
#cmakedefine HAVE_GPGME_OP_PASSWD

/* Define to 1 if your gpgme has gpgme_io_read and gpgme_io_write */
#cmakedefine HAVE_GPGME_IO_READWRITE 1

/* Define to 1 if your gpg-error has gpg_err_set_errno */
#cmakedefine HAVE_GPG_ERR_SET_ERRNO 1

/* Define to 1 if your gpgme has gpgme_err_set_errno(int) and friends */
#cmakedefine HAVE_GPGME_GPG_ERROR_WRAPPERS 1

/* Define to 1 if your gpgme defines gpgme_ssize_t */
#cmakedefine HAVE_GPGME_SSIZE_T 1

/* Define to 1 if your gpgme defines gpgme_off_t */
#cmakedefine HAVE_GPGME_OFF_T 1

#ifndef HAVE_GPGME_GPG_ERROR_WRAPPERS
# ifndef HAVE_GPG_ERR_SET_ERRNO
#  include <errno.h>
static inline void gpg_err_set_errno( int x ) { errno = x; }
# endif
#endif

/* Define to 1 if your gpgme_conf_arg_new takes its 'value' parameter as const void* */
#cmakedefine HAVE_GPGME_CONF_ARG_NEW_WITH_CONST_VALUE 1

#ifndef GPGMEPP_ERR_SOURCE_DEFAULT
#define GPGMEPP_ERR_SOURCE_DEFAULT 22
#endif
#ifndef   GPGME_ERR_SOURCE_DEFAULT
#define   GPGME_ERR_SOURCE_DEFAULT ((gpgme_err_source_t)22)
#endif
#ifndef     GPG_ERR_SOURCE_DEFAULT
#define     GPG_ERR_SOURCE_DEFAULT ((gpg_err_source_t)22)
#endif

#endif /* __GPGMEPP_CONFIG_GPGMEPP_H__ */
