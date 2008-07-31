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
#include "COLLADAMayaStableHeaders.h"
#include "COLLADAMayaEffectTextureExporter.h"
#include "COLLADAMayaShaderHelper.h"
#include "COLLADAMayaConvert.h"
#include "COLLADAMayaSyntax.h"
#include "COLLADAMayaAnimationExporter.h"
#include "COLLADAMayaExportOptions.h"

#include "COLLADAMathUtils.h"
#include "COLLADAStreamWriter.h"
#include "COLLADALibraryImages.h"

#include <maya/MFileIO.h>

namespace COLLADAMaya
{

    const String EffectTextureExporter::FORMAT = "A8R8G8B8";

    //------------------------------------------------------
    EffectTextureExporter::EffectTextureExporter ( DocumentExporter* _documentExporter )
            : mDocumentExporter ( _documentExporter ),
            techniqueIsOpen ( false ),
            mExtraSource ( NULL ),
            mTechniqueSource ( NULL )
    {}

    //---------------------------------------------------------------
    EffectTextureExporter::~EffectTextureExporter()
    {
        if ( !mExportedImageMap.empty() )
        {
            ImageMap::iterator it = mExportedImageMap.begin();
            for ( ; it!=mExportedImageMap.end(); ++it )
            {
                COLLADA::Image* image = it->second;
                delete image;
            }

            mExportedImageMap.clear();
        }
    }

    //---------------------------------------------------------------
    void EffectTextureExporter::exportTexture ( 
        COLLADA::Texture* colladaTexture,
        String channelSemantic,
        const MObject& texture,
        int blendMode,
        int textureIndex )
    {
        String colladaImageName = exportImage ( texture );

        colladaTexture->setImageId ( colladaImageName );
        colladaTexture->setSurfaceType ( COLLADA::Texture::SURFACE_TYPE_2D );
        colladaTexture->setTexcoord ( channelSemantic );
        colladaTexture->setFormat ( FORMAT );

        // Add 2D placement parameters
        add2DPlacement ( colladaTexture, texture );

        // Check for 3D projection node
        MObject colorReceiver = DagHelper::getSourceNodeConnectedTo ( texture, ATTR_OUT_COLOR );
        if ( colorReceiver != MObject::kNullObj && colorReceiver.apiType() == MFn::kProjection )
        {
            add3DProjection ( colladaTexture, colorReceiver );
        }

        // Add blend mode information
        String blendModeString = getBlendMode ( blendMode );

        colladaTexture->addExtraTechniqueParameter ( 
            MAYA_PROFILE, MAYA_TEXTURE_BLENDMODE_PARAMETER, blendModeString );

        // Wrap elements
        switch ( colladaTexture->getSurfaceType() )
        {

        case COLLADA::Texture::SURFACE_TYPE_1D:
        {
            colladaTexture->setWrapS ( COLLADA::Texture::WRAP_MODE_WRAP );
        }

        break;

        case COLLADA::Texture::SURFACE_TYPE_2D:
        {
            colladaTexture->setWrapS ( COLLADA::Texture::WRAP_MODE_WRAP );
            colladaTexture->setWrapT ( COLLADA::Texture::WRAP_MODE_WRAP );
        }

        break;

        case COLLADA::Texture::SURFACE_TYPE_3D:

        case COLLADA::Texture::SURFACE_TYPE_CUBE:
        {
            colladaTexture->setWrapS ( COLLADA::Texture::WRAP_MODE_WRAP );
            colladaTexture->setWrapT ( COLLADA::Texture::WRAP_MODE_WRAP );
            colladaTexture->setWrapP ( COLLADA::Texture::WRAP_MODE_WRAP );
        }

        break;
        }

        colladaTexture->setMinFilter ( COLLADA::Texture::SAMPLER_FILTER_NONE );

        colladaTexture->setMagFilter ( COLLADA::Texture::SAMPLER_FILTER_NONE );
        colladaTexture->setMipFilter ( COLLADA::Texture::SAMPLER_FILTER_NONE );
    }

    //---------------------------------------------------------------
    String EffectTextureExporter::getBlendMode ( int blendMode )
    {
        switch ( blendMode )
        {

        case NONE:
            return MAYA_NONE_BLENDMODE;

        case OVER:
            return MAYA_OVER_BLENDMODE;

        case In:
            return MAYA_IN_BLENDMODE;

        case OUt:
            return MAYA_OUT_BLENDMODE;

        case ADD:
            return MAYA_ADD_BLENDMODE;

        case SUBTRACT:
            return MAYA_SUBTRACT_BLENDMODE;

        case MULTIPLY:
            return MAYA_MULTIPLY_BLENDMODE;

        case DIFFERENCe:
            return MAYA_DIFFERENCE_BLENDMODE;

        case LIGHTEN:
            return MAYA_LIGHTEN_BLENDMODE;

        case DARKEN:
            return MAYA_DARKEN_BLENDMODE;

        case SATURATE:
            return MAYA_SATURATE_BLENDMODE;

        case DESATURATE:
            return MAYA_DESATURATE_BLENDMODE;

        case ILLUMINATE:
            return MAYA_ILLUMINATE_BLENDMODE;

        default:
            return MAYA_NONE_BLENDMODE;
        }
    }

