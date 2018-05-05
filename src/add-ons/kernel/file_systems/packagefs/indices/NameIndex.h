/*
 * Copyright 2011, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef NAME_INDEX_H
#define NAME_INDEX_H


#include <Index.h>
#include <NodeListener.h>


template<typename Policy> class GenericIndexIterator;


class NameIndex : public Index, private NodeListener {
public:
								NameIndex();
	virtual						~NameIndex();

			status_t			Init(Volume* volume);

	virtual	int32_t				CountEntries() const;

private:
	virtual	void				NodeAdded(Node* node);
	virtual	void				NodeRemoved(Node* node);
	virtual	void				NodeChanged(Node* node, uint32_t statFields,
									const OldNodeAttributes& oldAttributes);

protected:
	virtual	AbstractIndexIterator* InternalGetIterator();
	virtual	AbstractIndexIterator* InternalFind(const void* key,
									size_t length);

private:
			class EntryTree;
			struct IteratorPolicy;
			struct Iterator;

			friend class IteratorPolicy;

			void				_UpdateLiveQueries(Node* entry,
									const char* oldName, const char* newName);

private:
			EntryTree*			fEntries;
};


#endif	// NAME_INDEX_H
