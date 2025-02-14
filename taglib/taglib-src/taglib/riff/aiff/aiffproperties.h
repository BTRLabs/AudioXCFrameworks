/***************************************************************************
    copyright            : (C) 2008 by Scott Wheeler
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

#ifndef TAGLIB_AIFFPROPERTIES_H
#define TAGLIB_AIFFPROPERTIES_H

#include <taglib/audioproperties.h>

namespace TagLib {

  namespace RIFF {

    namespace AIFF {

      class File;

      //! An implementation of audio property reading for AIFF or AIFF-C

      /*!
       * This reads the data from an AIFF stream found in the AudioProperties
       * API.
       */

      class TAGLIB_EXPORT AudioProperties : public TagLib::AudioProperties
      {
      public:
        /*!
         * Create an instance of AIFF::AudioProperties with the data read from
         * the AIFF::File \a file.
         */
        AudioProperties(File *file, ReadStyle style = Average);

        /*!
         * Destroys this AIFF::AudioProperties instance.
         */
        virtual ~AudioProperties();

        virtual bool isNull() const;

        /*!
         * Returns the length of the file in seconds.  The length is rounded down to
         * the nearest whole second.
         *
         * \note This method is just an alias of lengthInSeconds().
         *
         * \deprecated
         */
        virtual int length() const;

        /*!
         * Returns the length of the file in seconds.  The length is rounded down to
         * the nearest whole second.
         *
         * \see lengthInMilliseconds()
         */
        virtual int lengthInSeconds() const;

        /*!
         * Returns the length of the file in milliseconds.
         *
         * \see lengthInSeconds()
         */
        virtual int lengthInMilliseconds() const;

        /*!
         * Returns the average bit rate of the file in kb/s.
         */
        virtual int bitrate() const;

        /*!
         * Returns the sample rate in Hz.
         */
        virtual int sampleRate() const;

        /*!
         * Returns the number of audio channels.
         */
        virtual int channels() const;

        /*!
         * Returns the number of bits per audio sample.
         */
        int bitsPerSample() const;

        /*!
         * Returns the number of bits per audio sample.
         *
         * \note This method is just an alias of bitsPerSample().
         *
         * \deprecated
         */
        int sampleWidth() const;

        /*!
         * Returns the number of sample frames
         */
        unsigned int sampleFrames() const;

        /*!
         * Returns true if the file is in AIFF-C format, false if AIFF format.
         */
        bool isAiffC() const;

        /*!
         * Returns the compression type of the AIFF-C file.  For example, "NONE" for
         * not compressed, "ACE2" for ACE 2-to-1.
         *
         * If the file is in AIFF format, always returns an empty vector.
         *
         * \see isAiffC()
         */
        ByteVector compressionType() const;

        /*!
         * Returns the concrete compression name of the AIFF-C file.
         *
         * If the file is in AIFF format, always returns an empty string.
         *
         * \see isAiffC()
         */
        String compressionName() const;

      private:
        void read(File *file);

        class PropertiesPrivate;
        PropertiesPrivate *d;
      };
    }
  }
}

#endif
