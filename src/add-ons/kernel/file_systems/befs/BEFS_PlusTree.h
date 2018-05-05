/*
 * Copyright 2001-2007, Axel Dörfler, axeld@pinc-software.de.
 * This file may be used under the terms of the MIT License.
 */
#ifndef BEFS_PLUS_TREE_H
#define BEFS_PLUS_TREE_H

//---------------------------------------------------------------------------
#include "BEFS.h"
#include "BEFS_Journal.h"
#include "BEFS_Chain.h"
//---------------------------------------------------------------------------
//	#pragma mark - on-disk structures
struct bplustree_node;
//---------------------------------------------------------------------------
#define BPLUSTREE_NULL			-1
#define BPLUSTREE_FREE			-2
//---------------------------------------------------------------------------
struct bplustree_header {
	uint32_t		magic;
	uint32_t		node_size;
	uint32_t		max_number_of_levels;
	uint32_t		data_type;
    int64_t		root_node_pointer;
    int64_t		free_node_pointer;
    int64_t		maximum_size;

    uint32_t Magic() const
    {      return BEFS_ENDIAN_TO_HOST_INT32(magic); }
    uint32_t NodeSize() const
    {      return BEFS_ENDIAN_TO_HOST_INT32(node_size); }
    uint32_t DataType() const
    {      return BEFS_ENDIAN_TO_HOST_INT32(data_type); }
    int64_t RootNode() const
    {      return BEFS_ENDIAN_TO_HOST_INT64(root_node_pointer); }
    int64_t FreeNode() const
    {      return BEFS_ENDIAN_TO_HOST_INT64(free_node_pointer); }
    int64_t MaximumSize() const
    {      return BEFS_ENDIAN_TO_HOST_INT64(maximum_size); }
	uint32_t MaxNumberOfLevels() const
    {      return BEFS_ENDIAN_TO_HOST_INT32(max_number_of_levels); }

	inline bool CheckNode(bplustree_node *node) const;
    inline bool IsValidLink(int64_t link) const;
} /* _PACKED*/;
//---------------------------------------------------------------------------
#define BPLUSTREE_MAGIC 			0x69f6c2e8
#define BPLUSTREE_NODE_SIZE 		1024
#define BPLUSTREE_MAX_KEY_LENGTH	256
#define BPLUSTREE_MIN_KEY_LENGTH	1
//---------------------------------------------------------------------------
enum bplustree_types {
        BPLUSTREE_STRING_TYPE	= 0,
        BPLUSTREE_INT32_TYPE	= 1,
        BPLUSTREE_UINT32_TYPE	= 2,
        BPLUSTREE_INT64_TYPE	= 3,
        BPLUSTREE_UINT64_TYPE	= 4,
        BPLUSTREE_FLOAT_TYPE	= 5,
        BPLUSTREE_DOUBLE_TYPE	= 6
};
//---------------------------------------------------------------------------
struct sorted_array; typedef sorted_array duplicate_array;
//---------------------------------------------------------------------------
struct bplustree_node {
    int64_t	left_link;
    int64_t	right_link;
    int64_t	overflow_link;
	uint16_t	all_key_count;
	uint16_t	all_key_length;

        int64_t LeftLink() const { return BEFS_ENDIAN_TO_HOST_INT64(left_link); }
        int64_t RightLink() const { return BEFS_ENDIAN_TO_HOST_INT64(right_link); }
        int64_t OverflowLink() const { return BEFS_ENDIAN_TO_HOST_INT64(overflow_link); }
    uint16_t NumKeys() const { return BEFS_ENDIAN_TO_HOST_INT16(all_key_count); }
    uint16_t AllKeyLength() const { return BEFS_ENDIAN_TO_HOST_INT16(all_key_length); }

    inline uint16_t *KeyLengths() const;
    inline int64_t *Values() const;
    inline uint8_t *Keys() const;
    inline int32_t Used() const;
    uint8_t *KeyAt(int32_t index, uint16_t *keyLength) const;