    //---------------------------------------------------------------
    // Export an image
    String EffectTextureExporter::exportImage ( const MObject &texture )
    {
        // Retrieve the texture filename
        MFnDependencyNode dgFn ( texture );
        MString mayaName = dgFn.name(), mayaFileName;
        MPlug filenamePlug = dgFn.findPlug ( ATTR_FILE_TEXTURE_NAME );

        // Get the maya filename with the path to the file.
        filenamePlug.getValue ( mayaFileName );
        if ( mayaFileName.length() == 0 ) return NULL;
        String sourceFile = mayaFileName.asChar ();

        // Get the file name and the URI
        String fullFileName;
        String fullFileNameURI;

        if ( ExportOptions::relativePaths() )
        {
            // Different filename and URI, if we copy the textures to the destination directory!
            if ( ExportOptions::copyTexturesToDestinationDirectory() )
            {
                // TODO In the moment, we don't create sub folders in the destination directory
                // for the texture. Instead, we copy them directly in the destination folder.

                // Get the filename with the path to the destination directory.
                String targetFile = getTargetFileName( sourceFile );
                String targetColladaFile = mDocumentExporter->getFilename();
                String targetPath = COLLADA::Utils::getAbsolutePathFromFile ( targetColladaFile );
                String relativeFileName = COLLADA::Utils::getRelativeFilename( targetPath, targetFile );

                // Get the filename and the URI
                fullFileName = relativeFileName;
                if ( relativeFileName[0] != '.' )
                    fullFileNameURI = "." + COLLADA::Utils::FILE_DELIMITER + relativeFileName;
                else
                    fullFileNameURI = relativeFileName;
            }
            else
            {
                // Get the relative file path from the destination folder to the source texture
                String targetFile = mDocumentExporter->getFilename();
                String targetPath = COLLADA::Utils::getAbsolutePathFromFile ( targetFile );

//                 String sourceMayaFile = MFileIO::currentFile().asChar();
//                 String sourceMayaPath = COLLADA::Utils::getAbsolutePathFromFile ( sourceMayaFile );

                String relativeFileName = COLLADA::Utils::getRelativeFilename( targetFile, sourceFile );

                // Get the filename and the URI
                fullFileName = relativeFileName;
                if ( relativeFileName[0] != '.' )
                    fullFileNameURI = "." + COLLADA::Utils::FILE_DELIMITER + relativeFileName;
                else
                    fullFileNameURI = relativeFileName;
            }
        }
        else
        {
            // Different filename and URI, if we copy the textures to the destination directory!
            if ( ExportOptions::copyTexturesToDestinationDirectory() )
            {
                // TODO In the moment, we don't create sub folders in the destination directory
                // for the texture. Instead, we copy them directly in the destination folder.

                // Get the filename with the path to the destination directory.
                String targetFile = getTargetFileName( sourceFile );

                // Get the filename and the URI
                fullFileName = targetFile;
                fullFileNameURI = COLLADA::Utils::FILE_PROTOCOL + COLLADA::Utils::UriEncode ( targetFile );
            }
            else
            {
                // Get the filename and the URI
                fullFileName = sourceFile;
                fullFileNameURI = COLLADA::Utils::FILE_PROTOCOL + COLLADA::Utils::UriEncode ( sourceFile );
            }
        }

        // Have we seen this texture node before?
        ImageMap::iterator exportedImagesIter = mExportedImageMap.find ( fullFileName );
        if ( exportedImagesIter != mExportedImageMap.end() )
        {
            COLLADA::Image* colladaImage = ( *exportedImagesIter ).second;
            return colladaImage->getImageId();
        }

        // Check, if we should copy the texture to the destination folder.
        if ( ExportOptions::copyTexturesToDestinationDirectory() )
        {
            copyTexturesToDestination( sourceFile );
        }

        // Convert the image name
        String colladaImageName = mDocumentExporter->mayaNameToColladaName ( mayaName );

        // Create a new image structure
        COLLADA::Image* colladaImage = new COLLADA::Image ( 
            fullFileNameURI, colladaImageName, colladaImageName );

        // Export the node type, because PSD textures don't behave the same as File textures.
        String nodeType = texture.hasFn ( MFn::kPsdFileTexture ) ? MAYA_TEXTURE_PSDTEXTURE : MAYA_TEXTURE_FILETEXTURE;
        colladaImage->addExtraTechniqueParameter ( MAYA_PROFILE, MAYA_TEXTURE_NODETYPE, nodeType );

        // Export whether this image is in fact an image sequence
        MPlug imgSeqPlug = dgFn.findPlug ( ATTR_IMAGE_SEQUENCE );
        bool isImgSeq = false;
        imgSeqPlug.getValue ( isImgSeq );
        colladaImage->addExtraTechniqueParameter ( MAYA_PROFILE, MAYA_TEXTURE_IMAGE_SEQUENCE, isImgSeq );

        // Add this texture to our list of exported images
        mExportedImageMap[ fullFileName ] = colladaImage;

        return colladaImageName;
    }

