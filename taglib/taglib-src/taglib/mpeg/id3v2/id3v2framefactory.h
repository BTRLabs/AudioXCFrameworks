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

#ifndef TAGLIB_ID3V2FRAMEFACTORY_H
#define TAGLIB_ID3V2FRAMEFACTORY_H

#include <taglib/taglib_export.h>
#include <taglib/tbytevector.h>
#include <taglib/id3v2frame.h>
#include <taglib/id3v2header.h>

namespace TagLib {

  namespace ID3v2 {

    class TextIdentificationFrame;

    //! A factory for creating ID3v2 frames during parsing

    /*!
     * This factory abstracts away the frame creation process and instantiates
     * the appropriate ID3v2::Frame subclasses based on the contents of the
     * data.
     *
     * Reimplementing this factory is the key to adding support for frame types
     * not directly supported by TagLib to your application.  To do so you would
     * subclass this factory reimplement createFrame().  Then by setting your
     * factory to be the default factory in ID3v2::Tag constructor you can
     * implement behavior that will allow for new ID3v2::Frame subclasses (also
     * provided by you) to be used.
     *
     * This implements both <i>abstract factory</i> and <i>singleton</i> patterns
     * of which more information is available on the web and in software design
     * textbooks (Notably <i>Design Patters</i>).
     *
     * \note You do not need to use this factory to create new frames to add to
     * an ID3v2::Tag.  You can instantiate frame subclasses directly (with new)
     * and add them to a tag using ID3v2::Tag::addFrame()
     *
     * \see ID3v2::Tag::addFrame()
     */

    class TAGLIB_EXPORT FrameFactory
    {
    public:
      static FrameFactory *instance();
      /*!
       * Create a frame based on \a data.  \a tagHeader should be a valid
       * ID3v2::Header instance.
       */
      virtual Frame *createFrame(const ByteVector &data, const Header *tagHeader) const;

      /*!
       * After a tag has been read, this tries to rebuild some of them
       * information, most notably the recording date, from frames that
       * have been deprecated and can't be upgraded directly.
       */
      // BIC: Make virtual
      void rebuildAggregateFrames(ID3v2::Tag *tag) const;

      /*!
       * Returns the default text encoding for text frames.  If setTextEncoding()
       * has not been explicitly called this will only be used for new text
       * frames.  However, if this value has been set explicitly all frames will be
       * converted to this type (unless it's explicitly set differently for the
       * individual frame) when being rendered.
       *
       * \see setDefaultTextEncoding()
       */
      String::Type defaultTextEncoding() const;

      /*!
       * Set the default text encoding for all text frames that are created to
       * \a encoding.  If no value is set the frames with either default to the
       * encoding type that was parsed and new frames default to Latin1.
       *
       * Valid string types for ID3v2 tags are Latin1, UTF8, UTF16 and UTF16BE.
       *
       * \see defaultTextEncoding()
       */
      void setDefaultTextEncoding(String::Type encoding);

    protected:
      /*!
       * Constructs a frame factory.  Because this is a singleton this method is
       * protected, but may be used for subclasses.
       */
      FrameFactory();

      /*!
       * Destroys the frame factory.
       */
      virtual ~FrameFactory();

      /*!
       * This method checks for compliance to the current ID3v2 standard (2.4)
       * and does nothing in the common case.  However if a frame is found that
       * is not compatible with the current standard, this method either updates
       * the frame or indicates that it should be discarded.
       *
       * This method with return true (with or without changes to the frame) if
       * this frame should be kept or false if it should be discarded.
       *
       * See the id3v2.4.0-changes.txt document for further information.
       */
      virtual bool updateFrame(Frame::Header *header) const;

    private:
      FrameFactory(const FrameFactory &);
      FrameFactory &operator=(const FrameFactory &);

      static FrameFactory factory;

      class FrameFactoryPrivate;
      FrameFactoryPrivate *d;
    };

  }
}

#endif
