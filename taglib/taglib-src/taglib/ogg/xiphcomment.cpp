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

#include <tbytevector.h>
#include "tdebug.h"

#include <flacpicture.h>
#include <xiphcomment.h>
#include <tpicturemap.h>
#include <tpropertymap.h>

using namespace TagLib;

namespace
{
  typedef Ogg::FieldListMap::Iterator FieldIterator;
  typedef Ogg::FieldListMap::ConstIterator FieldConstIterator;

  typedef List<FLAC::Picture *> XiphPictureList;
  typedef XiphPictureList::Iterator PictureIterator;
  typedef XiphPictureList::Iterator PictureConstIterator;
}

class Ogg::XiphComment::XiphCommentPrivate
{
public:
  XiphCommentPrivate()
  {
    pictureList.setAutoDelete(true);
  }

  FieldListMap fieldListMap;
  String vendorID;
  String commentField;
  XiphPictureList pictureList;
};

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

Ogg::XiphComment::XiphComment() :
  TagLib::Tag(),
  d(new XiphCommentPrivate())
{
}

Ogg::XiphComment::XiphComment(const ByteVector &data) :
  TagLib::Tag(),
  d(new XiphCommentPrivate())
{
  parse(data);
}

Ogg::XiphComment::~XiphComment()
{
  delete d;
}

String Ogg::XiphComment::title() const
{
  if(d->fieldListMap["TITLE"].isEmpty())
    return String();
  return d->fieldListMap["TITLE"].toString();
}

String Ogg::XiphComment::artist() const
{
  if(d->fieldListMap["ARTIST"].isEmpty())
    return String();
  return d->fieldListMap["ARTIST"].toString();
}

String Ogg::XiphComment::album() const
{
  if(d->fieldListMap["ALBUM"].isEmpty())
    return String();
  return d->fieldListMap["ALBUM"].toString();
}

String Ogg::XiphComment::comment() const
{
  if(!d->fieldListMap["DESCRIPTION"].isEmpty()) {
    d->commentField = "DESCRIPTION";
    return d->fieldListMap["DESCRIPTION"].toString();
  }

  if(!d->fieldListMap["COMMENT"].isEmpty()) {
    d->commentField = "COMMENT";
    return d->fieldListMap["COMMENT"].toString();
  }

  return String();
}

String Ogg::XiphComment::genre() const
{
  if(d->fieldListMap["GENRE"].isEmpty())
    return String();
  return d->fieldListMap["GENRE"].toString();
}

unsigned int Ogg::XiphComment::year() const
{
  if(!d->fieldListMap["DATE"].isEmpty())
    return d->fieldListMap["DATE"].front().toInt();
  if(!d->fieldListMap["YEAR"].isEmpty())
    return d->fieldListMap["YEAR"].front().toInt();
  return 0;
}

unsigned int Ogg::XiphComment::track() const
{
  if(!d->fieldListMap["TRACKNUMBER"].isEmpty())
    return d->fieldListMap["TRACKNUMBER"].front().toInt();
  if(!d->fieldListMap["TRACKNUM"].isEmpty())
    return d->fieldListMap["TRACKNUM"].front().toInt();
  return 0;
}

TagLib::PictureMap Ogg::XiphComment::pictures() const
{
    return PictureMap();
}

void Ogg::XiphComment::setTitle(const String &s)
{
  addField("TITLE", s);
}

void Ogg::XiphComment::setArtist(const String &s)
{
  addField("ARTIST", s);
}

void Ogg::XiphComment::setAlbum(const String &s)
{
  addField("ALBUM", s);
}

void Ogg::XiphComment::setComment(const String &s)
{
  if(d->commentField.isEmpty()) {
    if(!d->fieldListMap["DESCRIPTION"].isEmpty())
      d->commentField = "DESCRIPTION";
    else
      d->commentField = "COMMENT";
  }

  addField(d->commentField, s);
}

void Ogg::XiphComment::setGenre(const String &s)
{
  addField("GENRE", s);
}

void Ogg::XiphComment::setYear(unsigned int i)
{
  removeFields("YEAR");
  if(i == 0)
    removeFields("DATE");
  else
    addField("DATE", String::number(i));
}

void Ogg::XiphComment::setTrack(unsigned int i)
{
  removeFields("TRACKNUM");
  if(i == 0)
    removeFields("TRACKNUMBER");
  else
    addField("TRACKNUMBER", String::number(i));
}

void Ogg::XiphComment::setPictures(const PictureMap &l)
{
}

