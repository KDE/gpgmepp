/*  This file is part of the KDE project
    Copyright (C) 2007 David Faure <faure@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef GPGMEPP_EXPORT_H
#define GPGMEPP_EXPORT_H

/* needed for GPGMEPP_EXPORT and KDE_IMPORT macros */
#include <kdemacros.h>

#ifndef GPGMEPP_EXPORT
# if defined(MAKE_GPGME___LIB) || defined(MAKE_GPGMEPP_LIB)
   /* We are building this library */ 
#  define GPGMEPP_EXPORT KDE_EXPORT
# else
   /* We are using this library */ 
#  define GPGMEPP_EXPORT KDE_IMPORT
# endif
#endif

# ifndef GPGMEPP_EXPORT_DEPRECATED
#  define GPGMEPP_EXPORT_DEPRECATED KDE_DEPRECATED GPGMEPP_EXPORT
# endif

# ifndef GPGMEPP_DEPRECATED
#  define GPGMEPP_DEPRECATED KDE_DEPRECATED
# endif

# ifndef GPGMEPP_MAKE_STD_SWAP_SPECIALIZATION
#  define GPGMEPP_MAKE_STD_SWAP_SPECIALIZATION( Class ) \
    namespace std { template <> inline void swap< GpgME::Class >( GpgME::Class & lhs, GpgME::Class & rhs ) { lhs.swap( rhs ); } }
# endif

#endif
