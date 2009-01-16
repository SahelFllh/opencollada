/*
    Copyright (c) 2008 NetAllied Systems GmbH

    This file is part of MayaDataModel.

    Licensed under the MIT Open Source License,
    for details please see LICENSE file or the website
    http://www.opensource.org/licenses/mit-license.php
*/
#ifndef __MayaDM_ROCK_H__
#define __MayaDM_ROCK_H__
#include "MayaDMTypes.h"
#include "MayaDMConnectables.h"
#include "MayaDMTexture3d.h"
namespace MayaDM
{
class Rock : public Texture3d
{
public:
public:
	Rock(FILE* file,const std::string& name,const std::string& parent=""):Texture3d(file, name, parent, "rock"){}
	virtual ~Rock(){}
	void setColor1(const float3& c1)
	{
		if(c1 == float3(0.235f,0.0f,0.0f)) return;
		fprintf(mFile,"setAttr \".c1\" -type \"float3\" ");
		c1.write(mFile);
		fprintf(mFile,";\n");

	}
	void setColor1R(float c1r)
	{
		if(c1r == 0.0) return;
		fprintf(mFile,"setAttr \".c1.c1r\" %f;\n", c1r);

	}
	void setColor1G(float c1g)
	{
		if(c1g == 0.0) return;
		fprintf(mFile,"setAttr \".c1.c1g\" %f;\n", c1g);

	}
	void setColor1B(float c1b)
	{
		if(c1b == 0.0) return;
		fprintf(mFile,"setAttr \".c1.c1b\" %f;\n", c1b);

	}
	void setColor2(const float3& c2)
	{
		if(c2 == float3(1.0f,1.0f,1.0f)) return;
		fprintf(mFile,"setAttr \".c2\" -type \"float3\" ");
		c2.write(mFile);
		fprintf(mFile,";\n");

	}
	void setColor2R(float c2r)
	{
		if(c2r == 0.0) return;
		fprintf(mFile,"setAttr \".c2.c2r\" %f;\n", c2r);

	}
	void setColor2G(float c2g)
	{
		if(c2g == 0.0) return;
		fprintf(mFile,"setAttr \".c2.c2g\" %f;\n", c2g);

	}
	void setColor2B(float c2b)
	{
		if(c2b == 0.0) return;
		fprintf(mFile,"setAttr \".c2.c2b\" %f;\n", c2b);

	}
	void setGrainSize(float gs)
	{
		if(gs == 0.01) return;
		fprintf(mFile,"setAttr \".gs\" %f;\n", gs);

	}
	void setDiffusion(float di)
	{
		if(di == 1.0) return;
		fprintf(mFile,"setAttr \".di\" %f;\n", di);

	}
	void setMixRatio(float mr)
	{
		if(mr == 0.5) return;
		fprintf(mFile,"setAttr \".mr\" %f;\n", mr);

	}
	void getRefPointObj()
	{
		fprintf(mFile,"\"%s.rpo\"",mName.c_str());

	}
	void getRefPointObjX()
	{
		fprintf(mFile,"\"%s.rpo.rox\"",mName.c_str());

	}
	void getRefPointObjY()
	{
		fprintf(mFile,"\"%s.rpo.roy\"",mName.c_str());

	}
	void getRefPointObjZ()
	{
		fprintf(mFile,"\"%s.rpo.roz\"",mName.c_str());

	}
	void getRefPointCamera()
	{
		fprintf(mFile,"\"%s.rpc\"",mName.c_str());

	}
	void getRefPointCameraX()
	{
		fprintf(mFile,"\"%s.rpc.rcx\"",mName.c_str());

	}
	void getRefPointCameraY()
	{
		fprintf(mFile,"\"%s.rpc.rcy\"",mName.c_str());

	}
	void getRefPointCameraZ()
	{
		fprintf(mFile,"\"%s.rpc.rcz\"",mName.c_str());

	}
	void getColor1()
	{
		fprintf(mFile,"\"%s.c1\"",mName.c_str());

	}
	void getColor1R()
	{
		fprintf(mFile,"\"%s.c1.c1r\"",mName.c_str());

	}
	void getColor1G()
	{
		fprintf(mFile,"\"%s.c1.c1g\"",mName.c_str());

	}
	void getColor1B()
	{
		fprintf(mFile,"\"%s.c1.c1b\"",mName.c_str());

	}
	void getColor2()
	{
		fprintf(mFile,"\"%s.c2\"",mName.c_str());

	}
	void getColor2R()
	{
		fprintf(mFile,"\"%s.c2.c2r\"",mName.c_str());

	}
	void getColor2G()
	{
		fprintf(mFile,"\"%s.c2.c2g\"",mName.c_str());

	}
	void getColor2B()
	{
		fprintf(mFile,"\"%s.c2.c2b\"",mName.c_str());

	}
	void getGrainSize()
	{
		fprintf(mFile,"\"%s.gs\"",mName.c_str());

	}
	void getDiffusion()
	{
		fprintf(mFile,"\"%s.di\"",mName.c_str());

	}
	void getMixRatio()
	{
		fprintf(mFile,"\"%s.mr\"",mName.c_str());

	}
protected:
	Rock(FILE* file,const std::string& name,const std::string& parent,const std::string& nodeType):Texture3d(file, name, parent, nodeType) {}

};
}//namespace MayaDM
#endif//__MayaDM_ROCK_H__