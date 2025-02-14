/***************************************************************************
    copyright           : (C) 2011 by Mathias Panzenböck
    email               : grosser.meister.morti@gmx.net
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

#ifndef TAGLIB_MODPROPERTIES_H
#define TAGLIB_MODPROPERTIES_H

#include <taglib/taglib.h>
#include <taglib/audioproperties.h>

namespace TagLib {

  namespace Mod {

    class File;

    class TAGLIB_EXPORT AudioProperties : public TagLib::AudioProperties
    {
      friend class File;

    public:
      explicit AudioProperties(AudioProperties::ReadStyle propertiesStyle);
      virtual ~AudioProperties();

      int length()               const;
      int lengthInSeconds()      const;
      int lengthInMilliseconds() const;
      int bitrate()              const;
      int sampleRate()           const;
      int channels()             const;

      unsigned int  instrumentCount()  const;
      unsigned char lengthInPatterns() const;

    private:
      void setChannels(int channels);

      void setInstrumentCount(unsigned int sampleCount);
      void setLengthInPatterns(unsigned char lengthInPatterns);

      class PropertiesPrivate;
      PropertiesPrivate *d;
    };

  }

}

#endif
