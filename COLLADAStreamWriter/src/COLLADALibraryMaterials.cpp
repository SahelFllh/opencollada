/*
    Copyright (c) 2008 NetAllied Systems GmbH

	This file is part of COLLADAStreamWriter.
	
    Licensed under the MIT Open Source License, 
    for details please see LICENSE file or the website
    http://www.opensource.org/licenses/mit-license.php
*/


#include "COLLADAStreamWriter.h"
#include "COLLADALibraryMaterials.h"
#include "COLLADASWC.h"
#include "COLLADAUtils.h"

namespace COLLADA
{

    const String LibraryMaterials::MATERIAL_ID_SUFFIX = "-material";


    //-------------------------------------
    LibraryMaterials::LibraryMaterials ( COLLADA::StreamWriter* streamWriter )
            : Library ( streamWriter, CSWC::COLLADA_ELEMENT_LIBRARY_MATERIALS )
    {}

    //-------------------------------------
    void LibraryMaterials::addInstanceEffect( const URI &effectURL )
    {
        mSW->openElement ( CSWC::COLLADA_ELEMENT_INSTANCE_EFFECT );
        mSW->appendURIAttribute ( CSWC::COLLADA_ATTRIBUTE_URL, effectURL );
        mSW->closeElement();
    }

    //-------------------------------------
    void LibraryMaterials::openMaterial( 
        const String& materialId /*= EMPTY_STRING */, 
        const String& materialName /*= EMPTY_STRING */ )
    {
        openLibrary();

        mMaterialCloser = mSW->openElement ( CSWC::COLLADA_ELEMENT_MATERIAL );

        if ( !materialId.empty() )
            mSW->appendAttribute ( CSWC::COLLADA_ATTRIBUTE_ID, materialId );

        if ( !materialName.empty() )
            mSW->appendAttribute ( CSWC::COLLADA_ATTRIBUTE_NAME, materialName );
    }

    //-------------------------------------
    void LibraryMaterials::closeMaterial()
    {
        mMaterialCloser.close();
    }
} //namespace COLLADA