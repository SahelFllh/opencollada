/*
    Copyright (c) 2008-2009 NetAllied Systems GmbH

    This file is part of COLLADASaxFrameworkLoader.

    Licensed under the MIT Open Source License, 
    for details please see LICENSE file or the website
    http://www.opensource.org/licenses/mit-license.php
*/


#ifndef __COLLADASAXFWL_LIBRARYPHYSICSMATERIALSLOADER15_H__
#define __COLLADASAXFWL_LIBRARYPHYSICSMATERIALSLOADER15_H__


#include "COLLADASaxFWLPrerequisites.h"
#include "COLLADASaxFWLLibraryPhysicsMaterialsLoader.h"
#include "COLLADASaxFWLPhysicsMaterialLoader15.h"


namespace COLLADASaxFWL
{


class IFilePartLoader;


class LibraryPhysicsMaterialsLoader15 : public PhysicsMaterialLoader15
{
private:
LibraryPhysicsMaterialsLoader* mLoader;

public:
LibraryPhysicsMaterialsLoader15(LibraryPhysicsMaterialsLoader* loader)
 : PhysicsMaterialLoader15(loader)
 , mLoader(loader)
{}


virtual bool begin__physics_material( const COLLADASaxFWL15::physics_material__AttributeData& attributeData );

virtual bool end__physics_material();

virtual bool end__library_physics_materials();

virtual bool begin__technique____technique_type( const COLLADASaxFWL15::technique____technique_type__AttributeData& attributeData );

virtual bool end__technique____technique_type();


private:
/** Disable default copy ctor. */
LibraryPhysicsMaterialsLoader15(const LibraryPhysicsMaterialsLoader15&);
/** Disable default assignment operator. */
const LibraryPhysicsMaterialsLoader15& operator=(const LibraryPhysicsMaterialsLoader15&);
};
}
#endif // __COLLADASAXFWL_LIBRARYPHYSICSMATERIALSLOADER15_H__
