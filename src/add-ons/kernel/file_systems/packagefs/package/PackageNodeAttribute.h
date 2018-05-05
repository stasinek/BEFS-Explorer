/*
 * Copyright 2009-2011, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef PACKAGE_NODE_ATTRIBUTE_H
#define PACKAGE_NODE_ATTRIBUTE_H


#include <util/DoublyLinkedList.h>

#include <PackageData.h>

#include <String.h>


class PackageNode;


class PackageNodeAttribute
	: public DoublyLinkedListLinkImpl<PackageNodeAttribute> {
public:
								PackageNodeAttribute(uint32_t type,
									const PackageData& data);
								~PackageNodeAttribute();

			const String&		Name() const	{ return fName; }
			uint32_t				Type() const	{ return fType; }
			const PackageData&	Data() const	{ return fData; }

			void				Init(const String& name);

			void				SetIndexCookie(void* cookie)
									{ fIndexCookie = cookie; }
			void*				IndexCookie() const
									{ return fIndexCookie; }

protected:
			PackageData			fData;
			String				fName;
			void*				fIndexCookie;
			uint32_t				fType;
};


typedef DoublyLinkedList<PackageNodeAttribute> PackageNodeAttributeList;


#endif	// PACKAGE_NODE_ATTRIBUTE_H