	inline bool IsLeaf() const;
	void Initialize();
    uint8_t CountDuplicates(int64_t offset, bool isFragment) const;
    int64_t DuplicateAt(int64_t offset, bool isFragment, int8_t index) const;
	uint32_t FragmentsUsed(uint32_t nodeSize) const;
	inline duplicate_array *FragmentAt(int8_t index) const;
	inline duplicate_array *DuplicateArray() const;
    static inline uint8_t LinkType(int64_t link);
    static inline int64_t MakeLink(uint8_t type, int64_t link, uint32_t fragmentIndex = 0);
    static inline bool IsDuplicate(int64_t link);
    static inline int64_t FragmentOffset(int64_t link);
    static inline uint32_t FragmentIndex(int64_t link);
	static inline uint32_t MaxFragments(uint32_t nodeSize);
#ifdef DEBUG
	status_t CheckIntegrity(uint32_t nodeSize) const;
#endif
}/* _PACKED*/;

//---------------------------------------------------------------------------
//#define BPLUSTREE_NODE 0
#define BPLUSTREE_DUPLICATE_NODE 2
#define BPLUSTREE_DUPLICATE_FRAGMENT 3
//---------------------------------------------------------------------------
#define NUM_FRAGMENT_VALUES 7
#define NUM_DUPLICATE_VALUES 125
//---------------------------------------------------------------------------
enum bplustree_traversing {
	BPLUSTREE_FORWARD = 1,
	BPLUSTREE_BACKWARD = -1,
	
	BPLUSTREE_BEGIN = 0,
	BPLUSTREE_END = 1
};
//---------------------------------------------------------------------------
//	#pragma mark - in-memory structures
template<class T> class Stack;
struct node_and_key {
    int64_t	nodeOffset;
    uint16_t	keyIndex;
};
//---------------------------------------------------------------------------
class BPlusTree;
class TreeIterator;
class CachedNode;
class Inode;
// needed for searching (utilizing a stack)
class CachedNode {
	public:
		CachedNode(BPlusTree *tree)
            : fTree(tree),fNode(NULL)
		{
		}

        CachedNode(BPlusTree *tree, int64_t offset, bool check = true)
            : fTree(tree), fNode(NULL)
		{
			SetTo(offset, check);
		}

		~CachedNode()
		{
			Unset();
		}

        const bplustree_node *SetTo(int64_t offset, bool check = true);
        bplustree_node *SetToWritable(Transaction &transaction, int64_t offset,
			bool check = true);
		bplustree_node *MakeWritable(Transaction &transaction);
		const bplustree_header *SetToHeader();
		bplustree_header *SetToWritableHeader(Transaction &transaction);
		bplustree_header *MakeWritableHeader(Transaction &transaction);
		void UnsetUnchanged(Transaction &transaction);
		void Unset();
        status_t Free(Transaction &transaction, int64_t offset);
		status_t Allocate(Transaction &transaction, bplustree_node **node,
            int64_t *offset);
		bool IsWritable() const { return fWritable; }
		bplustree_node *Node() const { return fNode; }
protected:
        bplustree_node	*InternalSetTo(Transaction *transaction, int64_t offset);
		BPlusTree		*fTree;
		bplustree_node	*fNode;
        int64_t			fOffset;
        int64_t			fBlockNumber;
		bool			fWritable;
};
//---------------------------------------------------------------------------

