/* Attribute - connection between pure inode and kernel_interface attributes
 *
 * Copyright 2004, Axel Dörfler, axeld@pinc-software.de.
 * This file may be used under the terms of the MIT License.
 */
#ifndef BEFS_ATTRIBUTE_H
#define BEFS_ATTRIBUTE_H

#include "BEFS_Inode.h"

struct attr_cookie {
	char	name[B_ATTR_NAME_LENGTH];
	uint32_t	type;
	int		open_mode;
	bool	create;
};


class Attribute {
	public:
		Attribute(Inode *inode);
		Attribute(Inode *inode, attr_cookie *cookie);
		~Attribute();

		status_t InitCheck();
		status_t CheckAccess(const char *name, int openMode);

		status_t Get(const char *name);
		void Put();

		status_t Create(const char *name, type_code type, int openMode, attr_cookie **_cookie);
		status_t Open(const char *name, int openMode, attr_cookie **_cookie);

		status_t Stat(struct attr_stat &stat);

        status_t Read(attr_cookie *cookie, int64_t pos, uint8_t *buffer, int64_t *_length);
		status_t Write(Transaction &transaction, attr_cookie *cookie,
                    int64_t pos, const uint8_t *buffer, size_t *_length);

	private:
		NodeGetter	fNodeGetter;
		Inode		*fInode;
		small_data	*fSmall;
		Inode		*fAttribute;
		const char	*fName;
};

#endif	/* ATTRIBUTE_H */

