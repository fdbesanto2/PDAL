/******************************************************************************
* Copyright (c) 2011, Michael P. Gerlek (mpg@flaxen.com)
*
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following 
* conditions are met:
* 
*     * Redistributions of source code must retain the above copyright 
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright 
*       notice, this list of conditions and the following disclaimer in 
*       the documentation and/or other materials provided 
*       with the distribution.
*     * Neither the name of Hobu, Inc. or Flaxen Geo Consulting nor the 
*       names of its contributors may be used to endorse or promote 
*       products derived from this software without specific prior 
*       written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
* OF SUCH DAMAGE.
****************************************************************************/

#include "libpc/PointLayout.hpp"

#include <iostream>
using std::cout;
using std::endl;


PointLayout::PointLayout()
  : m_numBytes(0),
  m_fieldIndex_X(-1),
  m_fieldIndex_Y(-1),
  m_fieldIndex_Z(-1)
{
  return;
}


PointLayout::PointLayout(const PointLayout& other)
{
  m_numBytes = other.m_numBytes;
  m_fields = other.m_fields;
  m_isActive = other.m_isActive;
  m_fieldIndex_X = other.m_fieldIndex_X;
  m_fieldIndex_Y = other.m_fieldIndex_Y;
  m_fieldIndex_Z = other.m_fieldIndex_Z;

  return;
}


PointLayout& PointLayout::operator=(const PointLayout& other)
{
  if (this != &other)
  {
    m_numBytes = other.m_numBytes;
    m_fields = other.m_fields;
    m_isActive = other.m_isActive;
    m_fieldIndex_X = other.m_fieldIndex_X;
    m_fieldIndex_Y = other.m_fieldIndex_Y;
    m_fieldIndex_Z = other.m_fieldIndex_Z;
  }

  return *this;
}


bool PointLayout::same(const PointLayout & other, bool ignoreActive) const
{
    if (m_numBytes != other.m_numBytes) return false;
    if (!ignoreActive)
    {
      if (m_isActive != other.m_isActive) return false;
    }
    if (m_fields != other.m_fields) return false;

    // we don't need to check m_fieldIndex_X, etc, because those fields just 
    // replicate the data in m_fields

    return true;
}


bool PointLayout::operator==(const PointLayout & other) const
{
  return same(other);
}


void PointLayout::addField(const Field& fieldParam)
{
  Field myField(fieldParam);

  const Field::DataItem item = myField.getItem();

  if (hasField(item))
    return;

  const int offset = m_numBytes;
  myField.setOffset(offset);
  m_numBytes += myField.getNumBytes();

  m_fields.push_back(myField);
  m_isActive.push_back(false);

  const int index = m_fields.size() - 1;
  myField.setIndex(index);

  if (item == Field::XPos)
  {
    m_fieldIndex_X = index;
  }
  else if (item == Field::YPos)
  {
    m_fieldIndex_Y = index;
  }
  else if (item == Field::ZPos)
  {
    m_fieldIndex_Z = index;
  }

  return;
}


void PointLayout::addFields(const std::vector<Field>& fields)
{
  for (size_t i=0; i<fields.size(); i++)
  {
    addField(fields[i]);
  }
}


void PointLayout::dump() const
{
  cout << "PointLayout:" << endl;

  for (size_t i=0; i<m_fields.size(); i++)
  {
    m_fields[i].dump();
    if (!isActive(i))
    {
      cout << "(invalid field)";
    }
    cout << endl;
  }
}


int PointLayout::getSizeInBytes() const
{
  return m_numBytes;
}


int PointLayout::getNumFields() const
{
  return m_fields.size();
}


int PointLayout::findFieldIndex(Field::DataItem item) const
{
  for (size_t index=0; index<m_fields.size(); index++)
  {
    const Field& field = m_fields[index];
    if (field.getItem() == item) 
    {
      return index;
    }
  }

  return -1;
}


bool PointLayout::hasField(Field::DataItem item) const
{
  for (size_t index=0; index<m_fields.size(); index++)
  {
    const Field& field = m_fields[index];
    if (field.getItem() == item) 
    {
      return true;
    }
  }

  return false;
}


int PointLayout::findFieldOffset(Field::DataItem item) const
{
  int index = findFieldIndex(item);
  if (index == -1)
    return -1;

  return m_fields[index].getOffset();
}
