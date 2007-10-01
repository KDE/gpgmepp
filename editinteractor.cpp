/*
  editinteractor.cpp - Interface for edit interactors
  Copyright (C) 2007 Klarälvdalens Datakonsult AB

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

#include "editinteractor.h"
#include "callbacks.h"
#include "error.h"

#include <gpg-error.h>

#ifdef _WIN32
# include <io.h>
# define write _write
#else
# include <unistd.h>
#endif

#include <cstring>

using namespace GpgME;

static gpgme_error_t edit_interactor_callback( void * opaque, gpgme_status_code_t status, const char * args, int fd );

class EditInteractor::Private {
    friend class ::GpgME::EditInteractor;
    EditInteractor * const q;
public:
    explicit Private( EditInteractor * qq );
    ~Private();

    friend gpgme_error_t ::edit_interactor_callback( void * opaque, gpgme_status_code_t status, const char * args, int fd ) {
        Private * ei = (Private*)opaque;

        try {
            // advance to next state based on input:
            ei->state = ei->q->nextState( status, args );

            // if there was an error from before, we stop here (### this looks weird, can this happen at all?)
            if ( gpg_err_code( ei->error ) != GPG_ERR_NO_ERROR )
                return ei->error;

            // if there's a result, write it:
            if ( const char * const result = ei->q->action() ) {
                if ( *result )
                    write( fd, result, std::strlen( result ) );
                write( fd, "\n", 1 );
            }

            return 0;

        } catch ( const Error & err ) {
            ei->error = err;
            ei->state = EditInteractor::ErrorState;
            return err;
        }

        return 0;
    }

private:
    unsigned int state;
    Error error;
};

gpgme_edit_cb_t GpgME::edit_interactor_callback = ::edit_interactor_callback;

EditInteractor::Private::Private( EditInteractor * qq )
    : q( qq ),
      state( StartState ),
      error()
{

}

EditInteractor::Private::~Private() {}

EditInteractor::EditInteractor()
    : d( new Private( this ) )
{

}

EditInteractor::~EditInteractor() {
    delete d; d = 0;
}

unsigned int EditInteractor::state() const {
    return d->state;
}

Error EditInteractor::lastError() const {
    return d->error;
}