bool Ogg::XiphComment::isEmpty() const
{
  for(FieldConstIterator it = d->fieldListMap.begin(); it != d->fieldListMap.end(); ++it) {
    if(!(*it).second.isEmpty())
      return false;
  }

  return true;
}

unsigned int Ogg::XiphComment::fieldCount() const
{
  size_t count = 0;

  for(FieldConstIterator it = d->fieldListMap.begin(); it != d->fieldListMap.end(); ++it)
    count += (*it).second.size();

  count += d->pictureList.size();

  return static_cast<unsigned int>(count);
}

const Ogg::FieldListMap &Ogg::XiphComment::fieldListMap() const
{
  return d->fieldListMap;
}

PropertyMap Ogg::XiphComment::properties() const
{
  return d->fieldListMap;
}

PropertyMap Ogg::XiphComment::setProperties(const PropertyMap &properties)
{
  // check which keys are to be deleted
  StringList toRemove;
  for(FieldConstIterator it = d->fieldListMap.begin(); it != d->fieldListMap.end(); ++it)
    if (!properties.contains(it->first))
      toRemove.append(it->first);

  for(StringList::ConstIterator it = toRemove.begin(); it != toRemove.end(); ++it)
      removeFields(*it);

  // now go through keys in \a properties and check that the values match those in the xiph comment
  PropertyMap invalid;
  PropertyMap::ConstIterator it = properties.begin();
  for(; it != properties.end(); ++it)
  {
    if(!checkKey(it->first))
      invalid.insert(it->first, it->second);
    else if(!d->fieldListMap.contains(it->first) || !(it->second == d->fieldListMap[it->first])) {
      const StringList &sl = it->second;
      if(sl.isEmpty())
        // zero size string list -> remove the tag with all values
        removeFields(it->first);
      else {
        // replace all strings in the list for the tag
        StringList::ConstIterator valueIterator = sl.begin();
        addField(it->first, *valueIterator, true);
        ++valueIterator;
        for(; valueIterator != sl.end(); ++valueIterator)
          addField(it->first, *valueIterator, false);
      }
    }
  }
  return invalid;
}

bool Ogg::XiphComment::checkKey(const String &key)
{
  if(key.size() < 1)
    return false;

  // A key may consist of ASCII 0x20 through 0x7D, 0x3D ('=') excluded.

  for(String::ConstIterator it = key.begin(); it != key.end(); it++) {
      if(*it < 0x20 || *it > 0x7D || *it == 0x3D)
        return false;
  }

  return true;
}

String Ogg::XiphComment::vendorID() const
{
  return d->vendorID;
}

void Ogg::XiphComment::addField(const String &key, const String &value, bool replace)
{
  if(!checkKey(key)) {
    debug("Ogg::XiphComment::addField() - Invalid key. Field not added.");
    return;
  }

  const String upperKey = key.upper();

  if(replace)
    removeFields(upperKey);

  if(!key.isEmpty() && !value.isEmpty())
    d->fieldListMap[upperKey].append(value);
}

void Ogg::XiphComment::removeFields(const String &key)
{
  d->fieldListMap.erase(key.upper());
}

void Ogg::XiphComment::removeFields(const String &key, const String &value)
{
  StringList &fields = d->fieldListMap[key.upper()];
  for(StringList::Iterator it = fields.begin(); it != fields.end(); ) {
    if(*it == value)
      it = fields.erase(it);
    else
      ++it;
  }
}

void Ogg::XiphComment::removeAllFields()
{
  d->fieldListMap.clear();
}

bool Ogg::XiphComment::contains(const String &key) const
{
  return !d->fieldListMap[key.upper()].isEmpty();
}

void Ogg::XiphComment::removePicture(FLAC::Picture *picture, bool del)
{
  PictureIterator it = d->pictureList.find(picture);
  if(it != d->pictureList.end())
    d->pictureList.erase(it);

  if(del)
    delete picture;
}

void Ogg::XiphComment::removeAllPictures()
{
  d->pictureList.clear();
}

void Ogg::XiphComment::addPicture(FLAC::Picture * picture)
{
  d->pictureList.append(picture);
}

List<FLAC::Picture *> Ogg::XiphComment::pictureList()
{
  return d->pictureList;
}

