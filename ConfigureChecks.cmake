# gpgme configure checks

set(CMAKE_REQUIRED_INCLUDES ${GPGME_INCLUDES})
set(CMAKE_REQUIRED_LIBRARIES ${GPGME_LIBRARIES})

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
  }
" HAVE_GPGME_INCLUDE_CERTS_DEFAULT
)

# check if gpgme has GPGME_KEYLIST_MODE_SIG_NOTATIONS
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_keylist_mode_t mode = GPGME_KEYLIST_MODE_SIG_NOTATIONS;
  }
" HAVE_GPGME_KEYLIST_MODE_SIG_NOTATIONS
)

# check if gpgme_key_sig_t has notations
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_key_sig_t key_sig = 0;
    gpgme_sig_notation_t notation = key_sig->notations;
  }
" HAVE_GPGME_KEY_SIG_NOTATIONS
)

# check if gpgme has gpgme_key_t->is_qualified
check_cxx_source_compiles ("
  #include <gpgme.h>
  int test(gpgme_key_t& key) {
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
  }
" HAVE_GPGME_SIG_NOTATION_CRITICAL
)

# check if gpgme has gpgme_sig_notation_t->flags
check_cxx_source_compiles ("
  #include <gpgme.h>
  int main() {
    gpgme_sig_notation_t sig_notation = 0;
    gpgme_sig_notation_flags_t f = sig_notation->flags;
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
  }
" HAVE_GPGME_SIG_NOTATION_HUMAN_READABLE
)

# check if gpgme has gpgme_subkey_t->is_qualified
check_cxx_source_compiles ("
  #include <gpgme.h>
  int test(gpgme_subkey_t& subkey) {
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
  }
" HAVE_GPGME_CTX_GETSET_ENGINE_INFO
)

set(CMAKE_REQUIRED_INCLUDES)
set(CMAKE_REQUIRED_LIBRARIES)

