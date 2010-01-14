/*
  global.h - global gpgme functions and enums
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
#ifndef __GPGMEPP_GLOBAL_H__
#define __GPGMEPP_GLOBAL_H__

#include <gpgme++/gpgmefw.h>
#include <gpgme++/gpgme++_export.h>

#include <iosfwd>

namespace GpgME {
    class Error;
    class EngineInfo;
    class Context;
}

struct _GIOChannel;
typedef struct _GIOChannel      GIOChannel;
class QIODevice;

namespace GpgME {

    GPGMEPP_EXPORT void initializeLibrary();
    /*!
      Initializes the library, returns Error::code() ==
      GPG_ERR_USER_1 if underlying gpgme is too old.
    */
    GPGMEPP_EXPORT Error initializeLibrary(int);

    enum Protocol { OpenPGP, CMS, UnknownProtocol };

    enum Engine { GpgEngine, GpgSMEngine, GpgConfEngine, UnknownEngine, AssuanEngine, G13Engine };

    enum KeyListMode {
      Local = 0x1,
      Extern = 0x2,
      Signatures = 0x4,
      SignatureNotations = 0x8,
      Validate = 0x10,
      Ephemeral = 0x20
    };

    enum SignatureMode { NormalSignatureMode, Detached, Clearsigned };

    GPGMEPP_EXPORT std::ostream & operator<<( std::ostream & os, Protocol proto );
    GPGMEPP_EXPORT std::ostream & operator<<( std::ostream & os, Engine eng );
    GPGMEPP_EXPORT std::ostream & operator<<( std::ostream & os, KeyListMode mode );
    GPGMEPP_EXPORT std::ostream & operator<<( std::ostream & os, SignatureMode mode );


    GPGMEPP_EXPORT Error setDefaultLocale( int category, const char * value );

    GPGMEPP_EXPORT Context * wait( Error & e, bool hang=true );
    typedef void (*IdleFunction)(void);
    GPGMEPP_EXPORT IdleFunction registerIdleFunction( IdleFunction idleFunction );

    typedef void (*IOCallback)( void * data, int fd );

    GPGMEPP_EXPORT EngineInfo engineInfo( Protocol proto );
    GPGMEPP_EXPORT EngineInfo engineInfo( Engine engine );

    GPGMEPP_EXPORT Error checkEngine( Protocol proto );
    GPGMEPP_EXPORT Error checkEngine( Engine engine );

    GPGMEPP_EXPORT GIOChannel * getGIOChannel( int fd );
    GPGMEPP_EXPORT QIODevice  * getQIODevice( int fd );

    enum Feature {
        ValidatingKeylistModeFeature               = 0x00000001,
        CancelOperationFeature                     = 0x00000002,
        WrongKeyUsageFeature                       = 0x00000004,
        DefaultCertificateInclusionFeature         = 0x00000008,

        GetSetEngineInfoFeature                    = 0x00000010,
        EngineInfoHomeDirFeature                   = 0x00000020,
        NoEncryptToEncryptionFlagFeature           = 0x00000040,
        EphemeralKeylistModeFeature                = 0x00000080,

        SetDataFileNameFeeature                    = 0x00000100,
        VerificationResultFileNameFeature          = 0x00000200,
        DecryptionResultFileNameFeature            = 0x00000400,
        DecryptionResultRecipientsFeature          = 0x00000800,

        AuditLogFeature                            = 0x00001000,
        GpgConfEngineFeature                       = 0x00002000,
        CancelOperationAsyncFeature                = 0x00004000,
        AssuanEngineFeature                        = 0x00008000,

        ClearAddGetSignatureNotationsFeature       = 0x00010000,
        SignatureNotationsKeylistModeFeature       = 0x00020000,
        KeySignatureNotationsFeature               = 0x00040000,
        SignatureNotationsFlagsFeature             = 0x00080000,
        SignatureNotationsCriticalFlagFeature      = 0x00100000,
        SignatureNotationsHumanReadableFlagFeature = 0x00200000,
        CardKeyFeature                             = 0x00400000,
        ImportFromKeyserverFeature                 = 0x00800000,

        KeyIsQualifiedFeature                      = 0x01000200,
        SubkeyIsQualifiedFeature                   = 0x02000000,
        SignaturePkaFieldsFeature                  = 0x04000000,
        SignatureAlgorithmFieldsFeature            = 0x08000000,

        FdPointerFeature                           = 0x10000000,
        G13VFSFeature                              = 0x20000000,
        // reserved
        // unusable (max value)

        FeatureMaxValue                            = 0x80000000
    };
    GPGMEPP_EXPORT bool hasFeature( unsigned long feature );

} // namespace GpgME

#endif // __GPGMEPP_GLOBAL_H__
