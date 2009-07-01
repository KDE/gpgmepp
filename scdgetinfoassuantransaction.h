/*
  scdgetinfoassuantransaction.h - Assuan Transaction to get information from scdaemon
  Copyright (C) 2009 Klarälvdalens Datakonsult AB

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

#ifndef __GPGMEPP_SCDGETINFOASSUANTRANSACTION_H__
#define __GPGMEPP_SCDGETINFOASSUANTRANSACTION_H__

#include <gpgme++/interfaces/assuantransaction.h>

#include <string>
#include <vector>

namespace GpgME {

    class GPGMEPP_EXPORT ScdGetInfoAssuanTransaction : public AssuanTransaction {
    public:
        enum InfoItem {
            Version,         // string
            Pid,             // unsigned long
            SocketName,      // string (path)
            Status,          // char (status)
            ReaderList,      // string list
            DenyAdmin,       // (none, returns GPG_ERR_GENERAL when admin commands are allowed)
            ApplicationList, // string list

            LastInfoItem
        };

        explicit ScdGetInfoAssuanTransaction( InfoItem item );
        ~ScdGetInfoAssuanTransaction();
            
        std::string version() const;
        unsigned int pid() const;
        std::string socketName() const;
        char status() const;
        std::vector<std::string> readerList() const;
        std::vector<std::string> applicationList() const;

    private:
        /* reimp */ const char * command() const;
        /* reimp */ Error data( const char * data, size_t datalen );
        /* reimp */ Data inquire( const char * name, const char * args, Error & err );
        /* reimp */ Error status( const char * status, const char * args );

    private:
        void makeCommand() const;

    private:
        InfoItem m_item;
        mutable std::string m_command;
        std::string m_data;
    };

} // namespace GpgME

#endif // __GPGMEPP_SCDGETINFOASSUANTRANSACTION_H__
