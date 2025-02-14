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

#include "tdebug.h"
#include <tmap.h>

#include "relativevolumeframe.h"

using namespace TagLib;
using namespace ID3v2;

namespace
{
  struct ChannelData
  {
    ChannelData() : channelType(RelativeVolumeFrame::Other), volumeAdjustment(0) {}

    RelativeVolumeFrame::ChannelType channelType;
    short volumeAdjustment;
    RelativeVolumeFrame::PeakVolume peakVolume;
  };
}

class RelativeVolumeFrame::RelativeVolumeFramePrivate
{
public:
  String identification;
  Map<ChannelType, ChannelData> channels;
};

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

RelativeVolumeFrame::RelativeVolumeFrame() :
  Frame("RVA2"),
  d(new RelativeVolumeFramePrivate())
{
}

RelativeVolumeFrame::RelativeVolumeFrame(const ByteVector &data) :
  Frame(data),
  d(new RelativeVolumeFramePrivate())
{
  setData(data);
}

RelativeVolumeFrame::~RelativeVolumeFrame()
{
  delete d;
}

String RelativeVolumeFrame::toString() const
{
  return d->identification;
}

List<RelativeVolumeFrame::ChannelType> RelativeVolumeFrame::channels() const
{
  List<ChannelType> l;

  Map<ChannelType, ChannelData>::ConstIterator it = d->channels.begin();
  for(; it != d->channels.end(); ++it)
    l.append((*it).first);

  return l;
}

// deprecated

RelativeVolumeFrame::ChannelType RelativeVolumeFrame::channelType() const
{
  return MasterVolume;
}

// deprecated

void RelativeVolumeFrame::setChannelType(ChannelType)
{

}

short RelativeVolumeFrame::volumeAdjustmentIndex(ChannelType type) const
{
  return d->channels.contains(type) ? d->channels[type].volumeAdjustment : 0;
}

void RelativeVolumeFrame::setVolumeAdjustmentIndex(short index, ChannelType type)
{
  d->channels[type].volumeAdjustment = index;
}

float RelativeVolumeFrame::volumeAdjustment(ChannelType type) const
{
  return d->channels.contains(type) ? float(d->channels[type].volumeAdjustment) / float(512) : 0;
}

void RelativeVolumeFrame::setVolumeAdjustment(float adjustment, ChannelType type)
{
  d->channels[type].volumeAdjustment = short(adjustment * float(512));
}

RelativeVolumeFrame::PeakVolume RelativeVolumeFrame::peakVolume(ChannelType type) const
{
  return d->channels.contains(type) ? d->channels[type].peakVolume : PeakVolume();
}

void RelativeVolumeFrame::setPeakVolume(const PeakVolume &peak, ChannelType type)
{
  d->channels[type].peakVolume = peak;
}

String RelativeVolumeFrame::identification() const
{
  return d->identification;
}

void RelativeVolumeFrame::setIdentification(const String &s)
{
  d->identification = s;
}

////////////////////////////////////////////////////////////////////////////////
// protected members
////////////////////////////////////////////////////////////////////////////////

void RelativeVolumeFrame::parseFields(const ByteVector &data)
{
  size_t pos = 0;
  d->identification = readStringField(data, String::Latin1, pos);

  // Each channel is at least 4 bytes.

  while(pos + 4 <= data.size()) {

    ChannelType type = ChannelType(data[pos]);
    pos += 1;

    ChannelData &channel = d->channels[type];

    channel.volumeAdjustment = data.toInt16BE(pos);
    pos += 2;

    channel.peakVolume.bitsRepresentingPeak = data[pos];
    pos += 1;

    const int bytes = (channel.peakVolume.bitsRepresentingPeak + 7) / 8;
    channel.peakVolume.peakVolume = data.mid(pos, bytes);
    pos += bytes;
  }
}

ByteVector RelativeVolumeFrame::renderFields() const
{
  ByteVector data;

  data.append(d->identification.data(String::Latin1));
  data.append(textDelimiter(String::Latin1));

  Map<ChannelType, ChannelData>::ConstIterator it = d->channels.begin();

  for(; it != d->channels.end(); ++it) {
    ChannelType type = (*it).first;
    const ChannelData &channel = (*it).second;

    data.append(char(type));
    data.append(ByteVector::fromUInt16BE(channel.volumeAdjustment));
    data.append(char(channel.peakVolume.bitsRepresentingPeak));
    data.append(channel.peakVolume.peakVolume);
  }

  return data;
}

////////////////////////////////////////////////////////////////////////////////
// private members
////////////////////////////////////////////////////////////////////////////////

RelativeVolumeFrame::RelativeVolumeFrame(const ByteVector &data, Header *h) :
  Frame(h),
  d(new RelativeVolumeFramePrivate())
{
  parseFields(fieldData(data));
}
