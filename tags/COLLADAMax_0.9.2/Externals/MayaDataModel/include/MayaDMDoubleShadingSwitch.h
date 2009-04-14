/*
    Copyright (c) 2008-2009 NetAllied Systems GmbH

    This file is part of MayaDataModel.

    Licensed under the MIT Open Source License,
    for details please see LICENSE file or the website
    http://www.opensource.org/licenses/mit-license.php
*/
#ifndef __MayaDM_DOUBLESHADINGSWITCH_H__
#define __MayaDM_DOUBLESHADINGSWITCH_H__
#include "MayaDMTypes.h"
#include "MayaDMConnectables.h"
#include "MayaDMBaseShadingSwitch.h"
namespace MayaDM
{
class DoubleShadingSwitch : public BaseShadingSwitch
{
public:
	struct Input{
		float2 inDouble;
		void write(FILE* file) const
		{
			inDouble.write(file);
			fprintf(file, " ");
		}
	};
public:

	DoubleShadingSwitch():BaseShadingSwitch(){}
	DoubleShadingSwitch(FILE* file,const std::string& name,const std::string& parent="",bool create=true)
		:BaseShadingSwitch(file, name, parent, "doubleShadingSwitch", create){}
	virtual ~DoubleShadingSwitch(){}

	void setInput(size_t i_i,const Input& i_)
	{
		fprintf(mFile,"\tsetAttr \".i[%i]\" ",i_i);
		i_.write(mFile);
		fprintf(mFile,";\n");
	}
	void setInDouble(size_t i_i,const float2& idl)
	{
		fprintf(mFile,"\tsetAttr \".i[%i].idl\" -type \"float2\" ",i_i);
		idl.write(mFile);
		fprintf(mFile,";\n");
	}
	void setInComp1(size_t i_i,float ic1)
	{
		if(ic1 == 0.0) return;
		fprintf(mFile,"\tsetAttr \".i[%i].idl.ic1\" %f;\n", i_i,ic1);
	}
	void setInComp2(size_t i_i,float ic2)
	{
		if(ic2 == 0.0) return;
		fprintf(mFile,"\tsetAttr \".i[%i].idl.ic2\" %f;\n", i_i,ic2);
	}
	void setDefault(const float2& def)
	{
		if(def == float2(0.0f,0.0f)) return;
		fprintf(mFile,"\tsetAttr \".def\" -type \"float2\" ");
		def.write(mFile);
		fprintf(mFile,";\n");
	}
	void setDefComp1(float dc1)
	{
		if(dc1 == 0.0) return;
		fprintf(mFile,"\tsetAttr \".def.dc1\" %f;\n", dc1);
	}
	void setDefComp2(float dc2)
	{
		if(dc2 == 0.0) return;
		fprintf(mFile,"\tsetAttr \".def.dc2\" %f;\n", dc2);
	}
	void getInput(size_t i_i)const
	{
		fprintf(mFile,"\"%s.i[%i]\"",mName.c_str(),i_i);
	}
	void getInput()const
	{

		fprintf(mFile,"\"%s.i\"",mName.c_str());
	}
	void getInDouble(size_t i_i)const
	{
		fprintf(mFile,"\"%s.i[%i].idl\"",mName.c_str(),i_i);
	}
	void getInDouble()const
	{

		fprintf(mFile,"\"%s.i.idl\"",mName.c_str());
	}
	void getInComp1(size_t i_i)const
	{
		fprintf(mFile,"\"%s.i[%i].idl.ic1\"",mName.c_str(),i_i);
	}
	void getInComp1()const
	{

		fprintf(mFile,"\"%s.i.idl.ic1\"",mName.c_str());
	}
	void getInComp2(size_t i_i)const
	{
		fprintf(mFile,"\"%s.i[%i].idl.ic2\"",mName.c_str(),i_i);
	}
	void getInComp2()const
	{

		fprintf(mFile,"\"%s.i.idl.ic2\"",mName.c_str());
	}
	void getInShape(size_t i_i)const
	{
		fprintf(mFile,"\"%s.i[%i].is\"",mName.c_str(),i_i);
	}
	void getInShape()const
	{

		fprintf(mFile,"\"%s.i.is\"",mName.c_str());
	}
	void getDefault()const
	{
		fprintf(mFile,"\"%s.def\"",mName.c_str());
	}
	void getDefComp1()const
	{
		fprintf(mFile,"\"%s.def.dc1\"",mName.c_str());
	}
	void getDefComp2()const
	{
		fprintf(mFile,"\"%s.def.dc2\"",mName.c_str());
	}
	void getOutput()const
	{
		fprintf(mFile,"\"%s.out\"",mName.c_str());
	}
	void getOutComp1()const
	{
		fprintf(mFile,"\"%s.out.oc1\"",mName.c_str());
	}
	void getOutComp2()const
	{
		fprintf(mFile,"\"%s.out.oc2\"",mName.c_str());
	}
protected:
	DoubleShadingSwitch(FILE* file,const std::string& name,const std::string& parent,const std::string& nodeType,bool create=true)
		:BaseShadingSwitch(file, name, parent, nodeType, create) {}

};
}//namespace MayaDM
#endif//__MayaDM_DOUBLESHADINGSWITCH_H__