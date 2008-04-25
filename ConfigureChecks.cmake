# gpgme configure checks
include(CheckFunctionExists)

if ( GPGME_FOUND )

set(CMAKE_REQUIRED_INCLUDES ${GPGME_INCLUDES})
set(CMAKE_REQUIRED_LIBRARIES)
foreach( _FLAVOUR VANILLA PTHREAD QT PTH GLIB )
  if ( NOT CMAKE_REQUIRED_LIBRARIES )
    if ( GPGME_${_FLAVOUR}_FOUND )
      set(CMAKE_REQUIRED_LIBRARIES ${GPGME_VANILLA_LIBRARIES})
    endif( GPGME_${_FLAVOUR}_FOUND )
  endif( NOT CMAKE_REQUIRED_LIBRARIES )
endforeach( _FLAVOUR )

# check if gpgme has gpgme_data_{get,set}_file_name (new in 1.1.0)
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_data_t data = 0;
    const char * filename = 0;
    const gpgme_error_t err = gpgme_data_set_file_name( data, filename );
    char * filename2 = gpgme_data_get_file_name( data );
    (void)filename2; (void)err;
    return 0;
  }
" HAVE_GPGME_DATA_SET_FILE_NAME
)

# check if gpgme has GPGME_INCLUDE_CERTS_DEFAULT
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    int i = GPGME_INCLUDE_CERTS_DEFAULT;
    return 0;
  }
" HAVE_GPGME_INCLUDE_CERTS_DEFAULT
)

# check if gpgme has GPGME_KEYLIST_MODE_SIG_NOTATIONS
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_keylist_mode_t mode = GPGME_KEYLIST_MODE_SIG_NOTATIONS;
    return 0;
  }
" HAVE_GPGME_KEYLIST_MODE_SIG_NOTATIONS
)

# check if gpgme_key_sig_t has notations
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_key_sig_t key_sig = 0;
    gpgme_sig_notation_t notation = key_sig->notations;
    return 0;
  }
" HAVE_GPGME_KEY_SIG_NOTATIONS
)

# check if gpgme has gpgme_key_t->is_qualified
check_cxx_source_compiles ("
  #include <gpgme.h>
  void test(gpgme_key_t& key) {
    unsigned int iq;
    iq = key->is_qualified;
  }
  int main() { return 0; }
" HAVE_GPGME_KEY_T_IS_QUALIFIED 
)

# check if gpgme has gpgme_sig_notation_t->critical
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_sig_notation_t sig_notation = 0;
    unsigned int cr1 = sig_notation->critical;
    unsigned int cr2 = GPGME_SIG_NOTATION_CRITICAL;
    return 0;
  }
" HAVE_GPGME_SIG_NOTATION_CRITICAL
)

# check if gpgme has gpgme_sig_notation_t->flags
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_sig_notation_t sig_notation = 0;
    gpgme_sig_notation_flags_t f = sig_notation->flags;
    return 0;
  }
" HAVE_GPGME_SIG_NOTATION_FLAGS_T
)

# check if gpgme has gpgme_sig_notation_t->human_readable
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_sig_notation_t sig_notation = 0;
    unsigned int cr1 = sig_notation->human_readable;
    unsigned int cr2 = GPGME_SIG_NOTATION_HUMAN_READABLE;
    return 0;
  }
" HAVE_GPGME_SIG_NOTATION_HUMAN_READABLE
)

# check if gpgme has gpgme_subkey_t->is_qualified
check_cxx_source_compiles ("
  #include <gpgme.h>
  void test(gpgme_subkey_t& subkey) {
    unsigned int iq;
    iq = subkey->is_qualified;
  }
  int main() { return 0; }
" HAVE_GPGME_SUBKEY_T_IS_QUALIFIED 
)

# check if gpgme has gpgme_engine_info_t->home_dir
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_engine_info_t ei = 0;
    const char * hd = ei->home_dir;
    return 0;
  }
" HAVE_GPGME_ENGINE_INFO_T_HOME_DIR
)

#check if gpgme has gpgme_ctx_{get,set}_engine_info()
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_ctx_t ctx = 0;
    const gpgme_engine_info_t ei = gpgme_ctx_get_engine_info( ctx );
    const char * filename = 0;
    const char * home_dir = 0;
    const gpgme_error_t e
      = gpgme_ctx_set_engine_info( ctx, GPGME_PROTOCOL_OpenPGP, filename, home_dir );
    return 0;
  }
