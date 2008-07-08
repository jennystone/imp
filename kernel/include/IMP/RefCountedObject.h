/**
 *  \file RefCountedObject.h     
 *  \brief A common base class for ref counted objects.
 *
 *  Copyright 2007-8 Sali Lab. All rights reserved.
 *
 */

#ifndef __IMP_REF_COUNTED_OBJECT_H
#define __IMP_REF_COUNTED_OBJECT_H

#include "Object.h"

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

namespace IMP
{

//! Common base class for ref counted objects.
/** This class acts as a tag rather than providing any functionality.

   \internal
 */
class IMPDLLEXPORT RefCountedObject: public Object
{
  typedef Object P;
  typedef RefCountedObject This;
  static unsigned int live_objects_;
protected:
  RefCountedObject() {
     ++live_objects_;
  }

public:

  virtual ~RefCountedObject() {
    IMP_assert(!get_has_ref(), "Deleting object which still has references");
    --live_objects_;
  }

  static unsigned int get_number_of_live_objects() {
    // for debugging purposes only
    return live_objects_;
  }
};

} // namespace IMP

#endif  /* __IMP_REF_COUNTED_OBJECT_H */