class BPlusTree {
	public:
		BPlusTree(Transaction &transaction, Inode *stream,
			int32_t nodeSize = BPLUSTREE_NODE_SIZE);
		BPlusTree(Inode *stream);
		BPlusTree();
		~BPlusTree();
		status_t	SetTo(Transaction &transaction, Inode *stream,
						int32_t nodeSize = BPLUSTREE_NODE_SIZE);
		status_t	SetTo(Inode *stream);
		status_t	SetStream(Inode *stream);
		status_t	InitCheck();
		status_t	Validate();
		status_t	Remove(Transaction &transaction, const uint8_t *key,
                        uint16_t keyLength, int64_t value);
		status_t	Insert(Transaction &transaction, const uint8_t *key,
                        uint16_t keyLength, int64_t value);
		status_t	Remove(Transaction &transaction, const char *key,
                        int64_t value);
		status_t	Insert(Transaction &transaction, const char *key,
                        int64_t value);
        status_t	Insert(Transaction &transaction, int32_t key, int64_t value);
        status_t	Insert(Transaction &transaction, uint32_t key, int64_t value);
        status_t	Insert(Transaction &transaction, int64_t key, int64_t value);
        status_t	Insert(Transaction &transaction, uint64_t key, int64_t value);
        status_t	Insert(Transaction &transaction, float key, int64_t value);
        status_t	Insert(Transaction &transaction, double key, int64_t value);
		status_t	Replace(Transaction &transaction, const uint8_t *key,
                        uint16_t keyLength, int64_t value);
        status_t	Find(const uint8_t *key, uint16_t keyLength, int64_t *value);
		static int32_t TypeCodeToKeyType(type_code code);
		static int32_t ModeToKeyType(mode_t mode);
		//TODO: this is temporary
		const bplustree_header* Header(){ return fHeader; };
private:
        BPlusTree(const BPlusTree &);
		BPlusTree &operator=(const BPlusTree &);
			// no implementation
		int32_t		_CompareKeys(const void *key1, int keylength1,
						const void *key2, int keylength2);
		status_t	_FindKey(const bplustree_node *node, const uint8_t *key,
						uint16_t keyLength, uint16_t *index = NULL,
                        int64_t *next = NULL);
		status_t	_SeekDown(Stack<node_and_key> &stack, const uint8_t *key,
						uint16_t keyLength);
		status_t	_FindFreeDuplicateFragment(Transaction &transaction, 
						const bplustree_node *node, CachedNode &cached,
                        int64_t *_offset, bplustree_node **_fragment,
						uint32_t *_index);
		status_t	_InsertDuplicate(Transaction &transaction,
						CachedNode &cached, const bplustree_node *node,
                        uint16_t index, int64_t value);
		void		_InsertKey(bplustree_node *node, uint16_t index, uint8_t *key,
                        uint16_t keyLength, int64_t value);
        status_t	_SplitNode(bplustree_node *node, int64_t nodeOffset,
                        bplustree_node *other, int64_t otherOffset,
						uint16_t *_keyIndex, uint8_t *key, uint16_t *_keyLength,
                        int64_t *_value);
		status_t	_RemoveDuplicate(Transaction &transaction,
						const bplustree_node *node, CachedNode &cached,
                        uint16_t keyIndex, int64_t value);
		void		_RemoveKey(bplustree_node *node, uint16_t index);
        void		_UpdateIterators(int64_t offset, int64_t nextOffset,
						uint16_t keyIndex, uint16_t splitAt, int8_t change);
		void		_AddIterator(TreeIterator *iterator);
		void		_RemoveIterator(TreeIterator *iterator);
	private:
		friend class TreeIterator;
		friend class CachedNode;
		Inode		*fStream;
		const bplustree_header *fHeader;
		CachedNode	fCachedHeader;
		int32_t		fNodeSize;
		bool		fAllowDuplicates;
		status_t	fStatus;
		SimpleLock	fIteratorLock;
		Chain<TreeIterator> fIterators;
};
//---------------------------------------------------------------------------
//	#pragma mark - helper classes/functions
extern int32_t compareKeys(type_code type, const void *key1, int keyLength1, const void *key2, int keyLength2);
class TreeIterator {
public:
		TreeIterator(BPlusTree *tree);
		~TreeIterator();

		status_t	Goto(int8_t to);
		status_t	Traverse(int8_t direction, void *key, uint16_t *keyLength,
                        uint16_t maxLength, int64_t *value,
						uint16_t *duplicate = NULL);
		status_t	Find(const uint8_t *key, uint16_t keyLength);

