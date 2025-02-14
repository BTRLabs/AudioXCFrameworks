/***************************************************************************
    copyright            : (C) 2002 - 2008 by Scott Wheeler
    email                : wheeler@kde.org
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

#include <oggpageheader.h>

#include "vorbisproperties.h"
#include "vorbisfile.h"

using namespace TagLib;

class Ogg::Vorbis::AudioProperties::PropertiesPrivate
{
public:
  PropertiesPrivate() :
    length(0),
    bitrate(0),
    sampleRate(0),
    channels(0),
    vorbisVersion(0),
    bitrateMaximum(0),
    bitrateNominal(0),
    bitrateMinimum(0) {}

  int length;
  int bitrate;
  int sampleRate;
  int channels;
  int vorbisVersion;
  int bitrateMaximum;
  int bitrateNominal;
  int bitrateMinimum;
};

namespace TagLib {
  /*!
   * Vorbis headers can be found with one type ID byte and the string "vorbis" in
   * an Ogg stream.  0x01 indicates the setup header.
   */
  const char vorbisSetupHeaderID[] = { 0x01, 'v', 'o', 'r', 'b', 'i', 's', 0 };
}

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

Ogg::Vorbis::AudioProperties::AudioProperties(File *file, ReadStyle) :
  TagLib::AudioProperties(),
  d(new PropertiesPrivate())
{
  read(file);
}

Ogg::Vorbis::AudioProperties::~AudioProperties()
{
  delete d;
}

int Ogg::Vorbis::AudioProperties::length() const
{
  return lengthInSeconds();
}

int Ogg::Vorbis::AudioProperties::lengthInSeconds() const
{
  return d->length / 1000;
}

int Ogg::Vorbis::AudioProperties::lengthInMilliseconds() const
{
  return d->length;
}

int Ogg::Vorbis::AudioProperties::bitrate() const
{
  return d->bitrate;
}

int Ogg::Vorbis::AudioProperties::sampleRate() const
{
  return d->sampleRate;
}

int Ogg::Vorbis::AudioProperties::channels() const
{
  return d->channels;
}

int Ogg::Vorbis::AudioProperties::vorbisVersion() const
{
  return d->vorbisVersion;
}

int Ogg::Vorbis::AudioProperties::bitrateMaximum() const
{
  return d->bitrateMaximum;
}

int Ogg::Vorbis::AudioProperties::bitrateNominal() const
{
  return d->bitrateNominal;
}

int Ogg::Vorbis::AudioProperties::bitrateMinimum() const
{
  return d->bitrateMinimum;
}

String Ogg::Vorbis::AudioProperties::toString() const
{
  StringList desc;
  desc.append("Ogg Vorbis audio (version " + String::number(vorbisVersion()) + ")");
  desc.append(String::number(length()) + " seconds");
  desc.append(String::number(bitrate()) + " kbps");
  return desc.toString(", ");
}

////////////////////////////////////////////////////////////////////////////////
// private members
////////////////////////////////////////////////////////////////////////////////

void Ogg::Vorbis::AudioProperties::read(File *file)
{
  // Get the identification header from the Ogg implementation.

  const ByteVector data = file->packet(0);
  if(data.size() < 28) {
    debug("Ogg::Vorbis::AudioProperties::read() -- data is too short.");
    return;
  }

  size_t pos = 0;

  if(data.mid(pos, 7) != vorbisSetupHeaderID) {
    debug("Ogg::Vorbis::AudioProperties::read() -- invalid Ogg::Vorbis identification header");
    return;
  }

  pos += 7;

  d->vorbisVersion = data.toUInt32LE(pos);
  pos += 4;

  d->channels = static_cast<unsigned char>(data[pos]);
  pos += 1;

  d->sampleRate = data.toUInt32LE(pos);
  pos += 4;

  d->bitrateMaximum = data.toUInt32LE(pos);
  pos += 4;

  d->bitrateNominal = data.toUInt32LE(pos);
  pos += 4;

  d->bitrateMinimum = data.toUInt32LE(pos);
  pos += 4;

  // Find the length of the file.  See http://wiki.xiph.org/Ogg::VorbisStreamLength/
  // for my notes on the topic.

  const Ogg::PageHeader *first = file->firstPageHeader();
  const Ogg::PageHeader *last  = file->lastPageHeader();

  if(first && last) {
    const long long start = first->absoluteGranularPosition();
    const long long end   = last->absoluteGranularPosition();

    if(start >= 0 && end >= 0 && d->sampleRate > 0) {
      const long long frameCount = end - start;

      if(frameCount > 0) {
        const double length = frameCount * 1000.0 / d->sampleRate;

        d->length  = static_cast<int>(length + 0.5);
        d->bitrate = static_cast<int>(file->length() * 8.0 / length + 0.5);
      }
    }
    else {
      debug("Ogg::Vorbis::AudioProperties::read() -- Either the PCM values for the start or "
            "end of this file was incorrect or the sample rate is zero.");
    }
  }
  else
    debug("Ogg::Vorbis::AudioProperties::read() -- Could not find valid first and last Ogg pages.");

  // Alternative to the actual average bitrate.

  if(d->bitrate == 0 && d->bitrateNominal > 0)
    d->bitrate = static_cast<int>(d->bitrateNominal / 1000.0 + 0.5);
}
