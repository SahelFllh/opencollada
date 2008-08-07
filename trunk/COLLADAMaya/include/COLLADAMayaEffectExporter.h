/*
    Copyright (c) 2008 NetAllied Systems GmbH

	This file is part of COLLADAMaya.

    Portions of the code are:
    Copyright (c) 2005-2007 Feeling Software Inc.
    Copyright (c) 2005-2007 Sony Computer Entertainment America
    Copyright (c) 2004-2005 Alias Systems Corp.
	
    Licensed under the MIT Open Source License, 
    for details please see LICENSE file or the website
    http://www.opensource.org/licenses/mit-license.php
*/

#ifndef __COLLADA_MAYA_EFFECT_EXPORTER_H__
#define __COLLADA_MAYA_EFFECT_EXPORTER_H__

#include "COLLADAMayaStableHeaders.h"
#include "COLLADAMayaDocumentExporter.h"
#include "COLLADAMayaSceneElement.h"
#include "COLLADAMayaMaterialExporter.h"
#include "COLLADAMayaEffectTextureExporter.h"
#include <map>

#include "COLLADAStreamWriter.h"
#include "COLLADAEffectProfile.h"
#include "COLLADALibraryEffects.h"
#include "COLLADAIDList.h"

#include <maya/MObject.h>

namespace COLLADAMaya
{

#ifdef BLINN_EXPONENT_MODEL

    class BlinnEccentricityToShininess : public ConversionFunctor
    {

    public:
        virtual float operator() ( float v )
        {
            return 100.0f - 99.0f * sqrtf ( v );
        }
    };

    class BlinnShininessToEccentricity : public ConversionFunctor
    {

    public:
        virtual float operator() ( float v )
        {
            float out = ( 100.0f - v ) / 99.0f;
            return out * out;
        }
    };

#endif // BLINN_EXPONENT_MODEL


    class EffectTextureExporter;

    /** Map to store effects. */
    typedef std::map<String, MObject*> EffectMap;

    /** This class writes the <library_materials>. It uses informations created my an effect exporter*/

    class EffectExporter : public COLLADA::LibraryEffects
    {

    public:

        /** Suffix for the effect. */
        static const String EFFECT_ID_SUFFIX;

    private:

        /** Prefix for the color effect. */
        static const String COLOR_EFFECT_ID_PREFIX;

        /** Suffix for to build the tex-coordinate id. */
        static const String TEXCOORD_BASE;

        /** Holds information about an already exported effect*/

        struct ExportedEffect
        {
            MObject* shader; // The shader object, that was use to export the effect
            DWORD mColor;  // The color that was used to export the effect
        };

        /** Pointer to the document exporter */
        DocumentExporter* mDocumentExporter;

        /** For export the textures */
        EffectTextureExporter mTextureExporter;

        /** maps already exported materials to the corresponding collada effect id */
        MaterialMap* mMaterialMap;

        /** The std::map which stores all already exported effects */
        EffectMap mExportedEffectMap;

        /** List of unique base effect ids*/
        COLLADA::IDList mEffectIdList;

        /** A texture channel.
        Used by standard effects to assign textures to channels.
        Multi-texturing is done by assigning more than one texture per slot.
        Defaults to diffuse texture slot. */
        enum Channel
        {
            AMBIENT = 0, /**< The texels will be modulated with the ambient light colors. */
            BUMP, /**< The texels will re-orient the geometric normals. */
            DIFFUSE, /**< The texels will be modulated with the non-ambient light colors. */
            DISPLACEMENT, /**< The texels will displace the pixel positions. */
            EMISSION, /**< The texels will be added to the final color directly. */
            FILTER, /**< Max-specific. */
            REFLECTION, /**< The texels will modify the pixel reflection factor. */
            REFRACTION, /**< The texels will modify the pixel refraction factor. */
            SHININESS, /**< The texels will modify the specular shininess of the pixel. */
            SPECULAR, /**< The texels will be modulated with the specular light colors. */
            SPECULAR_LEVEL, /**< The texels will be modulated with the specular light colors. */
            TRANSPARENt, /**< The texels will be modify the final color alpha. */
            COUNT, /**< The number of texture channels. */
            UNKNOWN, /**< An unknown texture channel. */
            DEFAULT = DIFFUSE
        };

    public:
        /**
        @param streamWriter The stream the output will be written to
        @paramdocumentExporter The document exporter this material exporter is used in*/
        EffectExporter ( COLLADA::StreamWriter* _streamWriter, DocumentExporter* _documentExporter );
        virtual ~EffectExporter() {}

        /** Gets all the shaders and exports the effects of each */
        const ImageMap* exportEffects ( MaterialMap* materialMap=NULL );

        /**
         * Exports the effects from the scene graph.
         */
        void exportEffectsBySceneGraph();

        /**
        * Returns the std::map with the exported images.
        * @return const ImageMap*
        *   Returns a const pointer to the image map, managed and released
        *   in the class @EffectTextureExporter
        */
        const ImageMap* getExportedImageMap() const
        {
            return mTextureExporter.getExportedImageMap();
        }

    private:

        /** Exports the effects of a shader */
        void exportEffect ( MObject& shadingEngine );

        /** Export a shader, by type */
        void exportConstantShader ( COLLADA::EffectProfile* effectProfile,
                                    MObject shadingNetwork );

        /** Exports an standard shader */
        void exportStandardShader ( COLLADA::EffectProfile* effectProfile,
                                    MObject shadingNetwork,
                                    bool initialized=false );

        /** Export a shader, by type */
        void exportColladaFXShader ( COLLADA::EffectProfile* effectProfile,
                                     MObject shadingNetwork );

        /**
         * Find any textures connected to a material attribute and create the
         * associated texture elements.
         * @param node The maya node object.
         * @param attributeName The name of the attribute
         * @param effectProfile The collada effect profile.
         * @param channel The channel to export.
         * @param nextTextureIndex The texture index
         * @return MObject If exported, the texture element, otherwise NULL.
         */
        MObject exportTexturedParameter ( const MObject& node,
                                          const char* attributeName,
                                          COLLADA::EffectProfile* effectProfile,
                                          EffectExporter::Channel channel,
                                          int& nextTextureIndex );

        /**
         * Retrieve any texture (file or layered) associated with a material attribute.
         * @param shader The maya shader object.
         * @param attributeName The name of the attribute.
         * @param textures Array of textures.
         * @param blendModes Array of blend modes.
         */
        void getShaderTextures ( const MObject& shader,
                                 const char* attributeName,
                                 MObjectArray& textures,
                                 MIntArray& blendModes );

        /**
         * Exports the transparency.
         * @param shadingNetwork Maya object to export.
         * @param transparentColor Color to export.
         * @param effectProfile Pointer to the collada effect profile.
         * @param attributeName Name of the attribute.
         * @param nextTextureIndex Index of the texture.
         */
        void exportTransparency ( MObject shadingNetwork,
                                  const MColor& transparentColor,
                                  COLLADA::EffectProfile* effectProfile,
                                  const char* attributeName,
                                  int& nextTextureIndex );

        /** Blends the color on colorOrTexture with blendColor an amount @a amount*/
        void blendColor ( COLLADA::ColorOrTexture &colorOrTexture, COLLADA::Color blendColor, double ammount );

        /** Converts a maya color to a common ColorOrTexture type*/
        COLLADA::ColorOrTexture mayaColor2ColorOrTexture ( const MColor &color, double scale=1.0 );

        /** Exports all materials from the current mesh */
        void exportMeshEffects ( SceneElement* sceneElement );

    };

}

#endif //__COLLADA_MAYA_EFFECT_EXPORTER_H__