		status_t	Rewind();
		status_t	GetNextEntry(void *key, uint16_t *keyLength, uint16_t maxLength,
                        int64_t *value, uint16_t *duplicate = NULL);
		status_t	GetPreviousEntry(void *key, uint16_t *keyLength,
                        uint16_t maxLength, int64_t *value,
						uint16_t *duplicate = NULL);
		void		SkipDuplicates();
#ifdef DEBUG
		void		Dump();
#endif
private:
        BPlusTree  *fTree;
        int64_t		fCurrentNodeOffset;	// traverse position
		int32_t		fCurrentKey;
        int64_t		fDuplicateNode;
		uint16_t		fDuplicate, fNumDuplicates;
		bool		fIsFragment;
private:
		friend class Chain<TreeIterator>;
		friend class BPlusTree;
        void		Update(int64_t offset, int64_t nextOffset, uint16_t keyIndex, uint16_t splitAt, int8_t change);
		void Stop();
		TreeIterator *fNext;
};
//	#pragma mark - BPlusTree's inline functions
//	(most of them may not be needed)
//---------------------------------------------------------------------------

inline status_t BPlusTree::Remove(Transaction &transaction, const char *key, int64_t value)
{
	if (fHeader->data_type != BPLUSTREE_STRING_TYPE)
		return B_BAD_TYPE;
	return Remove(transaction, (uint8_t *)key, strlen(key), value);
}
//---------------------------------------------------------------------------

inline status_t BPlusTree::Insert(Transaction &transaction, const char *key, int64_t value)
{
	if (fHeader->data_type != BPLUSTREE_STRING_TYPE)
		return B_BAD_TYPE;
	return Insert(transaction, (uint8_t *)key, strlen(key), value);
}
//---------------------------------------------------------------------------

inline status_t BPlusTree::Insert(Transaction &transaction, int32_t key, int64_t value)
{
	if (fHeader->data_type != BPLUSTREE_INT32_TYPE)
		return B_BAD_TYPE;
	return Insert(transaction, (uint8_t *)&key, sizeof(key), value);
}
//---------------------------------------------------------------------------

inline status_t BPlusTree::Insert(Transaction &transaction, uint32_t key, int64_t value)
{
	if (fHeader->data_type != BPLUSTREE_UINT32_TYPE)
		return B_BAD_TYPE;
	return Insert(transaction, (uint8_t *)&key, sizeof(key), value);
}
//---------------------------------------------------------------------------

inline status_t BPlusTree::Insert(Transaction &transaction, int64_t key, int64_t value)
{
	if (fHeader->data_type != BPLUSTREE_INT64_TYPE)
		return B_BAD_TYPE;
	return Insert(transaction, (uint8_t *)&key, sizeof(key), value);
}
//---------------------------------------------------------------------------

inline status_t BPlusTree::Insert(Transaction &transaction, uint64_t key, int64_t value)
{
	if (fHeader->data_type != BPLUSTREE_UINT64_TYPE)
		return B_BAD_TYPE;
	return Insert(transaction, (uint8_t *)&key, sizeof(key), value);
}
//---------------------------------------------------------------------------

inline status_t BPlusTree::Insert(Transaction &transaction, float key, int64_t value)
{
	if (fHeader->data_type != BPLUSTREE_FLOAT_TYPE)
		return B_BAD_TYPE;
	return Insert(transaction, (uint8_t *)&key, sizeof(key), value);
}
//---------------------------------------------------------------------------

inline status_t BPlusTree::Insert(Transaction &transaction, double key, int64_t value)
{
	if (fHeader->data_type != BPLUSTREE_DOUBLE_TYPE)
		return B_BAD_TYPE;
	return Insert(transaction, (uint8_t *)&key, sizeof(key), value);
}
//---------------------------------------------------------------------------
//	#pragma mark - TreeIterator inline functions
inline status_t TreeIterator::Rewind()
{
	return Goto(BPLUSTREE_BEGIN);
}
//---------------------------------------------------------------------------

