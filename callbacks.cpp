/*
  callbacks.cpp - callback targets for internal use:
  Copyright (C) 2003,2004 Klarälvdalens Datakonsult AB

  This file is part of GPGME++.

  GPGME++ is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  GPGME++ is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with GPGME++; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include <gpgme++/config-gpgme++.h>

#include "callbacks.h"
#include "util.h"

#include <gpgme++/interfaces/progressprovider.h>
#include <gpgme++/interfaces/passphraseprovider.h>
#include <gpgme++/interfaces/dataprovider.h>
#include <gpgme++/error.h>

#include <gpgme.h>
#include <gpg-error.h>

#include <cassert>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>

static inline gpgme_error_t make_err_from_syserror() {
#ifdef HAVE_GPGME_GPG_ERROR_WRAPPERS
  return gpgme_error_from_syserror();
#else
  return gpg_error_from_syserror();
#endif
}

using GpgME::ProgressProvider;
using GpgME::PassphraseProvider;
using GpgME::DataProvider;

void progress_callback( void * opaque, const char * what,
			int type, int current, int total ) {
  ProgressProvider * provider = static_cast<ProgressProvider*>( opaque );
  if ( provider ) {
    provider->showProgress( what, type, current, total );
  }
}

static void wipe( char * buf, size_t len ) {
  for ( size_t i = 0 ; i < len ; ++i ) {
    buf[i] = '\0';
  }
}

gpgme_error_t passphrase_callback( void * opaque, const char * uid_hint, const char * desc,
				   int prev_was_bad, int fd ) {
  PassphraseProvider * provider = static_cast<PassphraseProvider*>( opaque );
  bool canceled = false;
  gpgme_error_t err = GPG_ERR_NO_ERROR;
  char * passphrase = provider ? provider->getPassphrase( uid_hint, desc, prev_was_bad, canceled ) : 0 ;
  if ( canceled ) {
    err = make_error( GPG_ERR_CANCELED );
  } else {
    if ( passphrase && *passphrase ) {
      size_t passphrase_length = std::strlen( passphrase );
      size_t written = 0;
      do {
#ifdef HAVE_GPGME_IO_READWRITE
	ssize_t now_written = gpgme_io_write( fd, passphrase + written, passphrase_length - written );
#else
        ssize_t now_written = write( fd, passphrase + written, passphrase_length - written );
#endif
	if ( now_written < 0 ) {
	  err = make_err_from_syserror();
	  break;
	}
	written += now_written;
      } while ( written < passphrase_length );
    }
  }

  if ( passphrase && *passphrase ) {
    wipe( passphrase, std::strlen( passphrase ) );
  }
  free( passphrase );
#ifdef HAVE_GPGME_IO_READWRITE
  gpgme_io_write( fd, "\n", 1 );
#else
  write( fd, "\n", 1 );
#endif
  return err;
}



static ssize_t
data_read_callback( void * opaque, void * buf, size_t buflen ) {
  DataProvider * provider = static_cast<DataProvider*>( opaque );
  if ( !provider ) {
#ifdef HAVE_GPGME_GPG_ERROR_WRAPPERS
    gpgme_err_set_errno( gpgme_err_code_to_errno( GPG_ERR_EINVAL ) );
#else
    gpg_err_set_errno( gpgme_err_code_to_errno( GPG_ERR_EINVAL ) );
#endif
    return -1;
  }
  return provider->read( buf, buflen );
}

static ssize_t
data_write_callback( void * opaque, const void * buf, size_t buflen ) {
  DataProvider * provider = static_cast<DataProvider*>( opaque );
  if ( !provider ) {
#ifdef HAVE_GPGME_GPG_ERROR_WRAPPERS
    gpgme_err_set_errno( gpgme_err_code_to_errno( GPG_ERR_EINVAL ) );
#else
    gpg_err_set_errno( gpgme_err_code_to_errno( GPG_ERR_EINVAL ) );
#endif
    return -1;
  }
  return provider->write( buf, buflen );
}

static off_t
data_seek_callback( void * opaque, off_t offset, int whence ) {
  DataProvider * provider = static_cast<DataProvider*>( opaque );
  if ( !provider ) {
#ifdef HAVE_GPGME_GPG_ERROR_WRAPPERS
    gpgme_err_set_errno( gpgme_err_code_to_errno( GPG_ERR_EINVAL ) );
#else
    gpg_err_set_errno( gpgme_err_code_to_errno( GPG_ERR_EINVAL ) );
#endif
    return -1;
  }
  if ( whence != SEEK_SET && whence != SEEK_CUR && whence != SEEK_END ) {
#ifdef HAVE_GPGME_GPG_ERROR_WRAPPERS
    gpgme_err_set_errno( gpgme_err_code_to_errno( GPG_ERR_EINVAL ) );
#else
    gpg_err_set_errno( gpgme_err_code_to_errno( GPG_ERR_EINVAL ) );
#endif
    return -1;
  }
  return provider->seek( offset, whence );
}

static void data_release_callback( void * opaque ) {
  DataProvider * provider = static_cast<DataProvider*>( opaque );
  if ( provider ) {
    provider->release();
  }
}

gpgme_data_cbs GpgME::data_provider_callbacks = {
  &data_read_callback,
  &data_write_callback,
  &data_seek_callback,
  &data_release_callback
};

