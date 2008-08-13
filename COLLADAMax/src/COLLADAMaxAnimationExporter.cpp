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

#include "ColladaMaxStableHeaders.h"

#include "COLLADAStreamWriter.h"
#include "COLLADASource.h"
#include "COLLADAMathUtils.h"

#include "COLLADAMaxAnimationExporter.h"
#include "COLLADAMaxExportSceneGraph.h"

#include <max.h>
#include <istdplug.h> 
#include <control.h>
#include <units.h>


namespace COLLADAMax
{


    //---------------------------------------------------------------
    Animation::Animation ( Control * controller, const String & id, const String & sid, const String * parameter, int type, ConversionFunction conversionFunction )
            : mController ( controller ),
            mId ( id ),
            mSid ( sid ),
            mParameters ( parameter ),
            mType ( type ),
            mConversionFunction ( conversionFunction ),
			mInputTypeFlags(NONE)
    {}

    //---------------------------------------------------------------
    int Animation::getDimension() const
    {
        switch ( mType )
        {

        case FLOAT:

        case POSITION_X:

        case POSITION_Y:

        case POSITION_Z:

        case ROTATION_X:

        case ROTATION_Y:

        case ROTATION_Z:
            return 1;

		case FLOAT3:
            return 3;
		
		case SCALE_ROT_AXIS_R:
		case SCALE_ROT_AXIS:
			return 4;

        default:
            return 0;
        }
    }

	const float AnimationExporter::DEFAULT_INLENGHT = 0.333f;
	const float AnimationExporter::DEFAULT_OUTLENGHT = 0.333f;
    const float AnimationExporter::DEFAULT_INLENGHT_FLOAT = AnimationExporter::DEFAULT_INLENGHT;
	const Point3 AnimationExporter::DEFAULT_INLENGHT_POINT = Point3(AnimationExporter::DEFAULT_INLENGHT, AnimationExporter::DEFAULT_INLENGHT, AnimationExporter::DEFAULT_INLENGHT);
    const float AnimationExporter::DEFAULT_OUTLENGHT_FLOAT = AnimationExporter::DEFAULT_OUTLENGHT;
	const Point3 AnimationExporter::DEFAULT_OUTLENGHT_POINT = Point3(AnimationExporter::DEFAULT_OUTLENGHT, AnimationExporter::DEFAULT_OUTLENGHT, AnimationExporter::DEFAULT_OUTLENGHT);


    const float AnimationExporter::mTimeFactor = 1.0f / ( GetTicksPerFrame() * GetFrameRate() );



    //---------------------------------------------------------------
    AnimationExporter::AnimationExporter ( COLLADA::StreamWriter * streamWriter, DocumentExporter * documentExporter )
            : COLLADA::LibraryAnimations ( streamWriter ),
            mDocumentExporter ( documentExporter )
    {}

    //---------------------------------------------------------------
    TimeValue AnimationExporter::getPreviousTime ( const int & i, Control * controller ) const
    {
        return ( i > 0 ) ? controller->GetKeyTime ( i - 1 ) : controller->GetKeyTime ( i ) - TimeValue ( 1.0f / mTimeFactor );
    }

    //---------------------------------------------------------------
    TimeValue AnimationExporter::getNextTime ( const int & i, const int & keyCount, Control * controller ) const
    {
        return ( i < keyCount - 1 ) ? controller->GetKeyTime ( i + 1 ) : controller->GetKeyTime ( i ) + TimeValue ( 1.0f / mTimeFactor );
    }



    //---------------------------------------------------------------
    void AnimationExporter::doExport()
    {
        if ( !mAnimationList.empty() )
        {
            openAnimation();

            for ( AnimationList::iterator it = mAnimationList.begin(); it != mAnimationList.end();++it )
                exportSources ( *it );

            for ( AnimationList::iterator it = mAnimationList.begin(); it != mAnimationList.end();++it )
                exportSampler ( *it );

            for ( AnimationList::iterator it = mAnimationList.begin(); it != mAnimationList.end();++it )
                exportChannel ( *it );

            closeLibrary();
        }
    }

    //---------------------------------------------------------------
    void AnimationExporter::addAnimatedPoint3 ( Control * controller, const String & id, const String & sid, const String parameters[] )
    {
        Control * subControllers[ 3 ] = {controller->GetXController(), controller->GetYController(), controller->GetZController() };

        // First, Try to extract animations from the component controllers

        if ( subControllers[ 0 ] != NULL && subControllers[ 1 ] != NULL && subControllers[ 2 ] != NULL )
        {
            for ( int i = 0; i < 3; ++i )
            {
                if ( isAnimated ( subControllers[ i ] ) )
                {
                    Animation animation ( subControllers[ i ], id, sid, parameters + i, Animation::FLOAT );
                    addAnimation ( animation );
                }
            }
        }

        else if ( isAnimated ( controller ) )
        {
            // Else, with no subs, try and export ourselves as keyframes
            Animation animation ( controller, id, sid, parameters, Animation::FLOAT3 );
            addAnimation ( animation );
        }
    }

    //---------------------------------------------------------------
    void AnimationExporter::addAnimatedAngle ( Control * controller, const String & id, const String & sid, const String parameters[], int animatedAngle )
    {
        if ( isAnimated ( controller ) )
        {

            // maybe this rotation controller doesn't have XYZ components
            Control * xyzController = NULL;

            switch ( animatedAngle )
            {

            case Animation::ROTATION_X:
                xyzController = controller->GetXController();
                break;

            case Animation::ROTATION_Y:
                xyzController = controller->GetYController();
                break;

            case Animation::ROTATION_Z:
                xyzController = controller->GetZController();
                break;
            }

            if ( xyzController != NULL )
                controller = xyzController;

            Animation animation ( controller, id, sid, parameters, animatedAngle, &COLLADA::MathUtils::radToDegF );

            addAnimation ( animation );
        }
    }


	//---------------------------------------------------------------
	void AnimationExporter::addAnimatedAxisAngle ( Control * controller, const String & id, const String & sid, const String parameters[], int type )
	{
		if ( isAnimated ( controller ) )
		{
			Animation animation ( controller, id, sid, parameters, type);
			addAnimation ( animation );
		}
	}



    /* void AnimationExporter::addAnimation4( Control * controller, const String & id, const String & sid, const String parameters[])
     {
      Control * controllers[4] = {controller->GetXController(), controller->GetYController(), controller->GetZController(), controller->GetWController()};
      for ( int i = 0; i<4; ++i)
      {
       if ( isAnimated(controllers[i]) )
       {
        Animation animation(controllers[i], id, sid, parameters[i]);
        addAnimation(animation);
       }
      }
     }

    */

