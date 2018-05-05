/*
 * Copyright 2009-2011, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include <PackageNodeAttribute.h>

#include <stdlib.h>
#include <string.h>


PackageNodeAttribute::PackageNodeAttribute(uint32_t type,
	const PackageData& data)
	:
	fData(data),
	fName(),
	fIndexCookie(NULL),
	fType(type)
{
}


PackageNodeAttribute::~PackageNodeAttribute()
{
}


void
PackageNodeAttribute::Init(const String& name)
{
	fName = name;
}
