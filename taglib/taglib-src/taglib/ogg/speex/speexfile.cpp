/***************************************************************************
    copyright            : (C) 2006 by Lukáš Lalinský
    email                : lalinsky@gmail.com

    copyright            : (C) 2002 - 2008 by Scott Wheeler
    email                : wheeler@kde.org
                           (original Vorbis implementation)
 ***************************************************************************/

/***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA         *
 *   02110-1301  USA                                                       *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

#include <tstring.h>
#include "tdebug.h"
#include <tpropertymap.h>
#include "tagutils.h"

#include "speexfile.h"

using namespace TagLib;
using namespace TagLib::Ogg;

class Speex::File::FilePrivate
{
public:
  FilePrivate() :
    comment(0),
    properties(0) {}

  ~FilePrivate()
  {
    delete comment;
    delete properties;
  }

  Ogg::XiphComment *comment;
  AudioProperties *properties;
};

////////////////////////////////////////////////////////////////////////////////
// static members
////////////////////////////////////////////////////////////////////////////////

bool Ogg::Speex::File::isSupported(IOStream *stream)
{
  // A Speex file has IDs "OggS" and "Speex   " somewhere.

  const ByteVector buffer = Utils::readHeader(stream, bufferSize(), false);
  return (buffer.find("OggS") != ByteVector::npos()
    && buffer.find("Speex   ") != ByteVector::npos());
}

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

Speex::File::File(FileName file, bool readProperties, AudioProperties::ReadStyle) :
  Ogg::File(file),
  d(new FilePrivate())
{
  if(isOpen())
    read(readProperties);
}

Speex::File::File(IOStream *stream, bool readProperties, AudioProperties::ReadStyle) :
  Ogg::File(stream),
  d(new FilePrivate())
{
  if(isOpen())
    read(readProperties);
}

Speex::File::~File()
{
  delete d;
}

Ogg::XiphComment *Speex::File::tag() const
{
  return d->comment;
}

Speex::AudioProperties *Speex::File::audioProperties() const
{
  return d->properties;
}

bool Speex::File::save()
{
  if(!d->comment)
    d->comment = new Ogg::XiphComment();

  setPacket(1, d->comment->render());

  return Ogg::File::save();
}

////////////////////////////////////////////////////////////////////////////////
// private members
////////////////////////////////////////////////////////////////////////////////

void Speex::File::read(bool readProperties)
{
  ByteVector speexHeaderData = packet(0);

  if(!speexHeaderData.startsWith("Speex   ")) {
    debug("Speex::File::read() -- invalid Speex identification header");
    return;
  }

  ByteVector commentHeaderData = packet(1);

  d->comment = new Ogg::XiphComment(commentHeaderData);

  if(readProperties)
    d->properties = new AudioProperties(this);
}
