/*
 * Copyright 2011, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef ATTRIBUTE_INDEXER_H
#define ATTRIBUTE_INDEXER_H


#include <SupportDefs.h>

#include <String.h>


class AttributeIndex;
class AttributeIndexTreeValue;
class IndexedAttributeOwner;


class AttributeIndexer {
public:
								AttributeIndexer(AttributeIndex* index);
								~AttributeIndexer();

			status_t			CreateCookie(IndexedAttributeOwner* owner,
									void* attributeCookie, uint32_t attributeType,
									size_t attributeSize, void*& _data,
									size_t& _toRead);
			void				DeleteCookie();

			AttributeIndexTreeValue* Cookie() const
									{ return fCookie; }

			const String&		IndexName() const
									{ return fIndexName; }

private:
			AttributeIndex*		fIndex;
			const String&		fIndexName;
			uint32_t				fIndexType;
			AttributeIndexTreeValue* fCookie;
};


#endif	// ATTRIBUTE_INDEX_H
