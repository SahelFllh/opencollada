/*
    Copyright (c) 2008 NetAllied Systems GmbH

	This file is part of COLLADAMax.

    Portions of the code are:
    Copyright (c) 2005-2007 Feeling Software Inc.
    Copyright (c) 2005-2007 Sony Computer Entertainment America
    
    Based on the 3dsMax COLLADA Tools:
    Copyright (c) 2005-2006 Autodesk Media Entertainment
	
    Licensed under the MIT Open Source License, 
    for details please see LICENSE file or the website
    http://www.opensource.org/licenses/mit-license.php
*/


#ifndef __COLLADAMAX_EXPORTNODE_H__
#define __COLLADAMAX_EXPORTNODE_H__

#include "COLLADAMaxPrerequisites.h"

#include <max.h>

#include "COLLADAUtils.h"
#include "COLLADAIDList.h"
#include <map>
#include <vector>

class INode;

class ExportSceneGraph;



namespace COLLADAMax
{


    /** A class to represent a node that should be exported.
    */

    class ExportNode
    {

    public:
        enum Type
        {
            UNDETERMINED,
            UNKNOWN,
            MESH,
            CAMERA,
            LIGHT,
            BONE,
			HELPER,
			MATERIAL
        };

        struct Symbol
        {
            bool used;
            String name;
        };

        /** Maps max materials to COLLADA symbols map<material, symbol>.*/
        typedef std::map<Mtl *, Symbol> MeshSymbolMap;

    private:
        typedef std::vector<ExportNode *> Children;

    private:
        /** Maps max material to symbol name.*/
        MeshSymbolMap mMeshSymbolMap;

        /** Only used if mMeshSymbolMap is empty.*/
        DWORD mWireFrameColor;

        /** The INode this export node wraps.*/
        INode * mINode;

        Children mChildren;

        /** The type of the node.*/
        mutable Type mType;

        /** The unique id of the node.*/
        String mId;

        COLLADA::IDList mIdList;


    public:
        /** Constructor
        @param iNode The INode represented by the Export node. */
        ExportNode ( INode * iNode );

        /** Desrtuctor*/
        virtual ~ExportNode()
        {
            clean();
        }

        /** Returns the unique id of the node.*/
        const String & getId() const
        {
            return mId;
        }

        /** Returns the type of the node.*/
        Type getType() const;

        /** Returns the INode associated with this Export Node.*/
        INode * getINode() const
        {
            return mINode;
        }

        /** Returns the number of child nodes.*/
        size_t getNumberOfChildren() const
        {
            return mChildren.size();
        }

        /** Returns the @a i'th  child.*/
        ExportNode * getChild ( size_t i )
        {
            return mChildren[ i ];
        }

        /** Returns the @a i'th  child.*/
        const ExportNode * const getChild ( size_t i ) const
        {
            return mChildren[ i ];
        }

        /** Determines and returns the type of @a INode.*/
        static Type determineType ( INode * iNode );

        /** Adds channel @a channel with corresponding effect id @a effectId. The symbol us marked as unused.*/
        void addSymbol ( Mtl * material, const String & symbol );

        /** Returns the symbol that corresponds to @a material.*/
        const String& getSymbolByMaterialAndSetAsUsed ( Mtl* material );

        /** Returns the @a MeshSymbolMap of this node.*/
        const MeshSymbolMap & getMeshSymbolMap() const
        {
            return mMeshSymbolMap;
        }


        /** Sets the wire frame color.*/
        void setWireFrameColor ( DWORD color )
        {
            mWireFrameColor = color;
        }

        /** Returns the wire frame color.*/
        DWORD getWireFrameColor() const
        {
            return mWireFrameColor;
        }

        /** Set the unique id of the export node to @a id.
        It is ensured, that the id is a valid id according to the xml spec*/
        const String& setId ( const String& id )
        {
            mId = COLLADA::Utils::checkID ( id );
            return mId;
        }

        /** Removes all children.*/
        void clean();

        /** Adds @a exportNode to its children.*/

        void add
        ( ExportNode * exportNode )
        {
            mChildren.push_back ( exportNode );
        }

    protected:
        ExportNode ( INode * iNode, Type type );

    private:

        /** Determines the type of the export node.*/
        Type determineType() const;
    };

}


#endif //__COLLADAMAX_EXPORTNODE_H__