    //---------------------------------------------------------------
    bool AnimationExporter::isAnimated ( Control * controller )
    {
		bool animated = controller && controller->IsAnimated();

		if ( !animated )
			return false;
#if 0
		if ( mDocumentExporter->getOptions().getCheckIfAnimationsIsAnimated() )
			return checkIfIsAnimated(controller);
#endif
		return animated;
    }

#if 0
	/** @TODO implement a test that check if an animations animated, i.e. if the values change */
	//---------------------------------------------------------------
	bool AnimationExporter::checkIfIsAnimated ( Control * controller )
	{
		bool animated;

		bool isSampling = mDocumentExporter->getOptions().getSampleAnimation(); 

		IKeyControl * keyInterface = GetKeyControlInterface ( controller );

		if ( !isSampling )
		{
			Class_ID classId = controller->ClassID();

			if ( classId.PartB() != 0 )
			{
				// This is not a Max controller, sample it.
				// The only max controllers that have non-zero
				// values are not keyable (attach, link, etc).
				isSampling = true;
			}
			else if ( keyInterface && keyInterface->GetNumKeys() > 0 )
			{
				int keyCount = keyInterface->GetNumKeys();

				switch ( classId.PartA() )
				{

				case LININTERP_FLOAT_CLASS_ID:
					exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getFloatValue<ILinFloatKey> );
					break;

				case LININTERP_POSITION_CLASS_ID:
					if ( animation.getDimension() == 1 )
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getPositionSingleValue<ILinPoint3Key> );
					else
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getPositionValue<ILinPoint3Key> );

					break;

				case LININTERP_ROTATION_CLASS_ID:
					if ( animation.getDimension() == 1 )
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getRotationSingleValue<ILinRotKey> );
					else
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getRotationValue<ILinRotKey> );
					break;

				case HYBRIDINTERP_FLOAT_CLASS_ID:
					exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getFloatValue<IBezFloatKey> );
					break;
				case HYBRIDINTERP_POINT3_CLASS_ID:
				case HYBRIDINTERP_POSITION_CLASS_ID: 
					if ( animation.getDimension() == 1 )
					{
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getPositionSingleValue<IBezPoint3Key> );
					}
					else
					{
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getPositionValue<IBezPoint3Key> );
					}
					break;
				case HYBRIDINTERP_ROTATION_CLASS_ID:
					if ( animation.getDimension() == 1 )
					{
						mKeyValueList.reserve( keyInterface->GetNumKeys() );
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getRotationSingleValuePatchEuler );
					}
					else
					{
						mKeyValueList.reserve( 3 * keyInterface->GetNumKeys() );
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getRotationValuePatchEuler );
					}
					mKeyValueList.clear();
					break;
				case LININTERP_SCALE_CLASS_ID:
					if ( animation.getType() == Animation::SCALE)
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getLinearScaleValue );
					else if ( animation.getType() == Animation::SCALE_ROT_AXIS )
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getLinearScaleRotationAxisValue<false> );
					else if ( animation.getType() == Animation::SCALE_ROT_AXIS_R )
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getLinearScaleRotationAxisValue<true> );
					break;
				default:
					isSampling = true;
					break;
				}
			}
		}


		return animated;
	}
#endif

    //---------------------------------------------------------------
    String AnimationExporter::getBaseId ( const Animation & animation )
    {
        if ( animation.getDimension() == 1 )
            return animation.getId() + "_" + animation.getSid() + "." + * ( animation.getParameter() );
        else
            return animation.getId() + "_" + animation.getSid();
    }

    //---------------------------------------------------------------
    String AnimationExporter::getTarget ( const Animation & animation )
    {
        if ( animation.getDimension() == 1 )
            return animation.getId() + "/" + animation.getSid() + "." + * ( animation.getParameter() );
        else
            return animation.getId() + "/" + animation.getSid();
    }

    //---------------------------------------------------------------
    void AnimationExporter::exportSources ( Animation & animation )
    {
        bool isSampling = mDocumentExporter->getOptions().getSampleAnimation(); //||forceSampling;

        Control * controller = animation.getController();

        String baseId = getBaseId ( animation );

		IKeyControl * keyInterface = GetKeyControlInterface ( controller );

		if (!isSampling)
		{
			if ( !keyInterface ) 
				isSampling = true;
			else if (keyInterface->GetNumKeys() <= 1) 
				return;
		}


		if ( !isSampling )
        {
            Class_ID classId = controller->ClassID();

            if ( classId.PartB() != 0 )
            {
                // This is not a Max controller, sample it.
                // The only max controllers that have non-zero
                // values are not keyable (attach, link, etc).
                isSampling = true;
            }
            else if ( keyInterface != NULL && keyInterface->GetNumKeys() > 0 )
            {
                int keyCount = keyInterface->GetNumKeys();

                switch ( classId.PartA() )
                {

                case LININTERP_FLOAT_CLASS_ID:
                    exportInputSource<ILinFloatKey> ( baseId, controller, keyInterface );
                    exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getFloatValue<ILinFloatKey> );
                    exportInterpolationSource ( baseId, keyInterface, getUniformInterpolation<LibraryAnimations::LINEAR_NAME>, keyInterface->GetNumKeys() );
					animation.setInputTypeFlags(Animation::INPUT | Animation::OUTPUT | Animation::INTERPOLATION);
                    break;

                case LININTERP_POSITION_CLASS_ID:
                    exportInputSource<ILinPoint3Key> ( baseId, controller, keyInterface );

                    if ( animation.getDimension() == 1 )
                        exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getPoint3SingleValue<ILinPoint3Key> );
                    else
                        exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getPoint3Value<ILinPoint3Key> );

                    exportInterpolationSource ( baseId, keyInterface, getUniformInterpolation<LibraryAnimations::LINEAR_NAME>, keyInterface->GetNumKeys() );
					animation.setInputTypeFlags(Animation::INPUT | Animation::OUTPUT | Animation::INTERPOLATION);
                    break;

                case LININTERP_ROTATION_CLASS_ID:
                    exportInputSource<ILinRotKey> ( baseId, controller, keyInterface );

                    if ( animation.getDimension() == 1 )
                        exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getRotationSingleValue<ILinRotKey> );
                    else
                        exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getRotationValue<ILinRotKey> );

                    exportInterpolationSource ( baseId, keyInterface, getUniformInterpolation<LibraryAnimations::LINEAR_NAME>, keyInterface->GetNumKeys() );
					animation.setInputTypeFlags(Animation::INPUT | Animation::OUTPUT | Animation::INTERPOLATION);

                    break;

                case HYBRIDINTERP_FLOAT_CLASS_ID:
                    exportInputSource<IBezFloatKey> ( baseId, controller, keyInterface );

                    exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getFloatValue<IBezFloatKey> );

                    exportInTangentSource ( animation, baseId, keyInterface, &AnimationExporter::getFloatInTangentValue );

                    exportOutTangentSource ( animation, baseId, keyInterface, &AnimationExporter::getFloatOutTangentValue );

                    exportInterpolationSource ( baseId, keyInterface, getBezierInterpolation<IBezFloatKey>, keyInterface->GetNumKeys() );
					animation.setInputTypeFlags(Animation::INPUT | Animation::OUTPUT | Animation::IN_TANGENT | Animation::OUT_TANGENT | Animation::INTERPOLATION);

                    break;
				case HYBRIDINTERP_POINT3_CLASS_ID:
				case HYBRIDINTERP_POSITION_CLASS_ID: 
				case HYBRIDINTERP_COLOR_CLASS_ID:
					exportInputSource<IBezPoint3Key> ( baseId, controller, keyInterface );
					if ( animation.getDimension() == 1 )
					{
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getPoint3SingleValue<IBezPoint3Key> );
						exportInTangentSource ( animation, baseId, keyInterface, &AnimationExporter::getPoint3SingleInTangentValue );
						exportOutTangentSource ( animation, baseId, keyInterface, &AnimationExporter::getPoint3SingleOutTangentValue );
					}
					else
					{
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getPoint3Value<IBezPoint3Key> );
						exportInTangentSource ( animation, baseId, keyInterface, &AnimationExporter::getPoint3InTangentValue );
						exportOutTangentSource ( animation, baseId, keyInterface, &AnimationExporter::getPoint3OutTangentValue );
					}
					exportInterpolationSource ( baseId, keyInterface, getBezierInterpolation<IBezPoint3Key>, keyInterface->GetNumKeys() );
					animation.setInputTypeFlags(Animation::INPUT | Animation::OUTPUT | Animation::IN_TANGENT | Animation::OUT_TANGENT | Animation::INTERPOLATION);
					break;
				case HYBRIDINTERP_POINT4_CLASS_ID:
				case HYBRIDINTERP_FRGBA_CLASS_ID:
					exportInputSource<IBezPoint4Key> ( baseId, controller, keyInterface );
					if ( animation.getDimension() == 1 )
					{
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getPoint3SingleValue<IBezPoint3Key> );
						exportInTangentSource ( animation, baseId, keyInterface, &AnimationExporter::getPoint3SingleInTangentValue );
						exportOutTangentSource ( animation, baseId, keyInterface, &AnimationExporter::getPoint3SingleOutTangentValue );
					}
					else
					{
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getPoint3Value<IBezPoint3Key> );
						exportInTangentSource ( animation, baseId, keyInterface, &AnimationExporter::getPoint3InTangentValue );
						exportOutTangentSource ( animation, baseId, keyInterface, &AnimationExporter::getPoint3OutTangentValue );
					}
					exportInterpolationSource ( baseId, keyInterface, getBezierInterpolation<IBezPoint3Key>, keyInterface->GetNumKeys() );
					animation.setInputTypeFlags(Animation::INPUT | Animation::OUTPUT | Animation::IN_TANGENT | Animation::OUT_TANGENT | Animation::INTERPOLATION);
					break;

				case HYBRIDINTERP_ROTATION_CLASS_ID:
					exportInputSource<IBezQuatKey> ( baseId, controller, keyInterface );
					if ( animation.getDimension() == 1 )
					{
						mKeyValueList.reserve( keyInterface->GetNumKeys() );
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getRotationSingleValuePatchEuler );
						exportInTangentSource ( animation, baseId, keyInterface, &AnimationExporter::getRotationSingleInTangentPatchEuler );
						exportOutTangentSource ( animation, baseId, keyInterface, &AnimationExporter::getRotationSingleOutTangentPatchEuler );
					}
					else
					{
						mKeyValueList.reserve( 3 * keyInterface->GetNumKeys() );
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getRotationValuePatchEuler );
						exportInTangentSource ( animation, baseId, keyInterface, &AnimationExporter::getRotationSingleInTangentPatchEuler );
						exportOutTangentSource ( animation, baseId, keyInterface, &AnimationExporter::getRotationSingleOutTangentPatchEuler );
					}
					mKeyValueList.clear();
					exportInterpolationSource ( baseId, keyInterface, getBezierInterpolation<IBezQuatKey>, keyInterface->GetNumKeys() );
					animation.setInputTypeFlags(Animation::INPUT | Animation::OUTPUT | Animation::IN_TANGENT | Animation::OUT_TANGENT | Animation::INTERPOLATION);
					break;
				case LININTERP_SCALE_CLASS_ID:
					exportInputSource<ILinScaleKey> ( baseId, controller, keyInterface );
					if ( animation.getType() == Animation::SCALE)
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getLinearScaleValue );
					else if ( animation.getType() == Animation::SCALE_ROT_AXIS )
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getLinearScaleRotationAxisValue<false> );
					else if ( animation.getType() == Animation::SCALE_ROT_AXIS_R )
						exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getLinearScaleRotationAxisValue<true> );
					exportInterpolationSource ( baseId, keyInterface, getUniformInterpolation<LibraryAnimations::LINEAR_NAME>, keyInterface->GetNumKeys() );
					animation.setInputTypeFlags(Animation::INPUT | Animation::OUTPUT | Animation::INTERPOLATION);
					break;
