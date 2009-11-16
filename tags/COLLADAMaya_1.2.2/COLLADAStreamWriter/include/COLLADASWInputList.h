/*
    Copyright (c) 2008-2009 NetAllied Systems GmbH

	This file is part of COLLADAStreamWriter.

    Licensed under the MIT Open Source License,
    for details please see LICENSE file or the website
    http://www.opensource.org/licenses/mit-license.php
*/

#ifndef __COLLADASTREAMWRITER_INPUT_LIST_H__
#define __COLLADASTREAMWRITER_INPUT_LIST_H__

#include "COLLADASWPrerequisites.h"
#include "COLLADASWElementWriter.h"
#include "COLLADASWBaseElement.h"
#include "COLLADASWConstants.h"

#include "COLLADABUURI.h"

namespace COLLADASW
{

    class InputList;
    class PrimitivesBase;
    class BaseInputElement;
    class Vertices;
	class ControlVertices;

    /** The geometry source data types. */
    enum Semantics
    {
        POSITION = 0, /**< Used for position data. */
        VERTEX, /**< Used to input the vertex sources within a polygon set. */
        NORMAL, /**< Used for surface normal vectors. */
		TANGENT, /**< Used for surface tangent vectors. */
		IN_TANGENT, /**< Used for in tangents for splines. */
		OUT_TANGENT, /**< Used for out tangents for splines. */
		INTERPOLATION, /**< Used for interpolations for splines. */
		BINORMAL, /**< Used for surface binormal vectors. */
        TEXCOORD, /**< Used for texture coordinates. */
        TEXTANGENT, /**< Used for texture-aligned surface tangent vectors. */
        TEXBINORMAL, /**< Used for texture-aligned surface binormal vectors. */
        UV, /**< Used for generic mapping parameters. Do not confuse this with TEXCOORD. */
        COLOR, /**< Used for vertex colors. */
        EXTRA, /**< Used for application-specific vertex data. This is Maya-specific to support blind data. */

        POINT_SIZE, /**< Used to define the size of the Billboard generated by a given point.
     This is a type::POINTS specific type.  If the size is 0, a single pixel pt is rendered. */
        POINT_ROTATION, /**< Used to give a billboard orientation.  This is a type::POINTS
      specific type, and is not compulsory */
        JOINT, /** Used for the skin controller element. */
        BINDMATRIX, /** Used for the skin controller element. */
        WEIGHT, /** Used for the skin controller element. */
        MORPH_TARGET, /** Used for the morph controller element. */
        MORPH_WEIGHT, /** Used for the morph controller element. */
        UNKNOWN = -1 /**< An unknown data source. */
    };

    /** A class that holds information about an @a \<input\> element.*/
    class Input
    {

    public:
        /** Constructor
        @param semantic The semantic of the @a \<input\> element.
        @param source The source of the @a \<input\> element.
        @param offset The offset of the @a \<input\> element.
        @param set The set of the @a \<input\> element.
        */
        Input ( Semantics semantic, const URI& source, int offset = -1, int set = -1 )
                : mSemantic ( semantic ),
                mSource ( source ),
                mOffset ( offset ),
                mSet ( set ) {}

        virtual ~Input() {}

        /** Returns the semantic of the Input*/
        Semantics getSemantic() const
        {
            return mSemantic;
        }

        /** Returns the source of the Input*/
        const COLLADABU::URI& getSource() const
        {
            return mSource;
        }

        /** Returns the offset of the Input*/
        int getOffset() const
        {
            return mOffset;
        }

        /** Returns the set of the Input*/
        int getSet() const
        {
            return mSet;
        }

    private:
        Semantics mSemantic;
        COLLADABU::URI mSource;
        int mOffset;
        int mSet;
    };


    /** A class that writes a list of Input objects to the stream.*/
    class InputList : public ElementWriter
    {

    private:

        /** Declare friend, so the class can use the 'add()' method. */
        friend class PrimitivesBase;

        /** Declare friend, so the class can use the 'add()' method. */
        friend class BaseInputElement;

		/** Declare friend, so the class can use the 'add()' method. */
		friend class Vertices;

		/** Declare friend, so the class can use the 'add()' method. */
		friend class ControlVertices;

        /** List of all the inputs*/
        typedef std::list<Input> List;
        List mList;

    public:

        /** Constructor
        @param The stream the InputList should be written to*/
        InputList ( StreamWriter* streamWriter ) : ElementWriter ( streamWriter ) {}

        /** Destructor*/
        virtual ~InputList() {}

        /** Adds @a input to list of inputs that should be added*/
        void push_back ( Input input )
        {
            mList.push_back ( input );
        }

		/** Returns a string containing the semantic name*/
		static const String& getSemanticString ( Semantics semantic );

	private:

        /** Add all the inputs, added using push_back(), to the stream*/
        void add();


    };


} //namespace COLLADASW



#endif //__COLLADASTREAMWRITER_INPUT_LIST_H__