inline status_t TreeIterator::GetNextEntry(void *key, uint16_t *keyLength, uint16_t maxLength,
    int64_t *value, uint16_t *duplicate)
{
	// key = dirent->d_name, keyLength = &lenght (0), maxLength = sizeof(dirent), value = &vnode_id (0)
	return Traverse(BPLUSTREE_FORWARD, key, keyLength, maxLength, value, duplicate);
}
//---------------------------------------------------------------------------

inline status_t TreeIterator::GetPreviousEntry(void *key, uint16_t *keyLength, uint16_t maxLength,
    int64_t *value, uint16_t *duplicate)
{
	return Traverse(BPLUSTREE_BACKWARD, key, keyLength, maxLength, value, duplicate);
}
//---------------------------------------------------------------------------
//	#pragma mark - bplustree_header inline functions
inline bool bplustree_header::CheckNode(bplustree_node *node) const
{
	// sanity checks (links, all_key_count)
	return IsValidLink(node->LeftLink())
		&& IsValidLink(node->RightLink())
		&& IsValidLink(node->OverflowLink())
        && (int8_t *)node->Values() + node->NumKeys() * sizeof(int64_t) <= (int8_t *)node + NodeSize();
}
//---------------------------------------------------------------------------

inline bool bplustree_header::IsValidLink(int64_t link) const
{
    return (link==(int64_t)BPLUSTREE_NULL) || ((link!=0) && (link <= (int64_t)MaximumSize() - (int64_t)NodeSize()));
}
//	#pragma mark - bplustree_node inline functions

inline uint16_t *bplustree_node::KeyLengths() const
{
	return (uint16_t *)(((char *)this) + round_up(sizeof(bplustree_node)
		+ AllKeyLength()));
}
//---------------------------------------------------------------------------

inline int64_t *bplustree_node::Values() const
{
    return (int64_t *)((char *)KeyLengths() + NumKeys() * sizeof(uint16_t));
}
//---------------------------------------------------------------------------

inline uint8_t *bplustree_node::Keys() const
{
	return (uint8_t *)this + sizeof(bplustree_node);
}
//---------------------------------------------------------------------------

inline int32_t bplustree_node::Used() const
{
	return round_up(sizeof(bplustree_node) + AllKeyLength()) + NumKeys()
        * (sizeof(uint16_t) + sizeof(int64_t));
}
//---------------------------------------------------------------------------

inline bool bplustree_node::IsLeaf() const
{
    return OverflowLink() == (int64_t)BPLUSTREE_NULL;
}
//---------------------------------------------------------------------------

inline duplicate_array *bplustree_node::FragmentAt(int8_t index) const
{
    return (duplicate_array *)((int64_t *)this
		+ index * (NUM_FRAGMENT_VALUES + 1));
}
//---------------------------------------------------------------------------

inline duplicate_array *bplustree_node::DuplicateArray() const
{
	return (duplicate_array *)&this->overflow_link;
}

inline uint8_t bplustree_node::LinkType(int64_t link)
{
    return *(int64_t *)&link >> 62;
}
//---------------------------------------------------------------------------

inline int64_t bplustree_node::MakeLink(uint8_t type, int64_t link, uint32_t fragmentIndex)
{
	return ((int64_t)type << 62) | (link & 0x3ffffffffffffc00)
		| (fragmentIndex & 0x3ff);
}
//---------------------------------------------------------------------------

inline bool bplustree_node::IsDuplicate(int64_t link)
{
	return (LinkType(link)
		& (BPLUSTREE_DUPLICATE_NODE | BPLUSTREE_DUPLICATE_FRAGMENT)) > 0;
}
//---------------------------------------------------------------------------

inline int64_t bplustree_node::FragmentOffset(int64_t link)
{
	return link & 0x3ffffffffffffc00;
}
//---------------------------------------------------------------------------

inline uint32_t bplustree_node::FragmentIndex(int64_t link)
{
	return (uint32_t)(link & 0x3ff);
}
//---------------------------------------------------------------------------

inline uint32_t bplustree_node::MaxFragments(uint32_t nodeSize)
{
    return nodeSize / ((NUM_FRAGMENT_VALUES + 1) * sizeof(int64_t));
}
#endif	// B_PLUS_TREE_H
