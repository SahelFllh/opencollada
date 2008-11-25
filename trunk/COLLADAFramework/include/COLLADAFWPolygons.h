/*
    Copyright (c) 2008 NetAllied Systems GmbH

	This file is part of COLLADAFramework.
	
    Licensed under the MIT Open Source License, 
    for details please see LICENSE file or the website
    http://www.opensource.org/licenses/mit-license.php
*/

#ifndef __COLLADAFW_POLYGONS_H__
#define __COLLADAFW_POLYGONS_H__

#include "COLLADAFWPolyBase.h"
#include "COLLADAFWPrimitiveBase.h"
#include "COLLADAFWPHElement.h"


namespace COLLADAFW
{

    /**
     * The polygons element provides the information needed to bind vertex attributes
     * together and  then organize those vertices into individual polygons. The
     * polygons described can contain  arbitrary numbers of vertices. These polygons
     * may be self intersecting and may also contain holes.
     * 
     * The <polygons> element declares the binding of geometric primitives and vertex attributes for a 
     * <mesh> element.
     * Note: Polygons are not the preferred way of storing data. Use <triangles> or <polylist> for the
     * most efficient representation of geometry. Use <polygons> only if holes are needed, and even 
     * then, only for the specific portions with holes.
     * The vertex array information is supplied in distinct attribute arrays of the <mesh> element that 
     * are then indexed by the <polygons> element.
     * The polygons described can contain arbitrary numbers of vertices. Ideally, they would describe 
     * convex shapes, but they also may be concave or even self-intersecting. The polygons may also 
     * contain holes.
     * Polygon primitives that contain four or more vertices may be non-planar as well.
     * Many operations need an exact orientation of a surface point. The normal vector partially 
     * defines this orientation, but it is still leaves the �rotation� about the normal itself 
     * ambiguous. One way to �lock down� this extra rotation is to also specify the surface tangent at 
     * the same point. Assuming that the type of the coordinate system is known (for example, 
     * right-handed), this fully specifies the orientation of the surface, meaning that we can define 
     * a 3x3 matrix to transforms between objectspace and surface space.
     * The tangent and the normal specify two axes of the surface coordinate system (two columns of the 
     * matrix) and the third one, called binormal may be computed as the cross-product of the tangent 
     * and the normal. COLLADA supports two different types of tangents, because they have different 
     * applications and different logical placements in a document:
     * � texture-space tangents: specified with the TEXTANGENT and TEXBINORMAL semantics and the
     *   set attribute on the <input> (shared) elements
     * � standard (geometric) tangents: specified with the TANGENT and BINORMAL semantics on the
     *   <input> (shared) elements
     */
    class Polygons : public PolyBase
    {

    private:

        /**
        * Contains a array of integers that specify the vertex attributes
        * (indices) for an individual Polylist ("p" stands for "primitive").
        * The p element may occur not or more. 
        */
        PArray mPArray;

        /** 
        * Contains the number of primitives in the @mPHArray array. 
        */
        size_t mPArraySize;

        /**
        * Contains a array of integers that specify the vertex attributes
        * (indices) for an individual Polylist ("p" stands for "primitive").
        * The p element may occur not or more. 
        */
        PHArray mPHArray;
        
        /** 
        * Contains the number of primitives in the @mPHArray array. 
        */
        size_t mPHArraySize;

    public:

        /**
        * Constructor
        */
        Polygons () 
            : PolyBase ()
        {}

        /**
        * Destructor
        */
        virtual ~Polygons() {}

        /**
        * Contains a array of integers that specify the vertex attributes
        * (indices) for an individual Polylist ("p" stands for "primitive").
        * The p element may occur not or more. 
        * @return PArray& Reference to the ph array.
        */
        PArray& getPArray () 
        { 
            return mPArray; 
        }

        /**
        * Contains a array of integers that specify the vertex attributes
        * (indices) for an individual Polylist ("p" stands for "primitive").
        * The p element may occur not or more. 
        * @return PArray& Const reference to the ph array.
        */
        const PArray& getPArray () const 
        { 
            return mPArray; 
        }

        /**
        * Contains a array of integers that specify the vertex attributes
        * (indices) for an individual Polylist ("p" stands for "primitive").
        * The p element may occur not or more. 
        * @param pArray Reference to the ph array.
        * @param pArraySize The parameter to get the size of the returned array.
        */
        void setPArray ( const PArray& pArray ) 
        { 
            mPArray = pArray; 
        }

        /**
        * Contains a array of integers that specify the vertex attributes
        * (indices) for an individual Polylist ("p" stands for "primitive").
        * The p element may occur not or more. 
        * @return PHArray& Reference to the ph array.
        */
        PHArray& getPHArray () 
        { 
            return mPHArray; 
        }

        /**
        * Contains a array of integers that specify the vertex attributes
        * (indices) for an individual Polylist ("p" stands for "primitive").
        * The p element may occur not or more. 
        * @return PHArray& Const reference to the ph array.
        */
        const PHArray& getPHArray () const 
        { 
            return mPHArray; 
        }

        /**
        * Contains a array of integers that specify the vertex attributes
        * (indices) for an individual Polylist ("p" stands for "primitive").
        * The p element may occur not or more. 
        * @param phArray Reference to the ph array.
        */
        void setPHArray ( const PHArray& phArray ) 
        { 
            mPHArray = phArray; 
        }

    };

    /**
     * Array of polygons elements.
     */
    typedef Array<Polygons> PolygonsArray;
}

#endif // __COLLADAFW_POLYGONS_H__
