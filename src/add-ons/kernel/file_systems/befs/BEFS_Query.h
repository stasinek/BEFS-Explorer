/* Query - query parsing and evaluation
 *
 * Copyright 2001-2004, Axel Dörfler, axeld@pinc-software.de.
 * This file may be used under the terms of the MIT License.
 */
//---------------------------------------------------------------------------
#ifndef BEFS_QUERY_H
#define BEFS_QUERY_H
//---------------------------------------------------------------------------
#include "BEFS_Chain.h"
#include "BEFS_Index.h"
#include "BEFS_Stack.h"
#include "BEFS.h"
//---------------------------------------------------------------------------
class Volume;
class Term;
class Equation;
class TreeIterator;
class Query;
//---------------------------------------------------------------------------
class Expression {
    public:
        Expression(char *expr);
        ~Expression();

        status_t InitCheck();
        const char *Position() const { return fPosition; }
        Term *Root() const { return fTerm; }

    protected:
        Term *ParseOr(char **expr);
        Term *ParseAnd(char **expr);
        Term *ParseEquation(char **expr);

        bool IsOperator(char **expr,char op);

    private:
        Expression(const Expression &);
        Expression &operator=(const Expression &);
            // no implementation

        char *fPosition;
        Term *fTerm;
};
//---------------------------------------------------------------------------

class Query {
    public:
        Query(Volume *volume, Expression *expression, uint32_t flags);
        ~Query();

        status_t Rewind();
        status_t GetNextEntry(struct dirent_t *, size_t size);

        void SetLiveMode(port_id port, int32_t token);
        void LiveUpdate(Inode *inode, const char *attribute, int32_t type,
            const uint8_t *oldKey, size_t oldLength, const uint8_t *newKey, size_t newLength);

        Expression *GetExpression() const { return fExpression; }

    private:
        Volume			*fVolume;
        Expression		*fExpression;
        Equation		*fCurrent;
        TreeIterator	*fIterator;
        Index			fIndex;
        Stack<Equation *> fStack;

        uint32_t			fFlags;
        port_id			fPort;
        int32_t			fToken;

    private:
        friend class Chain<Query>;

        Query			*fNext;
};
//---------------------------------------------------------------------------
#endif	/* QUERY_H */
