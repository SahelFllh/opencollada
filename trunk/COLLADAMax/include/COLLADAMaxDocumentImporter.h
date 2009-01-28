/*
Copyright (c) 2008 NetAllied Systems GmbH

This file is part of COLLADAMax.

Portions of the code are:
Copyright (c) 2005-2007 Feeling Software Inc.
Copyright (c) 2005-2007 Sony Computer Entertainment America

Based on the 3dsMax COLLADASW Tools:
Copyright (c) 2005-2006 Autodesk Media Entertainment

Licensed under the MIT Open Source License, 
for details please see LICENSE file or the website
http://www.opensource.org/licenses/mit-license.php
*/

#ifndef __COLLADAMAX_DOCUMENTIMPORTER_H__
#define __COLLADAMAX_DOCUMENTIMPORTER_H__

#include "COLLADAMaxPrerequisites.h"

#include "COLLADAFWIWriter.h"
#include <list>

//#include "dummy.h"


class Interface;
class ImpInterface;
class DummyObject;

namespace COLLADAFW
{
	class VisualScene;
	class Geometry;
	class UniqueId;
	class LibraryNodes;
	class Node;
}

namespace COLLADAMax
{
	class DocumentImporter 	: COLLADAFW::IWriter
	{
	public:
		/** Maps Unique id to INodes.*/
		typedef std::multimap<COLLADAFW::UniqueId, INode*> UniqueIdINodeMultiMap;

		/** Maps Unique id to ImpNodes.*/
		typedef std::multimap<COLLADAFW::UniqueId, ImpNode*> UniqueIdImpNodeMultiMap;

		/** Maps Unique id to INodes.*/
		typedef std::map<COLLADAFW::UniqueId, INode*> UniqueIdINodeMap;

		/** Maps Unique id to Objects.*/
		typedef std::map<COLLADAFW::UniqueId, Object*> UniqueIdObjectMap;

		/** Maps objects to Unique id to.*/
		typedef std::map< INode*, COLLADAFW::UniqueId> ObjectINodeUniqueIdMap;

		/** List of library nodes.*/
		typedef std::list<const COLLADAFW::LibraryNodes*> LibraryNodesList;

		/** Maps Unique id to framework nodes.*/
		typedef std::map<COLLADAFW::UniqueId, const COLLADAFW::Node*> UniqueIdFWNodeMap;

	private:
		/** Max interface.*/
		Interface* mMaxInterface;

		/** Max import interface.*/
		ImpInterface* mMaxImportInterface;

		/** File path of the COLLADA document to import.*/
		NativeString mImportFilePath;

		/** A dummy helper, that is used for nodes that do not have an object assigned to.*/
		DummyObject* mDummyObject;
		
		/** Maps the unique ids of objects (geometries, controllers,...) that are referenced by INodes to 
		these referencing INodes. This map is being filled while importing the visual scene. It is 
		required for objects that are referenced before they are imported.*/
		UniqueIdINodeMultiMap mUniqueIdObjectINodeMap;

		/** Maps the unique ids of objects (geometries, controllers,...) to the created max object. This map 
		is being filled while importing geometries. */
		UniqueIdObjectMap mUniqueIdObjectMap;

		/** Maps the unique ids of nodes to the created max INode. This map is being filled while importing 
		the visual scene. It is required when ever nodes are referenced, after they have been imported.*/
		UniqueIdINodeMap mUniqueIdINodeMap;

		/** Maps the unique ids of nodes that are instantiated to the created instantiating max INode. This 
		map is being filled while importing the visual scene. It is required when ever nodes are referenced, 
		before they have been imported.*/
		UniqueIdImpNodeMultiMap mUniqueIdReferencingImpNodeMap;

		/** Maps each already imported object to its Unique id. When ever a new object is created it 
		should be added to this map. .*/
		ObjectINodeUniqueIdMap mObjectINodeUniqueIdMap;

		/** Maps Unique ids of nodes that are in a library nodes but not already in the max scene graph to
		the frame word node itself. Each frame work node, that is in a library nodes but not immediately added 
		to the max scene graph should be added to that map. When looking for referenced nodes check this map if
		the referenced node is not already in the max scene graph.*/
		UniqueIdFWNodeMap mUniqueIdFWNodeMap;

		/** This vector contains all library nodes already received by the importer.*/
		LibraryNodesList mLibraryNodesList;

	public:
		/** Constructor .
		@param maxInterface The max interface.
		@param the file name of the file to import.*/
		DocumentImporter(Interface * maxInterface, ImpInterface* maxImportInterface, const NativeString &filepath);

		virtual ~DocumentImporter();

		/** Start the import of the model.
		@return True on success, false otherwise. */
		bool import();

		/** Returns the max interface.*/
		Interface* getMaxInterface() { return mMaxInterface; }

		/** Returns the max interface.*/
		ImpInterface* getMaxImportInterface() { return mMaxImportInterface; }

		/** Deletes the entire scene.
		@param errorMessage A message containing informations about the error that occurred.
		*/
		void cancel(const String& errorMessage){};

		/** Prepare to receive data.*/
		void start(){};

		/** Remove all objects that don't have an object. Deletes unused visual scenes.*/
		void finish(){};

		/** Writes the entire visual scene.
		@return True on succeeded, false otherwise.*/
		bool writeVisualScene ( const COLLADAFW::VisualScene* visualScene );

		/** Handles all nodes in the library nodes.
		@return True on succeeded, false otherwise.*/
		bool writeLibraryNodes( const COLLADAFW::LibraryNodes* libraryNodes );

		/** Writes the geometry.
		@return True on succeeded, false otherwise.*/
		bool writeGeometry ( const COLLADAFW::Geometry* geometry );

	
	private:
        /** Disable default copy ctor. */
		DocumentImporter( const DocumentImporter& pre );
        /** Disable default assignment operator. */
		const DocumentImporter& operator= ( const DocumentImporter& pre );

		/** Returns the dummy object used for nodes that do not have an object assigned to.*/
		DummyObject* getDummyObject(){ return mDummyObject; }

		/** Returns the UniqueId Object INode Mapping.*/
		UniqueIdINodeMultiMap& getUniqueIdObjectINodeMap(){ return mUniqueIdObjectINodeMap; }

		/** Returns the UniqueId Referencing INode Mapping.*/
		UniqueIdImpNodeMultiMap& getUniqueIdReferencingImpNodeMap(){ return mUniqueIdReferencingImpNodeMap; }

		/** Returns the UniqueId object Mapping.*/
		UniqueIdObjectMap& getUniqueIdObjectMap(){ return mUniqueIdObjectMap; }

		/** Returns the UniqueId INode Mapping.*/
		UniqueIdINodeMap& getUniqueIdINodeMap(){ return mUniqueIdINodeMap; }

		/** Returns the object UniqueId Mapping.*/
		ObjectINodeUniqueIdMap& getObjectINodeUniqueIdMap(){ return mObjectINodeUniqueIdMap; }

		/** Returns the Unique id framework node map.*/
		UniqueIdFWNodeMap& getUniqueIdFWNodeMap() { return mUniqueIdFWNodeMap; }

		/** Returns the list of library nodes.*/
		LibraryNodesList& getLibraryNodesList(){ return mLibraryNodesList; }

		friend class ImporterBase;

	};

} // namespace COLLADAMAX

#endif // __COLLADAMAX_DOCUMENTIMPORTER_H__