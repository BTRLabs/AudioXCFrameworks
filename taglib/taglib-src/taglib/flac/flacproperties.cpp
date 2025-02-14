/***************************************************************************
    copyright            : (C) 2003 by Allan Sandfeld Jensen
    email                : kde@carewolf.org
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

#include "flacproperties.h"
#include "flacfile.h"

using namespace TagLib;

class FLAC::AudioProperties::PropertiesPrivate
{
public:
  PropertiesPrivate() :
    length(0),
    bitrate(0),
    sampleRate(0),
    bitsPerSample(0),
    channels(0),
    sampleFrames(0) {}

  int length;
  int bitrate;
  int sampleRate;
  int bitsPerSample;
  int channels;
  unsigned long long sampleFrames;
  ByteVector signature;
};

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

FLAC::AudioProperties::AudioProperties(const ByteVector &data, long long streamLength, ReadStyle) :
  TagLib::AudioProperties(),
  d(new PropertiesPrivate())
{
  read(data, streamLength);
}

FLAC::AudioProperties::~AudioProperties()
{
  delete d;
}

int FLAC::AudioProperties::length() const
{
  return lengthInSeconds();
}

int FLAC::AudioProperties::lengthInSeconds() const
{
  return d->length / 1000;
}

int FLAC::AudioProperties::lengthInMilliseconds() const
{
  return d->length;
}

int FLAC::AudioProperties::bitrate() const
{
  return d->bitrate;
}

int FLAC::AudioProperties::sampleRate() const
{
  return d->sampleRate;
}

int FLAC::AudioProperties::bitsPerSample() const
{
  return d->bitsPerSample;
}

int FLAC::AudioProperties::sampleWidth() const
{
  return bitsPerSample();
}

int FLAC::AudioProperties::channels() const
{
  return d->channels;
}

unsigned long long FLAC::AudioProperties::sampleFrames() const
{
  return d->sampleFrames;
}

ByteVector FLAC::AudioProperties::signature() const
{
  return d->signature;
}

////////////////////////////////////////////////////////////////////////////////
// private members
////////////////////////////////////////////////////////////////////////////////

void FLAC::AudioProperties::read(const ByteVector &data, long long streamLength)
{
  if(data.size() < 18) {
    debug("FLAC::AudioProperties::read() - FLAC properties must contain at least 18 bytes.");
    return;
  }

  size_t pos = 0;

  // Minimum block size (in samples)
  pos += 2;

  // Maximum block size (in samples)
  pos += 2;

  // Minimum frame size (in bytes)
  pos += 3;

  // Maximum frame size (in bytes)
  pos += 3;

  const unsigned int flags = data.toUInt32BE(pos);
  pos += 4;

  d->sampleRate    = flags >> 12;
  d->channels      = ((flags >> 9) &  7) + 1;
  d->bitsPerSample = ((flags >> 4) & 31) + 1;

  // The last 4 bits are the most significant 4 bits for the 36 bit
  // stream length in samples. (Audio files measured in days)

  const unsigned long long hi = flags & 0xf;
  const unsigned long long lo = data.toUInt32BE(pos);
  pos += 4;

  d->sampleFrames = (hi << 32) | lo;

  if(d->sampleFrames > 0 && d->sampleRate > 0) {
    const double length = d->sampleFrames * 1000.0 / d->sampleRate;
    d->length  = static_cast<int>(length + 0.5);
    d->bitrate = static_cast<int>(streamLength * 8.0 / length + 0.5);
  }

  if(data.size() >= pos + 16)
    d->signature = data.mid(pos, 16);
}
