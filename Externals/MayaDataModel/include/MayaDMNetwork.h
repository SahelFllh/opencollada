/*
    Copyright (c) 2008 NetAllied Systems GmbH

    This file is part of MayaDataModel.

    Licensed under the MIT Open Source License,
    for details please see LICENSE file or the website
    http://www.opensource.org/licenses/mit-license.php
*/
#ifndef __MayaDM_NETWORK_H__
#define __MayaDM_NETWORK_H__
#include "MayaDMTypes.h"
#include "MayaDMConnectables.h"
#include "MayaDMDependNode.h"
namespace MayaDM
{
class Network : public DependNode
{
public:
public:
	Network(FILE* file,const std::string& name,const std::string& parent=""):DependNode(file, name, parent, "network"){}
	virtual ~Network(){}
	void getAffects(size_t a_i)
	{
		fprintf(mFile,"\"%s.a[%i]\"",mName.c_str(),a_i);

	}
	void getAffectedBy(size_t ab_i)
	{
		fprintf(mFile,"\"%s.ab[%i]\"",mName.c_str(),ab_i);

	}
protected:
	Network(FILE* file,const std::string& name,const std::string& parent,const std::string& nodeType):DependNode(file, name, parent, nodeType) {}

};
}//namespace MayaDM
#endif//__MayaDM_NETWORK_H__