ByteVector Ogg::XiphComment::render(bool addFramingBit) const
{
  ByteVector data;

  // Add the vendor ID length and the vendor ID.  It's important to use the
  // length of the data(String::UTF8) rather than the length of the the string
  // since this is UTF8 text and there may be more characters in the data than
  // in the UTF16 string.

  ByteVector vendorData = d->vendorID.data(String::UTF8);

  data.append(ByteVector::fromUInt32LE(vendorData.size()));
  data.append(vendorData);

  // Add the number of fields.

  data.append(ByteVector::fromUInt32LE(fieldCount()));

  // Iterate over the the field lists.  Our iterator returns a
  // std::pair<String, StringList> where the first String is the field name and
  // the StringList is the values associated with that field.

  FieldListMap::ConstIterator it = d->fieldListMap.begin();
  for(; it != d->fieldListMap.end(); ++it) {

    // And now iterate over the values of the current list.

    String fieldName = (*it).first;
    StringList values = (*it).second;

    StringList::ConstIterator valuesIt = values.begin();
    for(; valuesIt != values.end(); ++valuesIt) {
      ByteVector fieldData = fieldName.data(String::UTF8);
      fieldData.append('=');
      fieldData.append((*valuesIt).data(String::UTF8));

      data.append(ByteVector::fromUInt32LE(fieldData.size()));
      data.append(fieldData);
    }
  }

  for(PictureConstIterator it = d->pictureList.begin(); it != d->pictureList.end(); ++it) {
    ByteVector picture = (*it)->render().toBase64();
    data.append(ByteVector::fromUInt32LE(picture.size() + 23));
    data.append("METADATA_BLOCK_PICTURE=");
    data.append(picture);
  }

  // Append the "framing bit".

  if(addFramingBit)
    data.append(char(1));

  return data;
}

String Ogg::XiphComment::toString() const
{
  StringList desc;
  for(FieldListMap::ConstIterator i = d->fieldListMap.begin(); i != d->fieldListMap.end(); i++) {
    for(StringList::ConstIterator j = i->second.begin(); j != i->second.end(); j++) {
      desc.append(i->first + "=" + *j);
    }
  }
  return desc.toString("\n");
}

////////////////////////////////////////////////////////////////////////////////
// protected members
////////////////////////////////////////////////////////////////////////////////

void Ogg::XiphComment::parse(const ByteVector &data)
{
  // The first thing in the comment data is the vendor ID length, followed by a
  // UTF8 string with the vendor ID.

  size_t pos = 0;

  const unsigned int vendorLength = data.toUInt32LE(0);
  pos += 4;

  d->vendorID = String(data.mid(pos, vendorLength), String::UTF8);
  pos += vendorLength;

  // Next the number of fields in the comment vector.

  const unsigned int commentFields = data.toUInt32LE(pos);
  pos += 4;

  if(commentFields > (data.size() - 8) / 4) {
    return;
  }

  for(unsigned int i = 0; i < commentFields; i++) {

    // Each comment field is in the format "KEY=value" in a UTF8 string and has
    // 4 bytes before the text starts that gives the length.

    const unsigned int commentLength = data.toUInt32LE(pos);
    pos += 4;

    const ByteVector entry = data.mid(pos, commentLength);
    pos += commentLength;

    // Don't go past data end

    if(pos > data.size())
      break;

    // Check for field separator

    const size_t sep = entry.find('=');
    if(sep == 0 || sep == ByteVector::npos()) {
      debug("Ogg::XiphComment::parse() - Discarding a field. Separator not found.");
      continue;
    }

    // Parse the key

    const String key = String(entry.mid(0, sep), String::UTF8).upper();
    if(!checkKey(key)) {
      debug("Ogg::XiphComment::parse() - Discarding a field. Invalid key.");
      continue;
    }

    if(key == "METADATA_BLOCK_PICTURE" || key == "COVERART") {

      // Handle Pictures separately

      const ByteVector picturedata = ByteVector::fromBase64(entry.mid(sep + 1));
      if(picturedata.isEmpty()) {
        debug("Ogg::XiphComment::parse() - Discarding a field. Invalid base64 data");
        continue;
      }

      if(key[0] == L'M') {

        // Decode FLAC Picture

        FLAC::Picture * picture = new FLAC::Picture();
        if(picture->parse(picturedata)) {
          d->pictureList.append(picture);
        }
        else {
          delete picture;
          debug("Ogg::XiphComment::parse() - Failed to decode FLAC Picture block");
        }
      }
      else {

        // Assume it's some type of image file

        FLAC::Picture * picture = new FLAC::Picture();
        picture->setData(picturedata);
        picture->setMimeType("image/");
        picture->setType(FLAC::Picture::Other);
        d->pictureList.append(picture);
      }
    }
    else {

      // Parse the text

      addField(key, String(entry.mid(sep + 1), String::UTF8), false);
    }
  }
}
