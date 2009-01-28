/*
    Copyright (c) 2008 NetAllied Systems GmbH

    This file is part of MayaDataModel.

    Licensed under the MIT Open Source License,
    for details please see LICENSE file or the website
    http://www.opensource.org/licenses/mit-license.php
*/
#ifndef __MayaDM_ROUNDCONSTANTRADIUS_H__
#define __MayaDM_ROUNDCONSTANTRADIUS_H__
#include "MayaDMTypes.h"
#include "MayaDMConnectables.h"
#include "MayaDMAbstractBaseCreate.h"
namespace MayaDM
{
class RoundConstantRadius : public AbstractBaseCreate
{
public:
	struct Edge{
		nurbsCurve* inputCurveA;
		nurbsCurve* inputCurveB;
		int* inSurfIdxA;
		int* inSurfIdxB;
		bool edgeValid;
		void write(FILE* file) const
		{
			size_t size = sizeof(inputCurveA)/sizeof(nurbsCurve);
			for(size_t i=0; i<size; ++i)
			{
				inputCurveA[i].write(file);
				fprintf(file, " ");
				if(i+1<size) fprintf(file," ");
			}
			size = sizeof(inputCurveB)/sizeof(nurbsCurve);
			for(size_t i=0; i<size; ++i)
			{
				inputCurveB[i].write(file);
				fprintf(file, " ");
				if(i+1<size) fprintf(file," ");
			}
			size = sizeof(inSurfIdxA)/sizeof(int);
			for(size_t i=0; i<size; ++i)
			{
				fprintf(file,"%i ", inSurfIdxA[i]);
				if(i+1<size) fprintf(file," ");
			}
			size = sizeof(inSurfIdxB)/sizeof(int);
			for(size_t i=0; i<size; ++i)
			{
				fprintf(file,"%i ", inSurfIdxB[i]);
				if(i+1<size) fprintf(file," ");
			}
			fprintf(file,"%i", edgeValid);
		}
	};
public:
	RoundConstantRadius(FILE* file,const std::string& name,const std::string& parent=""):AbstractBaseCreate(file, name, parent, "roundConstantRadius"){}
	virtual ~RoundConstantRadius(){}
	void setInputSurface(size_t is_i,const nurbsSurface& is)
	{
		fprintf(mFile,"setAttr \".is[%i]\" -type \"nurbsSurface\" ",is_i);
		is.write(mFile);
		fprintf(mFile,";\n");

	}
	void setInputSurface(size_t is_start,size_t is_end,nurbsSurface* is)
	{
		fprintf(mFile,"setAttr \".is[%i:%i]\" ", is_start,is_end);
		size_t size = (is_end-is_start)*1+1;
		for(size_t i=0;i<size;++i)
		{
			is[i].write(mFile);
			fprintf(mFile,"\n");
		}
		fprintf(mFile,";\n");

	}
	void startInputSurface(size_t is_start,size_t is_end)
	{
		fprintf(mFile,"setAttr \".is[%i:%i]\"",is_start,is_end);
		fprintf(mFile," -type \"nurbsSurface\" ");

	}
	void appendInputSurface(const nurbsSurface& is)
	{
		fprintf(mFile,"\n");
		is.write(mFile);

	}
	void endInputSurface()
	{
		fprintf(mFile,";\n");

	}
	void setRadius(size_t r_i,double r)
	{
		if(r == 1) return;
		fprintf(mFile,"setAttr \".r[%i]\" %f;\n", r_i,r);

	}
	void setRadius(size_t r_start,size_t r_end,double* r)
	{
		fprintf(mFile,"setAttr \".r[%i:%i]\" ", r_start,r_end);
		size_t size = (r_end-r_start)*1+1;
		for(size_t i=0;i<size;++i)
		{
			fprintf(mFile,"%f",r[i]);
			if(i+1<size) fprintf(mFile," ");
		}
		fprintf(mFile,";\n");

	}
	void startRadius(size_t r_start,size_t r_end)
	{
		fprintf(mFile,"setAttr \".r[%i:%i]\"",r_start,r_end);

	}
	void appendRadius(double r)
	{
		fprintf(mFile," %f",r);

	}
	void endRadius()
	{
		fprintf(mFile,";\n");

	}
	void setEdge(size_t e_i,const Edge& e)
	{
		fprintf(mFile,"setAttr \".e[%i]\" ",e_i);
		e.write(mFile);
		fprintf(mFile,";\n");

	}
	void setInputCurveA(size_t e_i,size_t ica_i,const nurbsCurve& ica)
	{
		fprintf(mFile,"setAttr \".e[%i].ica[%i]\" -type \"nurbsCurve\" ",e_i,ica_i);
		ica.write(mFile);
		fprintf(mFile,";\n");

	}
	void setInputCurveA(size_t e_i,size_t ica_start,size_t ica_end,nurbsCurve* ica)
	{
		fprintf(mFile,"setAttr \".e[%i].ica[%i:%i]\" ", e_i,ica_start,ica_end);
		size_t size = (ica_end-ica_start)*1+1;
		for(size_t i=0;i<size;++i)
		{
			ica[i].write(mFile);
			fprintf(mFile,"\n");
		}
		fprintf(mFile,";\n");

	}
	void startInputCurveA(size_t e_i,size_t ica_start,size_t ica_end)
	{
		fprintf(mFile,"setAttr \".e[%i].ica[%i:%i]\"",e_i,ica_start,ica_end);
		fprintf(mFile," -type \"nurbsCurve\" ");

	}
	void appendInputCurveA(const nurbsCurve& ica)
	{
		fprintf(mFile,"\n");
		ica.write(mFile);

	}
	void endInputCurveA()
	{
		fprintf(mFile,";\n");

	}
	void setInputCurveB(size_t e_i,size_t icb_i,const nurbsCurve& icb)
	{
		fprintf(mFile,"setAttr \".e[%i].icb[%i]\" -type \"nurbsCurve\" ",e_i,icb_i);
		icb.write(mFile);
		fprintf(mFile,";\n");

	}
	void setInputCurveB(size_t e_i,size_t icb_start,size_t icb_end,nurbsCurve* icb)
	{
		fprintf(mFile,"setAttr \".e[%i].icb[%i:%i]\" ", e_i,icb_start,icb_end);
		size_t size = (icb_end-icb_start)*1+1;
		for(size_t i=0;i<size;++i)
		{
			icb[i].write(mFile);
			fprintf(mFile,"\n");
		}
		fprintf(mFile,";\n");

	}
	void startInputCurveB(size_t e_i,size_t icb_start,size_t icb_end)
	{
		fprintf(mFile,"setAttr \".e[%i].icb[%i:%i]\"",e_i,icb_start,icb_end);
		fprintf(mFile," -type \"nurbsCurve\" ");

	}
	void appendInputCurveB(const nurbsCurve& icb)
	{
		fprintf(mFile,"\n");
		icb.write(mFile);

	}
	void endInputCurveB()
	{
		fprintf(mFile,";\n");

	}
	void setInSurfIdxA(size_t e_i,size_t isa_i,int isa)
	{
		if(isa == 123456) return;
		fprintf(mFile,"setAttr \".e[%i].isa[%i]\" %i;\n", e_i,isa_i,isa);

	}
	void setInSurfIdxA(size_t e_i,size_t isa_start,size_t isa_end,int* isa)
	{
		fprintf(mFile,"setAttr \".e[%i].isa[%i:%i]\" ", e_i,isa_start,isa_end);
		size_t size = (isa_end-isa_start)*1+1;
		for(size_t i=0;i<size;++i)
		{
			fprintf(mFile,"%i",isa[i]);
			if(i+1<size) fprintf(mFile," ");
		}
		fprintf(mFile,";\n");

	}
	void startInSurfIdxA(size_t e_i,size_t isa_start,size_t isa_end)
	{
		fprintf(mFile,"setAttr \".e[%i].isa[%i:%i]\"",e_i,isa_start,isa_end);

	}
	void appendInSurfIdxA(int isa)
	{
		fprintf(mFile," %i",isa);

	}
	void endInSurfIdxA()
	{
		fprintf(mFile,";\n");

	}
	void setInSurfIdxB(size_t e_i,size_t isb_i,int isb)
	{
		if(isb == 123456) return;
		fprintf(mFile,"setAttr \".e[%i].isb[%i]\" %i;\n", e_i,isb_i,isb);

	}
	void setInSurfIdxB(size_t e_i,size_t isb_start,size_t isb_end,int* isb)
	{
		fprintf(mFile,"setAttr \".e[%i].isb[%i:%i]\" ", e_i,isb_start,isb_end);
		size_t size = (isb_end-isb_start)*1+1;
		for(size_t i=0;i<size;++i)
		{
			fprintf(mFile,"%i",isb[i]);
			if(i+1<size) fprintf(mFile," ");
		}
		fprintf(mFile,";\n");

	}
	void startInSurfIdxB(size_t e_i,size_t isb_start,size_t isb_end)
	{
		fprintf(mFile,"setAttr \".e[%i].isb[%i:%i]\"",e_i,isb_start,isb_end);

	}
	void appendInSurfIdxB(int isb)
	{
		fprintf(mFile," %i",isb);

	}
	void endInSurfIdxB()
	{
		fprintf(mFile,";\n");

	}
	void setEdgeValid(size_t e_i,bool ev)
	{
		if(ev == 1) return;
		fprintf(mFile,"setAttr \".e[%i].ev\" %i;\n", e_i,ev);

	}
	void setTolerance(double tol)
	{
		if(tol == 0) return;
		fprintf(mFile,"setAttr \".tol\" %f;\n", tol);

	}
	void getInputSurface(size_t is_i)
	{
		fprintf(mFile,"\"%s.is[%i]\"",mName.c_str(),is_i);

	}
	void getRadius(size_t r_i)
	{
		fprintf(mFile,"\"%s.r[%i]\"",mName.c_str(),r_i);

	}
	void getInputCurveA(size_t e_i,size_t ica_i)
	{
		fprintf(mFile,"\"%s.e[%i].ica[%i]\"",mName.c_str(),e_i,ica_i);

	}
	void getInputCurveB(size_t e_i,size_t icb_i)
	{
		fprintf(mFile,"\"%s.e[%i].icb[%i]\"",mName.c_str(),e_i,icb_i);

	}
	void getTolerance()
	{
		fprintf(mFile,"\"%s.tol\"",mName.c_str());

	}
	void getFilletStatus(size_t fis_i)
	{
		fprintf(mFile,"\"%s.fis[%i]\"",mName.c_str(),fis_i);

	}
	void getOriginalSurface(size_t os_i)
	{
		fprintf(mFile,"\"%s.os[%i]\"",mName.c_str(),os_i);

	}
	void getFilletSurface(size_t fs_i)
	{
		fprintf(mFile,"\"%s.fs[%i]\"",mName.c_str(),fs_i);

	}
	void getCornerSurface(size_t cs_i)
	{
		fprintf(mFile,"\"%s.cs[%i]\"",mName.c_str(),cs_i);

	}
protected:
	RoundConstantRadius(FILE* file,const std::string& name,const std::string& parent,const std::string& nodeType):AbstractBaseCreate(file, name, parent, nodeType) {}

};
}//namespace MayaDM
#endif//__MayaDM_ROUNDCONSTANTRADIUS_H__