" HAVE_GPGME_CTX_GETSET_ENGINE_INFO
)

# missing, but not needed yet (only for edit interaction)
#+    GPGME_STATUS_SIG_SUBPACKET,
#+    GPGME_STATUS_NEED_PASSPHRASE_PIN,
#+    GPGME_STATUS_SC_OP_FAILURE,
#+    GPGME_STATUS_SC_OP_SUCCESS,
#+    GPGME_STATUS_CARDCTRL,
#+    GPGME_STATUS_BACKUP_KEY_CREATED,
#+    GPGME_STATUS_PKA_TRUST_BAD,
#+    GPGME_STATUS_PKA_TRUST_GOOD,
#+
#+    GPGME_STATUS_PLAINTEXT

# check if gpgme has gpgme_sig_notation_{clear,add,get}
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_ctx_t ctx = 0;
    const gpgme_sig_notation_t nota = gpgme_sig_notation_get( ctx );
    const char * const name = 0;
    const char * const value = 0;
    const gpgme_sig_notation_flags_t flags = 0;
    const gpgme_error_t err = gpgme_sig_notation_add( ctx, name, value, flags );
    gpgme_sig_notation_clear( ctx );
    return 0;
  }
" HAVE_GPGME_SIG_NOTATION_CLEARADDGET
)

# check if gpgme has gpgme_decrypt_result_t->file_name
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_decrypt_result_t res = 0;
    const char * const fn = res->file_name;
    (void)fn;
  }
" HAVE_GPGME_DECRYPT_RESULT_T_FILE_NAME
)

# check if gpgme has gpgme_recipient_t and gpgme_decrypt_result_t->recipients
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_decrypt_result_t res = 0;
    gpgme_recipient_t r = res->recipients;
    const char * kid = r->keyid;
    r = r->next;
    const gpgme_pubkey_algo_t algo = r->pubkey_algo;
    const gpgme_error_t err = r->status;
    return 0;
  }
" HAVE_GPGME_DECRYPT_RESULT_T_RECIPIENTS
)

# check if gpgme has gpgme_verify_result_t->file_name
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_verify_result_t res = 0;
    const char * fn = res->file_name;
    (void)fn;
    return 0;
  }
" HAVE_GPGME_VERIFY_RESULT_T_FILE_NAME
)

# check if gpgme has gpgme_signature_t->pka_{trust,address}
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_signature_t sig = 0;
    unsigned int pkat = sig->pka_trust;
    const char * pkaa = sig->pka_address;
    return 0;
  }
" HAVE_GPGME_SIGNATURE_T_PKA_FIELDS
)

# check if gpgme has gpgme_signature_t->{hash,pubkey}_algo
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_signature_t sig = 0;
    gpgme_pubkey_algo_t pk = sig->pubkey_algo;
    gpgme_hash_algo_t h = sig->hash_algo;
    return 0;
  }
" HAVE_GPGME_SIGNATURE_T_ALGORITHM_FIELDS
)

# check if gpgme has gpgme_signature_t->chain_model
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_signature_t sig = 0;
    unsigned int cm = sig->chain_model;
    return 0;
  }
" HAVE_GPGME_SIGNATURE_T_CHAIN_MODEL
)

# check if gpgme has gpgme_get_fdptr
check_function_exists( "gpgme_get_fdptr" HAVE_GPGME_GET_FDPTR )

# check if gpgme has gpgme_op_getauditlog
check_function_exists ("gpgme_op_getauditlog" HAVE_GPGME_OP_GETAUDITLOG )

# check if gpgme has GPGME_PROTOCOL_GPGCONF
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_protocol_t proto = GPGME_PROTOCOL_GPGCONF;
    return 0;
  }
" HAVE_GPGME_PROTOCOL_GPGCONF
)

# check if gpgme has GPGME_ENCRYPT_NO_ENCRYPT_TO
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_encrypt_flags_t flags = GPGME_ENCRYPT_NO_ENCRYPT_TO;
    return 0;
  }
" HAVE_GPGME_NO_ENCRYPT_TO_FLAG
)

set(CMAKE_REQUIRED_INCLUDES)
set(CMAKE_REQUIRED_LIBRARIES)

endif( GPGME_FOUND )