    // ------------------------------------------------------------
    void EffectTextureExporter::copyTexturesToDestination( String &sourceFile )
    {
        // Get the target file from source file.
        String targetFile = getTargetFileName( sourceFile );

        // TODO If the image is in a sub-directory, we have to create 
        // the sub-directories before copying the file!

        // Copy the source file to the destination directory
        if ( !COLLADA::Utils::copyFile( sourceFile, targetFile ) )
        {
            String message = "Couldn't successful copy the texture from \""
                            + sourceFile + "\" to \"" + targetFile + "\"!";
            MGlobal::displayError( message.c_str() );
        }
    }

    // ------------------------------------------------------------
    String EffectTextureExporter::getTargetFileName( String &sourceFile )
    {
        // TODO In the moment, we don't create sub folders in the destination directory
        // for the texture. Instead, we copy them directly in the destination folder.

        // Target file
        String targetFile = mDocumentExporter->getFilename();
        String targetPath = COLLADA::Utils::getAbsolutePathFromFile ( targetFile );

        // Get the file path and name
        String filePathString = COLLADA::Utils::getAbsolutePathFromFile ( sourceFile );
        String fileNameWithoutPath = COLLADA::Utils::getFileNameFromFile( sourceFile );

        // Get the path to the maya source file
        String mayaSourceFile = MFileIO::currentFile().asChar();
        String mayaSourcePath = COLLADA::Utils::getAbsolutePathFromFile ( mayaSourceFile );

        // Get the relative file name
        String relativeFileName = COLLADA::Utils::getRelativeFilename( mayaSourcePath, sourceFile );

        // Generate the target file name
        return targetPath + relativeFileName;
    }

    // ------------------------------------------------------------
    // Helper to dump a place2dTexture node
    void EffectTextureExporter::add2DPlacement ( COLLADA::Texture* colladaTexture, MObject texture )
    {
        // Is there a texture placement applied to this texture?
        MObject placementNode = DagHelper::getSourceNodeConnectedTo ( texture, ATTR_UV_COORD );
        if ( placementNode.hasFn ( MFn::kPlace2dTexture ) )
        {
            MFnDependencyNode placement2d ( placementNode );

            addBoolParameter ( colladaTexture, MAYA_TEXTURE_WRAPU_PARAMETER, placement2d );
            addBoolParameter ( colladaTexture, MAYA_TEXTURE_WRAPV_PARAMETER, placement2d );
            addBoolParameter ( colladaTexture, MAYA_TEXTURE_MIRRORU_PARAMETER, placement2d );
            addBoolParameter ( colladaTexture, MAYA_TEXTURE_MIRRORV_PARAMETER, placement2d );

            addFloatParameter ( colladaTexture, MAYA_TEXTURE_COVERAGEU_PARAMETER, placement2d );
            addFloatParameter ( colladaTexture, MAYA_TEXTURE_COVERAGEV_PARAMETER, placement2d );
            addFloatParameter ( colladaTexture, MAYA_TEXTURE_TRANSFRAMEU_PARAMETER, placement2d );
            addFloatParameter ( colladaTexture, MAYA_TEXTURE_TRANSFRAMEV_PARAMETER, placement2d );
            addAngleParameter ( colladaTexture, MAYA_TEXTURE_ROTFRAME_PARAMETER, placement2d );

            addBoolParameter ( colladaTexture, MAYA_TEXTURE_STAGGER_PARAMETER, placement2d );
            addBoolParameter ( colladaTexture, MAYA_TEXTURE_FAST_PARAMETER, placement2d );
            addFloatParameter ( colladaTexture, MAYA_TEXTURE_REPEATU_PARAMETER, placement2d );
            addFloatParameter ( colladaTexture, MAYA_TEXTURE_REPEATV_PARAMETER, placement2d );
            addFloatParameter ( colladaTexture, MAYA_TEXTURE_OFFSETU_PARAMETER, placement2d );
            addFloatParameter ( colladaTexture, MAYA_TEXTURE_OFFSETV_PARAMETER, placement2d );
            addAngleParameter ( colladaTexture, MAYA_TEXTURE_ROTATEUV_PARAMETER, placement2d );
            addFloatParameter ( colladaTexture, MAYA_TEXTURE_NOISEU_PARAMETER, placement2d );
            addFloatParameter ( colladaTexture, MAYA_TEXTURE_NOISEV_PARAMETER, placement2d );
        }
    }

