/*
    Copyright (c) 2008 NetAllied Systems GmbH

    This file is part of COLLADAStreamWriter.

    Licensed under the MIT Open Source License, 
    for details please see LICENSE file or the website
    http://www.opensource.org/licenses/mit-license.php
*/
#include "COLLADAInstanceMaterial.h"
#include "COLLADASWC.h"

namespace COLLADA
{

    //---------------------------------------------------------------
    void InstanceMaterialList::add()
    {
        for ( List::iterator it = mList.begin(); it != mList.end(); ++it )
        {
            mSW->openElement ( CSWC::COLLADA_ELEMENT_INSTANCE_MATERIAL );
            mSW->appendAttribute ( CSWC::COLLADA_ATTRIBUTE_SYMBOL, it->getSymbol() );
            mSW->appendURLAttribute ( CSWC::COLLADA_ATTRIBUTE_TARGET, it->getTarget() );
            mSW->closeElement();
        }
    }

} //namespace COLLADA