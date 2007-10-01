/*
  context_p.h - wraps a gpgme context (private part)
  Copyright (C) 2003, 2007 Klarälvdalens Datakonsult AB

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

// -*- c++ -*-
#ifndef __GPGMEPP_CONTEXT_P_H__
#define __GPGMEPP_CONTEXT_P_H__

#include <gpgme++/context.h>

#include <gpgme.h>

namespace GpgME {


  class Context::Private {
  public:
    enum Operation {
      None = 0,

      Encrypt   = 0x001,
      Decrypt   = 0x002,
      Sign      = 0x004,
      Verify    = 0x008,
      DecryptAndVerify = Decrypt|Verify,
      SignAndEncrypt   = Sign|Encrypt,

      Import    = 0x010,
      Export    = 0x020, // no gpgme_export_result_t, but nevertheless...
      Delete    = 0x040, // no gpgme_delete_result_t, but nevertheless...

      KeyGen    = 0x080,
      KeyList   = 0x100,
      TrustList = 0x200, // no gpgme_trustlist_result_t, but nevertheless...

      Edit      = 0x400, // no gpgme_edit_result_t, but nevertheless...
      CardEdit  = 0x800, // no gpgme_card_edit_result_t, but nevertheless...
    };

    Private( gpgme_ctx_t c=0 );
    ~Private();


    gpgme_ctx_t ctx;
    gpgme_io_cbs * iocbs;
    Operation lastop;
    gpgme_error_t lasterr;
    std::auto_ptr<EditInteractor> lastEditInteractor, lastCardEditInteractor;
  };

} // namespace GpgME

#endif // __GPGMEPP_CONTEXT_P_H__
