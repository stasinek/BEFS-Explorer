/* Author: Peter Speybrouck - peter.speybrouck@gmail.com
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <phk@FreeBSD.ORG> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Poul-Henning Kamp
 * ----------------------------------------------------------------------------
 */
//---------------------------------------------------------------------------
#ifndef BEFS_INTERFACE_H
#define BEFS_INTERFACE_H
//-----------------------------------------------------------------------------
#include "BEFS.h"
#include "BEFS_Attribute.h" //for struct attr_cookie
//-----------------------------------------------------------------------------
status_t bfs_open_dir(void *, void *, void **);
status_t bfs_read_dir(void *_ns, void *_node, void *_cookie, struct dirent_t *Dirent,
    size_t bufferSize, uint32_t *_num);
status_t bfs_rewind_dir(void * ns, void * node, void *_cookie);
status_t bfs_close_dir(void * /*ns*/, void * /*node*/, void * /*_cookie*/);
//-----------------------------------------------------------------------------
status_t bfs_lookup(void *_ns, void *_directory, const char *file, vnode_id *_vnodeID, int *_type);
status_t bfs_open(void *_fs, void *_node, int openMode, void **_cookie);
status_t bfs_read(void *_ns, void *_node, void *_cookie, int64_t pos, void *buffer, int64_t *_length);
status_t bfs_get_vnode_name(void* _fs, fs_vnode _node, char *buffer, size_t bufferSize);
status_t bfs_read_vnode(void *_ns, vnode_id id, void **_node, bool reenter);
status_t bfs_close(void *_ns, void *_node, void *_cookie);
//-----------------------------------------------------------------------------
status_t bfs_open_attr_dir(void *_ns, void *_node, void **_cookie);
status_t bfs_close_attr_dir(void *ns, void *node, void *cookie);
status_t bfs_rewind_attr_dir(void *_ns, void *_node, void *_cookie);
status_t bfs_read_attr_dir(void *_ns, void *node, void *_cookie, struct dirent_t *dirent, size_t bufferSize, uint32_t *_num);
//-----------------------------------------------------------------------------
status_t bfs_open_attr(void* _fs, fs_vnode _node, const char *name, int openMode, void *_cookie);
status_t bfs_close_attr(void* _fs, fs_vnode _file, attr_cookie* cookie);
status_t bfs_read_attr(void* _fs, fs_vnode _file, attr_cookie* _cookie, int64_t pos, void *buffer, int64_t *_length);
status_t bfs_read_attr_stat(void* _fs, fs_vnode _file, attr_cookie* _cookie, struct attr_stat *stat);
//-----------------------------------------------------------------------------
#endif