    // ------------------------------------------------------------
    void EffectTextureExporter::add3DProjection ( COLLADA::Texture* colladaTexture, MObject projection )
    {
        int projectionType;
        DagHelper::getPlugValue ( projection, ATTR_PROJECTION_TYPE, projectionType );
        String strProjectionType = ShaderHelper::projectionTypeToString ( projectionType );
        colladaTexture->addExtraTechniqueChildParameter ( 
            MAYA_PROFILE,
            MAYA_PROJECTION_ELEMENT,
            MAYA_PROJECTION_TYPE_PARAMETER,
            strProjectionType );

        MMatrix projectionMx;
        DagHelper::getPlugValue ( projection, ATTR_PLACEMENT_MATRIX, projectionMx );
        double sceneMatrix[4][4];
        MConvert::convertMMatrixToDouble4x4 ( sceneMatrix, projectionMx );
        colladaTexture->addExtraTechniqueChildParameter ( 
            MAYA_PROFILE,
            MAYA_PROJECTION_ELEMENT,
            MAYA_PROJECTION_MATRIX_PARAMETER,
            sceneMatrix );
    }

    // ------------------------------------------------------------
    void EffectTextureExporter::addBoolParameter ( COLLADA::Texture* colladaTexture,
            const char* plugName,
            MFnDependencyNode &placement2d )
    {
        MStatus status;
        MPlug plug = placement2d.findPlug ( plugName, &status );

        if ( status == MStatus::kSuccess )
        {
            bool value = false;
            plug.getValue ( value );

            colladaTexture->addExtraTechniqueParameter ( MAYA_PROFILE, plugName, value );

            // TODO Parameters???
            AnimationExporter* animationExporter = mDocumentExporter->getAnimationExporter();
            String parameters[] = {"BOOL"};
            animationExporter->addPlugAnimation ( plug, plugName, parameters, kBoolean );
            //  ANIM->AddPlugAnimation(placementNode, plugName, colladaParameter->GetAnimated(), kBoolean); }
        }
    }

    // ------------------------------------------------------------
    void EffectTextureExporter::addFloatParameter ( COLLADA::Texture* colladaTexture,
            const char* plugName,
            MFnDependencyNode &placement2d )
    {
        MStatus status;
        MPlug plug = placement2d.findPlug ( plugName, &status );

        if ( status == MStatus::kSuccess )
        {
            float value;
            plug.getValue ( value );

            colladaTexture->addExtraTechniqueParameter ( MAYA_PROFILE, plugName, value );

            // TODO Parameters???
            AnimationExporter* animationExporter = mDocumentExporter->getAnimationExporter();
            String parameters[] = {"FLOAT"};
            animationExporter->addPlugAnimation ( plug, plugName, parameters, kSingle );
            //  ANIM->AddPlugAnimation(placementNode, plugName, colladaParameter->GetAnimated(), kSingle); }
        }
    }

    // ------------------------------------------------------------
    void EffectTextureExporter::addAngleParameter ( COLLADA::Texture* colladaTexture,
            const char* plugName,
            MFnDependencyNode &placement2d )
    {
        MStatus status;
        MPlug plug = placement2d.findPlug ( plugName, &status );

        if ( status == MStatus::kSuccess )
        {
            float value;
            plug.getValue ( value );

            colladaTexture->addExtraTechniqueParameter ( MAYA_PROFILE, plugName, value );

            // TODO Parameters???
            AnimationExporter* animationExporter = mDocumentExporter->getAnimationExporter();
            String parameters[] = {"ANGLE"};
            animationExporter->addPlugAnimation ( plug, plugName, parameters, kSingle | kAngle );
            //  ANIM->AddPlugAnimation(placementNode, plugName, colladaParameter->GetAnimated(), kSingle | kAngle); }
        }
    }

    // ------------------------------------------------------------
    void EffectTextureExporter::openTechnique()
    {
        if ( !techniqueIsOpen )
        {
            COLLADA::StreamWriter* streamWriter = mDocumentExporter->getStreamWriter();

            mExtraSource = new COLLADA::Extra ( streamWriter );
            mExtraSource->openExtra();

            mTechniqueSource = new COLLADA::Technique ( streamWriter );
            mTechniqueSource->openTechnique ( MAYA_PROFILE );

            techniqueIsOpen = true;
        }
    }

    // ------------------------------------------------------------
    void EffectTextureExporter::closeTechnique()
    {
        if ( techniqueIsOpen )
        {
            mTechniqueSource->closeTechnique();
            mExtraSource->closeExtra();

            techniqueIsOpen = false;
        }
    }

}