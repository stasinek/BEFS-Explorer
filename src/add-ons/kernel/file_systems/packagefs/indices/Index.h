/*
 * Copyright 2011, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef INDEX_H
#define INDEX_H


#include <string.h>

#include <SupportDefs.h>

#include <util/OpenHashTable.h>

#include <String.h>
#include <StringKey.h>


class AbstractIndexIterator;
class IndexIterator;
class Node;
class Volume;


static const size_t kMaxIndexKeyLength = 256;


class Index {
public:
								Index();
	virtual						~Index();

			status_t			Init(Volume* volume, const char* name,
									uint32_t type, bool fixedKeyLength,
									size_t keyLength = 0);

			Volume*				GetVolume() const		{ return fVolume; }

			const String&		Name() const			{ return fName; }
			uint32_t				Type() const			{ return fType; }
			bool				HasFixedKeyLength() const
									{ return fFixedKeyLength; }
			size_t				KeyLength() const		{ return fKeyLength; }

	virtual	int32_t				CountEntries() const = 0;

			bool				GetIterator(IndexIterator& iterator);
			bool				Find(const void* key, size_t length,
									IndexIterator& iterator);
									// sets the iterator to the first value
									// >= key

			Index*&				IndexHashLink()
									{ return fHashLink; }

			// debugging
			void				Dump();

protected:
	virtual	AbstractIndexIterator* InternalGetIterator() = 0;
	virtual	AbstractIndexIterator* InternalFind(const void* key,
									size_t length) = 0;
									// returns an iterator pointing to the first
									// value >= key

protected:
			Index*				fHashLink;
			Volume*				fVolume;
			String				fName;
			uint32_t				fType;
			size_t				fKeyLength;
			bool				fFixedKeyLength;
};


class IndexIterator {
public:
								IndexIterator();
								~IndexIterator();

			bool				HasNext() const;
			Node*				Next();
			Node*				Next(void* buffer, size_t* _keyLength);

			status_t			Suspend();
			status_t			Resume();

private:
			void				SetIterator(AbstractIndexIterator* iterator);

private:
			friend class Index;

private:
			AbstractIndexIterator* fIterator;
};


// #pragma mark - IndexHashDefinition


struct IndexHashDefinition {
	typedef StringKey	KeyType;
	typedef	Index		ValueType;

	size_t HashKey(const StringKey& key) const
	{
		return key.Hash();
	}

	size_t Hash(const Index* value) const
	{
		return value->Name().Hash();
	}

	bool Compare(const StringKey& key, const Index* value) const
	{
		return key == value->Name();
	}

	Index*& GetLink(Index* value) const
	{
		return value->IndexHashLink();
	}
};


typedef BOpenHashTable<IndexHashDefinition> IndexHashTable;


#endif	// INDEX_H