//				case TCBINTERP_FLOAT_CLASS_ID:
//					exportInputSource<ITCBFloatKey> ( baseId, controller, keyInterface );
//					exportOutputSource ( animation, baseId, keyInterface, &AnimationExporter::getFloatValue<ITCBFloatKey> );

					break;
				default:
					isSampling = true;
					break;
                }
            }
        }

		if ( isSampling )
		{
			int ticksPerFrame = GetTicksPerFrame();
			TimeValue startTime = mDocumentExporter->getOptions().getAnimationStart();
			TimeValue endTime = mDocumentExporter->getOptions().getAnimationEnd() + 1;

			if ( endTime > startTime )
			{
				exportSamplingInputSource(baseId, startTime, endTime, ticksPerFrame);
				SClass_ID type = controller->SuperClassID();
				switch (type)
				{
				case CTRL_FLOAT_CLASS_ID:
					exportSamplingFloatOutputSource(animation, baseId, keyInterface, startTime, endTime, ticksPerFrame);
					animation.setInputTypeFlags(Animation::INPUT | Animation::OUTPUT | Animation::INTERPOLATION);
					break;
				case CTRL_POINT3_CLASS_ID:
				case CTRL_POSITION_CLASS_ID:
					exportSamplingPoint3OutputSource(animation, baseId, keyInterface, startTime, endTime, ticksPerFrame);
					animation.setInputTypeFlags(Animation::INPUT | Animation::OUTPUT | Animation::INTERPOLATION);
					break;
				case CTRL_ROTATION_CLASS_ID:
					exportSamplingRotationOutputSource(animation, baseId, keyInterface, startTime, endTime, ticksPerFrame);
					animation.setInputTypeFlags(Animation::INPUT | Animation::OUTPUT | Animation::INTERPOLATION);
					break;
				}
				exportSamplingInterpolationSource(baseId, startTime, endTime, ticksPerFrame);
			}

		}
    }


    //---------------------------------------------------------------
    template <class KeyClassName>
    void AnimationExporter::exportInputSource ( const String & baseId, Control * controller, IKeyControl* keyInterface )
    {
        int keyCount = keyInterface->GetNumKeys();

        COLLADA::FloatSource source ( mSW );
        source.setId ( baseId + INPUT_SOURCE_ID_SUFFIX );
        source.setArrayId ( baseId + INPUT_SOURCE_ID_SUFFIX + ARRAY_ID_SUFFIX );
        source.setAccessorStride ( 1 );
        source.getParameterNameList().push_back ( "TIME" );
        source.setAccessorCount ( keyCount );
        source.prepareToAppendValues();

        for ( int i = 0; i < keyCount; ++i )
        {
            KeyClassName key;
            keyInterface->GetKey ( i, &key );
            source.appendValues ( key.time * mTimeFactor );
        }

        source.finish();
    }


    //---------------------------------------------------------------
    void AnimationExporter::exportOutputSource ( const Animation & animation, const String & baseId, IKeyControl* keyInterface, OutputValueFunctionPtr outputValueFunction )
    {
        int keyCount = keyInterface->GetNumKeys();
        int keyLength = animation.getDimension();

        COLLADA::FloatSource source ( mSW );
        source.setId ( baseId + OUTPUT_SOURCE_ID_SUFFIX );
        source.setArrayId ( baseId + OUTPUT_SOURCE_ID_SUFFIX + ARRAY_ID_SUFFIX );
        source.setAccessorStride ( keyLength );

        for ( int i = 0; i < keyLength; ++i )
            source.getParameterNameList().push_back ( * ( animation.getParameter() + i ) );

        source.setAccessorCount ( keyCount );

        source.prepareToAppendValues();

        float * keyBuffer = new float[ keyLength ];

        for ( int i = 0; i < keyCount; ++i )
        {
            (this->*outputValueFunction) ( keyBuffer, keyInterface, i, animation );

            for ( int j = 0; j < keyLength; ++j )
            {
                const Animation::ConversionFunction & conversionFunction = animation.getConversionFunction();

                if ( conversionFunction )
                    source.appendValues ( conversionFunction ( keyBuffer[ j ] ) );
                else
                    source.appendValues ( keyBuffer[ j ] );
            }
        }

        delete[] keyBuffer;

        source.finish();
    }

    //---------------------------------------------------------------
	template <class KeyType>
    void AnimationExporter::getFloatValue ( float * keyValues, IKeyControl * keyInterface, const int & keyIndex, const Animation & animation )
    {
        KeyType key;
        keyInterface->GetKey ( keyIndex, &key );
        *keyValues = key.val;
    }

    //---------------------------------------------------------------
	template<class KeyType>
    void AnimationExporter::getPoint3SingleValue ( float * keyValues, IKeyControl * keyInterface, const int & keyIndex, const Animation & animation )
    {
        KeyType key;
        keyInterface->GetKey ( keyIndex, &key );
        *keyValues = key.val[ animation.getType() - Animation::POSITION_X ];
    }

    //---------------------------------------------------------------
	template<class KeyType>
    void AnimationExporter::getPoint3Value ( float * keyValues, IKeyControl * keyInterface, const int & keyIndex, const Animation & animation )
    {
        KeyType key;
        keyInterface->GetKey ( keyIndex, &key );
        keyValues[ 0 ] = key.val[ 0 ];
        keyValues[ 1 ] = key.val[ 1 ];
        keyValues[ 2 ] = key.val[ 2 ];
    }

    //---------------------------------------------------------------
	template<class KeyType>
    void AnimationExporter::getRotationSingleValue ( float * keyValues, IKeyControl * keyInterface, const int & keyIndex, const Animation & animation )
    {
        KeyType key;
        keyInterface->GetKey ( keyIndex, &key );
        float eulerAngles[ 3 ];
        key.val.GetEuler ( &eulerAngles[ 0 ], &eulerAngles[ 1 ], &eulerAngles[ 2 ] );
        *keyValues = eulerAngles[ animation.getType() - Animation::ROTATION_X ];
    }

    //---------------------------------------------------------------
	template<class KeyType>
    void AnimationExporter::getRotationValue ( float * keyValues, IKeyControl * keyInterface, const int & keyIndex, const Animation & animation )
    {
        KeyType key;
        keyInterface->GetKey ( keyIndex, &key );
        key.val.GetEuler ( &keyValues[ 0 ], &keyValues[ 1 ], &keyValues[ 2 ] );
    }


	//---------------------------------------------------------------
	void AnimationExporter::getRotationSingleValuePatchEuler ( float * keyValues, IKeyControl * keyInterface, const int & keyIndex, const Animation & animation )
	{
		IBezQuatKey key;
		keyInterface->GetKey ( keyIndex, &key );
		float eulerAngles[ 3 ];
		key.val.GetEuler ( &eulerAngles[ 0 ], &eulerAngles[ 1 ], &eulerAngles[ 2 ] );
		if ( keyIndex > 0)
			patchEuler(mPreviousEulerAngles, eulerAngles);

		mPreviousEulerAngles[0] = eulerAngles[0];
		mPreviousEulerAngles[1] = eulerAngles[1];
		mPreviousEulerAngles[2] = eulerAngles[2];

		*keyValues = eulerAngles[ animation.getType() - Animation::ROTATION_X ];
		
		Animation::ConversionFunction conversionFunction = animation.getConversionFunction();
		if ( conversionFunction )
			mKeyValueList.push_back((float)(conversionFunction(*keyValues)));
		else
			mKeyValueList.push_back((float)(*keyValues));

	}

	//---------------------------------------------------------------
	void AnimationExporter::getRotationValuePatchEuler ( float * keyValues, IKeyControl * keyInterface, const int & keyIndex, const Animation & animation )
	{
		IBezQuatKey key;
		keyInterface->GetKey ( keyIndex, &key );
		float eulerAngles[ 3 ];
		key.val.GetEuler ( &eulerAngles[ 0 ], &eulerAngles[ 1 ], &eulerAngles[ 2 ] );
		if ( keyIndex > 0)
			patchEuler(mPreviousEulerAngles, eulerAngles);

		mPreviousEulerAngles[0] = eulerAngles[0];
		mPreviousEulerAngles[1] = eulerAngles[1];
		mPreviousEulerAngles[2] = eulerAngles[2];

		keyValues[ 0 ] = eulerAngles[0];
		keyValues[ 1 ] = eulerAngles[1];
		keyValues[ 2 ] = eulerAngles[2];

		Animation::ConversionFunction conversionFunction = animation.getConversionFunction();
		if ( conversionFunction )
		{
			mKeyValueList.push_back((float)(conversionFunction(eulerAngles[0])));
			mKeyValueList.push_back((float)(conversionFunction(eulerAngles[1])));
			mKeyValueList.push_back((float)(conversionFunction(eulerAngles[2])));
		}
		else
		{
			mKeyValueList.push_back(eulerAngles[0]);
			mKeyValueList.push_back(eulerAngles[1]);
			mKeyValueList.push_back(eulerAngles[2]);
		}
	}

	//---------------------------------------------------------------
	void AnimationExporter::getLinearScaleValue( float * keyValues, IKeyControl * keyInterface, const int & keyIndex, const Animation & animation )
	{
		ILinScaleKey key;
		keyInterface->GetKey ( keyIndex, &key );
		keyValues[0] = key.val.s.x;
		keyValues[1] = key.val.s.y;
		keyValues[2] = key.val.s.z;
	}

	//---------------------------------------------------------------
	template<bool reversed>
	void AnimationExporter::getLinearScaleRotationAxisValue( float * keyValues, IKeyControl * keyInterface, const int & keyIndex, const Animation & animation )
	{
		ILinScaleKey key;
		keyInterface->GetKey ( keyIndex, &key );
		AngAxis angleAxis(key.val.q);

		keyValues[0] = angleAxis.axis.x;
		keyValues[1] = angleAxis.axis.y;
		keyValues[2] = angleAxis.axis.z;
		if ( reversed )
			keyValues[3] = -COLLADA::MathUtils::radToDegF(angleAxis.angle);
		else
			keyValues[3] = COLLADA::MathUtils::radToDegF(angleAxis.angle);
	}

	
	//---------------------------------------------------------------
    void AnimationExporter::exportTangentSource ( const String & sourceIdSuffix, const Animation & animation, const String & baseId, IKeyControl* keyInterface, TangentValueFunctionPtr tangentValueFunction )
    {
        int keyCount = keyInterface->GetNumKeys();
		int keyLength = animation.getDimension();

        COLLADA::FloatSource source ( mSW );
        source.setId ( baseId + sourceIdSuffix );
        source.setArrayId ( baseId + sourceIdSuffix + ARRAY_ID_SUFFIX );
        source.setAccessorStride ( 2 * keyLength );
		for ( int i = 0; i < keyLength; ++i )
		{
			source.getParameterNameList().push_back ( "X" );
			source.getParameterNameList().push_back ( "Y" );
		}
        source.setAccessorCount ( keyCount );
        source.prepareToAppendValues();

		float * keyBufferX = new float[ keyLength ];
		float * keyBufferY = new float[ keyLength ];

        for ( int i = 0; i < keyCount; ++i )
        {
            ( this->*tangentValueFunction ) ( keyBufferX, keyBufferY, keyInterface, i, keyCount, animation );
			for ( int j = 0; j < keyLength; ++j )
			{
	            source.appendValues ( keyBufferX[j], keyBufferY[j] );
			}
        }

		delete[] keyBufferX;
		delete[] keyBufferY;

        source.finish();
    }

    //---------------------------------------------------------------
    void AnimationExporter::exportInTangentSource ( const Animation & animation, const String & baseId, IKeyControl* keyInterface, TangentValueFunctionPtr tangentValueFunction )
    {
        exportTangentSource ( INTANGENT_SOURCE_ID_SUFFIX, animation, baseId, keyInterface, tangentValueFunction );
    }

    //---------------------------------------------------------------
    void AnimationExporter::getFloatInTangentValue ( float * inTangentValuesX, float * inTangentValuesY, IKeyControl * keyInterface, const int & keyIndex, const int & keyCount, const Animation & animation )
    {
        IBezFloatKey key;
        keyInterface->GetKey ( keyIndex, &key );

        float previousTime = ( float ) getPreviousTime ( keyIndex, animation.getController() );

        //testing
        /*        bool b1 = key.flags & BEZKEY_UNCONSTRAINHANDLE;
                bool b2 = GetInTanType( key.flags ) != BEZKEY_USER;
                bool b3 = GetOutTanType( key.flags ) != BEZKEY_USER;
                bool b4 = GetOutTanType( key.flags ) == BEZKEY_LINEAR;
                bool b5 = GetOutTanType( key.flags ) == BEZKEY_STEP;
        */
        InterpolationType interpolationOutType = getInterpolationType ( key.flags );

        // export control point. If it is not a BEZIER interpolation, export the point itself

        if ( interpolationOutType == BEZIER )
        {
            if ( GetInTanType ( key.flags ) != BEZKEY_USER )
                key.inLength = DEFAULT_INLENGHT_FLOAT;
            else if ( key.flags & BEZKEY_UNCONSTRAINHANDLE )
                key.inLength *= GetTicksPerFrame() / ( float ) ( key.time - previousTime );

            float inInterval = ( key.time - previousTime ) * key.inLength;

            *inTangentValuesX = ( key.time - inInterval ) * mTimeFactor;

            *inTangentValuesY = key.val + key.intan * inInterval;
        }

        else
        {
            /// @TODO: clarify if this makes sense or if we should export the same as above
            *inTangentValuesX = key.time * mTimeFactor;
            *inTangentValuesY = key.val;
        }
    }


	//---------------------------------------------------------------
	void AnimationExporter::getPoint3SingleInTangentValue ( float * inTangentValuesX, float * inTangentValuesY, IKeyControl * keyInterface, const int & keyIndex, const int & keyCount, const Animation & animation )
	{
		IBezPoint3Key key;
		keyInterface->GetKey ( keyIndex, &key );

		float previousTime = ( float ) getPreviousTime ( keyIndex, animation.getController() );

		//testing
		/*        bool b1 = key.flags & BEZKEY_UNCONSTRAINHANDLE;
		bool b2 = GetInTanType( key.flags ) != BEZKEY_USER;
		bool b3 = GetOutTanType( key.flags ) != BEZKEY_USER;
		bool b4 = GetOutTanType( key.flags ) == BEZKEY_LINEAR;
		bool b5 = GetOutTanType( key.flags ) == BEZKEY_STEP;
		*/
		InterpolationType interpolationOutType = getInterpolationType ( key.flags );

		int keyParamterIndex = animation.getType() - Animation::POSITION_X;

		// export control point. If it is not a BEZIER interpolation, export the point itself

		if ( interpolationOutType == BEZIER )
		{
			if ( GetInTanType ( key.flags ) != BEZKEY_USER )
				key.inLength = DEFAULT_INLENGHT_POINT;
			else if ( key.flags & BEZKEY_UNCONSTRAINHANDLE )
				key.inLength *= GetTicksPerFrame() / ( float ) ( key.time - previousTime );

			float inInterval = ( key.time - previousTime ) * key.inLength[keyParamterIndex];
			*inTangentValuesX = ( key.time - inInterval ) * mTimeFactor;
			*inTangentValuesY = key.val[keyParamterIndex] + key.intan[keyParamterIndex] * inInterval;
		}

		else
		{
			/// @TODO: clarify if this makes sense or if we should export the same as above
			*inTangentValuesX = key.time * mTimeFactor;
			*inTangentValuesY = key.val[keyParamterIndex];
		}
	}


	//---------------------------------------------------------------
	void AnimationExporter::getPoint3InTangentValue ( float * inTangentValuesX, float * inTangentValuesY, IKeyControl * keyInterface, const int & keyIndex, const int & keyCount, const Animation & animation )
	{
		IBezPoint3Key key;
		keyInterface->GetKey ( keyIndex, &key );

		float previousTime = ( float ) getPreviousTime ( keyIndex, animation.getController() );

		//testing
		/*        bool b1 = key.flags & BEZKEY_UNCONSTRAINHANDLE;
		bool b2 = GetInTanType( key.flags ) != BEZKEY_USER;
		bool b3 = GetOutTanType( key.flags ) != BEZKEY_USER;
		bool b4 = GetOutTanType( key.flags ) == BEZKEY_LINEAR;
		bool b5 = GetOutTanType( key.flags ) == BEZKEY_STEP;
		*/
		InterpolationType interpolationOutType = getInterpolationType ( key.flags );

		// export control point. If it is not a BEZIER interpolation, export the point itself

		if ( interpolationOutType == BEZIER )
		{
			if ( GetInTanType ( key.flags ) != BEZKEY_USER )
				key.inLength = DEFAULT_INLENGHT_POINT;
			else if ( key.flags & BEZKEY_UNCONSTRAINHANDLE )
				key.inLength *= GetTicksPerFrame() / ( float ) ( key.time - previousTime );

			float inInterval;

			inInterval = ( key.time - previousTime ) * key.inLength.x;
			inTangentValuesX[0] = ( key.time - inInterval ) * mTimeFactor;
			inTangentValuesY[0] = key.val.x + key.intan.x * inInterval;

			inInterval = ( key.time - previousTime ) * key.inLength.y;
			inTangentValuesX[1] = ( key.time - inInterval ) * mTimeFactor;
			inTangentValuesY[1] = key.val.y + key.intan.y * inInterval;

			inInterval = ( key.time - previousTime ) * key.inLength.z;
			inTangentValuesX[2] = ( key.time - inInterval ) * mTimeFactor;
			inTangentValuesY[2] = key.val.z + key.intan.z * inInterval;

		}

		else
		{
			/// @TODO: clarify if this makes sense or if we should export the same as above
			inTangentValuesX[0] = key.time * mTimeFactor;
			inTangentValuesX[1] = key.time * mTimeFactor;
			inTangentValuesX[2] = key.time * mTimeFactor;
			
			inTangentValuesY[0] = key.val.x;
			inTangentValuesY[1] = key.val.y;
			inTangentValuesY[2] = key.val.z;
		}
	}


	//---------------------------------------------------------------
	void AnimationExporter::getRotationSingleInTangentPatchEuler ( float * inTangentValuesX, float * inTangentValuesY, IKeyControl * keyInterface, const int & keyIndex, const int & keyCount, const Animation & animation )
	{
		IBezQuatKey key;
		keyInterface->GetKey ( keyIndex, &key );

		float previousTime = ( float ) getPreviousTime ( keyIndex, animation.getController() );

		//testing
		/*        bool b1 = key.flags & BEZKEY_UNCONSTRAINHANDLE;
		bool b2 = GetInTanType( key.flags ) != BEZKEY_USER;
		bool b3 = GetOutTanType( key.flags ) != BEZKEY_USER;
		bool b4 = GetOutTanType( key.flags ) == BEZKEY_LINEAR;
		bool b5 = GetOutTanType( key.flags ) == BEZKEY_STEP;
		*/
		InterpolationType interpolationOutType = getInterpolationType ( key.flags );

		int keyParamterIndex = animation.getType() - Animation::ROTATION_X;


		// export control point. If it is not a BEZIER interpolation, export the point itself

		size_t keyValueIndex = keyIndex;

		if ( interpolationOutType == BEZIER )
		{
			float inInterval = ( key.time - previousTime ) * DEFAULT_INLENGHT;
			*inTangentValuesX = ( key.time - inInterval ) * mTimeFactor;
			*inTangentValuesY = mKeyValueList[keyValueIndex];
		}

		else
		{
			/// @TODO: clarify if this makes sense or if we should export the same as above
			*inTangentValuesX = key.time * mTimeFactor;

			*inTangentValuesY = mKeyValueList[keyValueIndex];
		}
	}


	//---------------------------------------------------------------
		void AnimationExporter::getRotationInTangentPatchEuler ( float * inTangentValuesX, float * inTangentValuesY, IKeyControl * keyInterface, const int & keyIndex, const int & keyCount, const Animation & animation )
	{
		IBezQuatKey key;
		keyInterface->GetKey ( keyIndex, &key );

		float previousTime = ( float ) getPreviousTime ( keyIndex, animation.getController() );

		//testing
		/*        bool b1 = key.flags & BEZKEY_UNCONSTRAINHANDLE;
		bool b2 = GetInTanType( key.flags ) != BEZKEY_USER;
		bool b3 = GetOutTanType( key.flags ) != BEZKEY_USER;
		bool b4 = GetOutTanType( key.flags ) == BEZKEY_LINEAR;
		bool b5 = GetOutTanType( key.flags ) == BEZKEY_STEP;
		*/
		InterpolationType interpolationOutType = getInterpolationType ( key.flags );

		// export control point. If it is not a BEZIER interpolation, export the point itself

		size_t keyValueIndex = 3 * keyIndex;

		if ( interpolationOutType == BEZIER )
		{
			float inInterval = ( key.time - previousTime ) * DEFAULT_INLENGHT;

			inTangentValuesX[0] = ( key.time - inInterval ) * mTimeFactor;
			inTangentValuesY[0] = mKeyValueList[keyValueIndex++];

			inTangentValuesX[1] = inTangentValuesX[0];
			inTangentValuesY[1] = mKeyValueList[keyValueIndex++];

			inTangentValuesX[2] = inTangentValuesX[0];
			inTangentValuesY[2] = mKeyValueList[keyValueIndex++];
		}

		else
		{
			/// @TODO: clarify if this makes sense or if we should export the same as above
			inTangentValuesX[0] = key.time * mTimeFactor;
			inTangentValuesX[1] = inTangentValuesX[0];
			inTangentValuesX[2] = inTangentValuesX[0];

			inTangentValuesY[0] = mKeyValueList[keyValueIndex++];
			inTangentValuesY[1] = mKeyValueList[keyValueIndex++];
			inTangentValuesY[2] = mKeyValueList[keyValueIndex++];
		}
	}


	//---------------------------------------------------------------
    void AnimationExporter::exportOutTangentSource ( const Animation & animation, const String & baseId, IKeyControl* keyInterface, TangentValueFunctionPtr tangentValueFunction )
    {
        exportTangentSource ( OUTTANGENT_SOURCE_ID_SUFFIX, animation, baseId, keyInterface, tangentValueFunction );
    }

    //---------------------------------------------------------------
    void AnimationExporter::getFloatOutTangentValue ( float * outTangentValuesX, float * outTangentValuesY, IKeyControl * keyInterface, const int & keyIndex, const int & keyCount, const Animation & animation )
    {
        IBezFloatKey key;
        keyInterface->GetKey ( keyIndex, &key );

        float nextTime = ( float ) getNextTime ( keyIndex, keyCount, animation.getController() );

        //testing
        /*        bool b1 = key.flags & BEZKEY_UNCONSTRAINHANDLE;
                bool b2 = GetInTanType( key.flags ) != BEZKEY_USER;
                bool b3 = GetOutTanType( key.flags ) != BEZKEY_USER;
                bool b4 = GetOutTanType( key.flags ) == BEZKEY_LINEAR;
                bool b5 = GetOutTanType( key.flags ) == BEZKEY_STEP;
        */
        InterpolationType interpolationOutType = getInterpolationType ( key.flags );

        // export control point. If it is not a BEZIER interpolation, export the point itself

        if ( interpolationOutType == BEZIER )
        {
            if ( GetOutTanType ( key.flags ) != BEZKEY_USER )
                key.outLength = DEFAULT_OUTLENGHT_FLOAT;
            else if ( key.flags & BEZKEY_UNCONSTRAINHANDLE )
                key.outLength *= GetTicksPerFrame() / ( float ) ( nextTime - key.time );

            float outInterval = ( nextTime - key.time ) * key.outLength;

            *outTangentValuesX = ( key.time + outInterval ) * mTimeFactor;

            *outTangentValuesY = key.val + key.outtan * outInterval;
        }

        else
        {
            *outTangentValuesX = key.time * mTimeFactor;
            *outTangentValuesY = key.val;
        }
    }


	//---------------------------------------------------------------
	void AnimationExporter::getPoint3SingleOutTangentValue ( float * outTangentValuesX, float * outTangentValuesY, IKeyControl * keyInterface, const int & keyIndex, const int & keyCount, const Animation & animation )
	{
		IBezPoint3Key key;
		keyInterface->GetKey ( keyIndex, &key );

		float nextTime = ( float ) getNextTime ( keyIndex, keyCount, animation.getController() );

		//testing
		/*        bool b1 = key.flags & BEZKEY_UNCONSTRAINHANDLE;
		bool b2 = GetInTanType( key.flags ) != BEZKEY_USER;
		bool b3 = GetOutTanType( key.flags ) != BEZKEY_USER;
		bool b4 = GetOutTanType( key.flags ) == BEZKEY_LINEAR;
		bool b5 = GetOutTanType( key.flags ) == BEZKEY_STEP;
		*/
		InterpolationType interpolationOutType = getInterpolationType ( key.flags );

		int keyParamterIndex = animation.getType() - Animation::POSITION_X;

		// export control point. If it is not a BEZIER interpolation, export the point itself

		if ( interpolationOutType == BEZIER )
		{

			if ( GetOutTanType ( key.flags ) != BEZKEY_USER )
				key.outLength = DEFAULT_OUTLENGHT_POINT;
			else if ( key.flags & BEZKEY_UNCONSTRAINHANDLE )
				key.outLength *= GetTicksPerFrame() / ( float ) ( nextTime - key.time );

			float outInterval = ( nextTime - key.time ) * key.outLength[keyParamterIndex];

			*outTangentValuesX = ( key.time + outInterval ) * mTimeFactor;
			*outTangentValuesY = key.val[keyParamterIndex] + key.outtan[keyParamterIndex] * outInterval;
		}

		else
		{
			/// @TODO: clarify if this makes sense or if we should export the same as above
			*outTangentValuesX = key.time * mTimeFactor;
			*outTangentValuesY = key.val[keyParamterIndex];
		}
	}


	//---------------------------------------------------------------
	void AnimationExporter::getPoint3OutTangentValue ( float * outTangentValuesX, float * outTangentValuesY, IKeyControl * keyInterface, const int & keyIndex, const int & keyCount, const Animation & animation )
	{
		IBezPoint3Key key;
		keyInterface->GetKey ( keyIndex, &key );

		float nextTime = ( float ) getNextTime ( keyIndex, keyCount, animation.getController() );

		//testing
		/*        bool b1 = key.flags & BEZKEY_UNCONSTRAINHANDLE;
		bool b2 = GetInTanType( key.flags ) != BEZKEY_USER;
		bool b3 = GetOutTanType( key.flags ) != BEZKEY_USER;
		bool b4 = GetOutTanType( key.flags ) == BEZKEY_LINEAR;
		bool b5 = GetOutTanType( key.flags ) == BEZKEY_STEP;
		*/
		InterpolationType interpolationOutType = getInterpolationType ( key.flags );

		// export control point. If it is not a BEZIER interpolation, export the point itself

		if ( interpolationOutType == BEZIER )
		{
			if ( GetOutTanType ( key.flags ) != BEZKEY_USER )
				key.outLength = DEFAULT_OUTLENGHT_POINT;
			else if ( key.flags & BEZKEY_UNCONSTRAINHANDLE )
				key.outLength *= GetTicksPerFrame() / ( float ) ( nextTime - key.time );

			float outInterval;

			outInterval = ( nextTime - key.time ) * key.outLength.x;
			outTangentValuesX[0] = ( key.time + outInterval ) * mTimeFactor;
			outTangentValuesY[0] = key.val.x + key.outtan.x * outInterval;

			outInterval = ( nextTime - key.time ) * key.outLength.y;
			outTangentValuesX[1] = ( key.time + outInterval ) * mTimeFactor;
			outTangentValuesY[1] = key.val.y + key.outtan.y * outInterval;

			outInterval = ( nextTime - key.time ) * key.outLength.z;
			outTangentValuesX[2] = ( key.time + outInterval ) * mTimeFactor;
			outTangentValuesY[2] = key.val.z + key.outtan.z * outInterval;

		}

		else
		{
			/// @TODO: clarify if this makes sense or if we should export the same as above
			outTangentValuesX[0] = key.time * mTimeFactor;
			outTangentValuesX[1] = key.time * mTimeFactor;
			outTangentValuesX[2] = key.time * mTimeFactor;

			outTangentValuesY[0] = key.val.x;
			outTangentValuesY[1] = key.val.y;
			outTangentValuesY[2] = key.val.z;
		}
	}


	//---------------------------------------------------------------
	void AnimationExporter::getRotationSingleOutTangentPatchEuler ( float * outTangentValuesX, float * outTangentValuesY, IKeyControl * keyInterface, const int & keyIndex, const int & keyCount, const Animation & animation )
	{
		IBezQuatKey key;
		keyInterface->GetKey ( keyIndex, &key );

		float nextTime = ( float ) getNextTime ( keyIndex, keyCount, animation.getController() );

		//testing
		/*        bool b1 = key.flags & BEZKEY_UNCONSTRAINHANDLE;
		bool b2 = GetInTanType( key.flags ) != BEZKEY_USER;
		bool b3 = GetOutTanType( key.flags ) != BEZKEY_USER;
		bool b4 = GetOutTanType( key.flags ) == BEZKEY_LINEAR;
		bool b5 = GetOutTanType( key.flags ) == BEZKEY_STEP;
		*/
		InterpolationType interpolationOutType = getInterpolationType ( key.flags );

		int keyParamterIndex = animation.getType() - Animation::ROTATION_X;


		// export control point. If it is not a BEZIER interpolation, export the point itself

		size_t keyValueIndex = keyIndex;

		if ( interpolationOutType == BEZIER )
		{
			float outInterval = ( nextTime - key.time ) * DEFAULT_INLENGHT;
			*outTangentValuesX = ( key.time + outInterval ) * mTimeFactor;
			*outTangentValuesY = mKeyValueList[keyValueIndex];
		}

		else
		{
			/// @TODO: clarify if this makes sense or if we should export the same as above
			*outTangentValuesX = key.time * mTimeFactor;

			*outTangentValuesY = mKeyValueList[keyValueIndex];
		}
	}


	//---------------------------------------------------------------
	void AnimationExporter::getRotationOutTangentPatchEuler ( float * outTangentValuesX, float * outTangentValuesY, IKeyControl * keyInterface, const int & keyIndex, const int & keyCount, const Animation & animation )
	{
		IBezQuatKey key;
		keyInterface->GetKey ( keyIndex, &key );

		float nextTime = ( float ) getNextTime ( keyIndex, keyCount, animation.getController() );

		//testing
		/*        bool b1 = key.flags & BEZKEY_UNCONSTRAINHANDLE;
		bool b2 = GetInTanType( key.flags ) != BEZKEY_USER;
		bool b3 = GetOutTanType( key.flags ) != BEZKEY_USER;
		bool b4 = GetOutTanType( key.flags ) == BEZKEY_LINEAR;
		bool b5 = GetOutTanType( key.flags ) == BEZKEY_STEP;
		*/
		InterpolationType interpolationOutType = getInterpolationType ( key.flags );

		// export control point. If it is not a BEZIER interpolation, export the point itself

		size_t keyValueIndex = 3 * keyIndex;

		if ( interpolationOutType == BEZIER )
		{
			float outInterval = ( nextTime - key.time ) * DEFAULT_INLENGHT;

			outTangentValuesX[0] = ( key.time + outInterval ) * mTimeFactor;
			outTangentValuesY[0] = mKeyValueList[keyValueIndex++];

			outTangentValuesX[1] = outTangentValuesX[0];
			outTangentValuesY[1] = mKeyValueList[keyValueIndex++];

			outTangentValuesX[2] = outTangentValuesX[0];
			outTangentValuesY[2] = mKeyValueList[keyValueIndex++];
		}

		else
		{
			/// @TODO: clarify if this makes sense or if we should export the same as above
			outTangentValuesX[0] = key.time * mTimeFactor;
			outTangentValuesX[1] = outTangentValuesX[0];
			outTangentValuesX[2] = outTangentValuesX[0];

			outTangentValuesY[0] = mKeyValueList[keyValueIndex++];
			outTangentValuesY[1] = mKeyValueList[keyValueIndex++];
			outTangentValuesY[2] = mKeyValueList[keyValueIndex++];
		}
	}



    //---------------------------------------------------------------
    AnimationExporter::InterpolationType AnimationExporter::getInterpolationType ( const DWORD & flags )
    {
        switch ( GetOutTanType ( flags ) )
        {

        case BEZKEY_LINEAR:
            return LINEAR;

        case BEZKEY_STEP:
            return STEP;

        default:
            return BEZIER;
        }
    }

    //---------------------------------------------------------------
    void AnimationExporter::exportInterpolationSource ( const String & baseId, IKeyControl * keyInterface, InterpolationTypeFunctionPtr interpolationTypeFunction, int keyCount )
    {
        COLLADA::NameSource source ( mSW );
        source.setId ( baseId + INTERPOLATION_SOURCE_ID_SUFFIX );
        source.setArrayId ( baseId + INTERPOLATION_SOURCE_ID_SUFFIX + ARRAY_ID_SUFFIX );
        source.setAccessorStride ( 1 );
        source.getParameterNameList().push_back ( "INTERPOLATION" );
        source.setAccessorCount ( keyCount );
        source.prepareToAppendValues();

        for ( int i = 0; i < keyCount; ++i )
            source.appendValues ( interpolationTypeFunction ( keyInterface, i ) );

        source.finish();
    }

    //---------------------------------------------------------------
    template <const String & interpolationTypeName>
    const String & AnimationExporter::getUniformInterpolation ( IKeyControl * keyInterface, int keyIndex )
    {
        return interpolationTypeName;
    }

    //---------------------------------------------------------------
	template <class KeyType>
    const String & AnimationExporter::getBezierInterpolation ( IKeyControl * keyInterface, int keyIndex )
    {
        KeyType key;
        keyInterface->GetKey ( keyIndex, &key );
        return getNameOfInterpolation ( getInterpolationType ( key.flags ) );
    }



	//---------------------------------------------------------------
	void AnimationExporter::exportSamplingInputSource ( const String & baseId, TimeValue startTime, TimeValue endTime, int ticksPerFrame )
	{
		int keyCount = (endTime - startTime) / ticksPerFrame + 1;

		COLLADA::FloatSource source ( mSW );
		source.setId ( baseId + INPUT_SOURCE_ID_SUFFIX );
		source.setArrayId ( baseId + INPUT_SOURCE_ID_SUFFIX + ARRAY_ID_SUFFIX );
		source.setAccessorStride ( 1 );
		source.getParameterNameList().push_back ( "TIME" );
		source.setAccessorCount ( keyCount );
		source.prepareToAppendValues();


		for (TimeValue time = startTime; time < endTime; time += ticksPerFrame)
		{
			source.appendValues ( time / TIME_TICKSPERSEC );
		}

		source.finish();
	}



	//---------------------------------------------------------------
	void AnimationExporter::exportSamplingFloatOutputSource ( const Animation & animation, const String & baseId, IKeyControl* keyInterface, TimeValue startTime, TimeValue endTime, int ticksPerFrame )
	{
		int keyCount = (endTime - startTime) / ticksPerFrame + 1;

		COLLADA::FloatSource source ( mSW );
		source.setId ( baseId + OUTPUT_SOURCE_ID_SUFFIX );
		source.setArrayId ( baseId + OUTPUT_SOURCE_ID_SUFFIX + ARRAY_ID_SUFFIX );
		source.setAccessorStride ( 1 );
		source.getParameterNameList().push_back ( *(animation.getParameter()) );
		source.setAccessorCount ( keyCount );
		source.prepareToAppendValues();

		for (TimeValue time = startTime; time < endTime; time += ticksPerFrame)
		{
			float keyValue;
			animation.getController()->GetValue(time, &keyValue, FOREVER, CTRL_ABSOLUTE);
			source.appendValues ( keyValue );
		}

		source.finish();
	}


	//---------------------------------------------------------------
	void AnimationExporter::exportSamplingPoint3OutputSource ( const Animation & animation, const String & baseId, IKeyControl* keyInterface, TimeValue startTime, TimeValue endTime, int ticksPerFrame )
	{
		int keyCount = (endTime - startTime) / ticksPerFrame + 1;
		int keyLength = animation.getDimension();

		COLLADA::FloatSource source ( mSW );
		source.setId ( baseId + OUTPUT_SOURCE_ID_SUFFIX );
		source.setArrayId ( baseId + OUTPUT_SOURCE_ID_SUFFIX + ARRAY_ID_SUFFIX );
		source.setAccessorStride ( keyLength );

		for ( int i = 0; i < keyLength; ++i )
			source.getParameterNameList().push_back ( * ( animation.getParameter() + i ) );

		source.setAccessorCount ( keyCount );
		source.prepareToAppendValues();

		for (TimeValue time = startTime; time < endTime; time += ticksPerFrame)
		{

			Point3 keyValue;
			animation.getController()->GetValue(time, &keyValue, FOREVER, CTRL_ABSOLUTE);

			const Animation::ConversionFunction & conversionFunction = animation.getConversionFunction();

			if ( keyLength == 1)
			{
				if ( conversionFunction )
					source.appendValues ( conversionFunction ( keyValue[ animation.getType() - Animation::POSITION_X ] ) );
				else
					source.appendValues ( keyValue[ animation.getType() - Animation::POSITION_X ] );
			}
			else
			{
				for ( int j = 0; j < keyLength; ++j )
				{
					assert( keyLength == 3);

					if ( conversionFunction )
						source.appendValues ( conversionFunction ( keyValue[ j ] ) );
					else
						source.appendValues ( keyValue[ j ] );
				}
			}
		}
		source.finish();
	}


	//---------------------------------------------------------------
	void AnimationExporter::exportSamplingRotationOutputSource ( const Animation & animation, const String & baseId, IKeyControl* keyInterface, TimeValue startTime, TimeValue endTime, int ticksPerFrame )
	{
		int keyCount = (endTime - startTime) / ticksPerFrame + 1;
		int keyLength = animation.getDimension();

		COLLADA::FloatSource source ( mSW );
		source.setId ( baseId + OUTPUT_SOURCE_ID_SUFFIX );
		source.setArrayId ( baseId + OUTPUT_SOURCE_ID_SUFFIX + ARRAY_ID_SUFFIX );
		source.setAccessorStride ( keyLength );

		for ( int i = 0; i < keyLength; ++i )
			source.getParameterNameList().push_back ( * ( animation.getParameter() + i ) );

		source.setAccessorCount ( keyCount );
		source.prepareToAppendValues();

		Animation::ConversionFunction conversionFunction = animation.getConversionFunction();

		for (TimeValue time = startTime; time < endTime; time += ticksPerFrame)
		{
			AngAxis keyValue;
			animation.getController()->GetValue(time, &keyValue, FOREVER, CTRL_ABSOLUTE);

			float eulerAngles[ 3 ];
			Quat quaternion(keyValue);
			quaternion.GetEuler ( &eulerAngles[ 0 ], &eulerAngles[ 1 ], &eulerAngles[ 2 ] );
			if ( time > startTime)
				patchEuler(mPreviousEulerAngles, eulerAngles);

			mPreviousEulerAngles[0] = eulerAngles[0];
			mPreviousEulerAngles[1] = eulerAngles[1];
			mPreviousEulerAngles[2] = eulerAngles[2];

			if ( keyLength == 1)
			{
				if ( conversionFunction )
					source.appendValues( conversionFunction(eulerAngles[ animation.getType() - Animation::ROTATION_X ]) );
				else
					source.appendValues( eulerAngles[ animation.getType() - Animation::ROTATION_X ] );
			}
			else
			{
				if ( conversionFunction )
				{
					eulerAngles[0] = conversionFunction( eulerAngles[0] );
					eulerAngles[1] = conversionFunction( eulerAngles[1] );
					eulerAngles[2] = conversionFunction( eulerAngles[2] );
				}
				source.appendValues( eulerAngles[0], eulerAngles[1], eulerAngles[2] );
			}
		}
		source.finish();
	}



	//---------------------------------------------------------------
	void AnimationExporter::exportSamplingInterpolationSource ( const String & baseId, TimeValue startTime, TimeValue endTime, int ticksPerFrame )
	{
		int keyCount = (endTime - startTime) / ticksPerFrame + 1;

		COLLADA::NameSource source ( mSW );
		source.setId ( baseId + INTERPOLATION_SOURCE_ID_SUFFIX );
		source.setArrayId ( baseId + INTERPOLATION_SOURCE_ID_SUFFIX + ARRAY_ID_SUFFIX );
		source.setAccessorStride ( 1 );
		source.getParameterNameList().push_back ( "INTERPOLATION" );
		source.setAccessorCount ( keyCount );
		source.prepareToAppendValues();

		for (TimeValue time = startTime; time < endTime; time += ticksPerFrame)
			source.appendValues ( LINEAR_NAME );

		source.finish();
	}


    //---------------------------------------------------------------
    void AnimationExporter::exportSampler ( const Animation & animation )
    {

		if ( animation.hasAnyInputFlagsSet() )
		{
			String baseId = getBaseId ( animation );

			LibraryAnimations::Sampler sampler ( baseId + SAMPLER_ID_SUFFIX );

			if ( animation.inputFlagSet(Animation::INPUT) )
				sampler.addInput ( LibraryAnimations::Sampler::INPUT, "#" + baseId + INPUT_SOURCE_ID_SUFFIX );

			if ( animation.inputFlagSet(Animation::OUTPUT) )
				sampler.addInput ( LibraryAnimations::Sampler::OUTPUT, "#" + baseId + OUTPUT_SOURCE_ID_SUFFIX );

			if ( animation.inputFlagSet(Animation::IN_TANGENT) )
				sampler.addInput ( LibraryAnimations::Sampler::IN_TANGENT, "#" + baseId + INTANGENT_SOURCE_ID_SUFFIX );

			if ( animation.inputFlagSet(Animation::OUT_TANGENT) )
				sampler.addInput ( LibraryAnimations::Sampler::OUT_TANGENT, "#" + baseId + OUTTANGENT_SOURCE_ID_SUFFIX );

			if ( animation.inputFlagSet(Animation::INTERPOLATION) )
				sampler.addInput ( LibraryAnimations::Sampler::INTERPOLATION, "#" + baseId + INTERPOLATION_SOURCE_ID_SUFFIX );

			addSampler ( sampler );
		}
    }

    //---------------------------------------------------------------
    void AnimationExporter::exportChannel ( const Animation & animation )
    {
		if ( animation.hasAnyInputFlagsSet() )
		{
	        addChannel ( "#" + getBaseId ( animation ) + SAMPLER_ID_SUFFIX, getTarget ( animation ) );
		}
    }



	//---------------------------------------------------------------
	void AnimationExporter::angleApproach(float pval, float& val)
	{
		while (val - pval > COLLADA::MathUtils::COLLADA_PI_f) 
			val -= COLLADA::MathUtils::COLLADA_PI_f * 2.0f;
		while (val - pval < -COLLADA::MathUtils::COLLADA_PI_f) 
			val += COLLADA::MathUtils::COLLADA_PI_f * 2.0f;
	}


	//---------------------------------------------------------------
	void AnimationExporter::patchEuler(float* pval, float* val)
	{
		// Approach these Eulers to the previous value.
		for (int i = 0; i < 3; ++i) 
			angleApproach(pval[i], val[i]);

		float distanceSq = 0.0f; 
		for (int i = 0; i < 3; ++i) 
			distanceSq += (val[i] - pval[i]) * (val[i] - pval[i]);

		// All quaternions can be expressed two ways. Check if the second way is better.
		float alternative[3] = { val[0] + COLLADA::MathUtils::COLLADA_PI_f, COLLADA::MathUtils::COLLADA_PI_f - val[1], val[2] + COLLADA::MathUtils::COLLADA_PI_f };
		for (int i = 0; i < 3; ++i) 
			angleApproach(pval[i], alternative[i]);
		
		float alternateDistanceSq = 0.0f; for (int i = 0; i < 3; ++i) alternateDistanceSq += (alternative[i] - pval[i]) * (alternative[i] - pval[i]);

		if (alternateDistanceSq < distanceSq)
		{
			// Pick the alternative
			for (int i = 0; i < 3; ++i) 
				val[i] = alternative[i];
		}
	}

}
