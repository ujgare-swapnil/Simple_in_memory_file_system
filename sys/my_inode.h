#ifndef __MY_INODE_H__
#define __MY_INODE_H__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <sys/my_inode.h>
#include <linux/pagemap.h>
#include <linux/slab.h>
#include <linux/time.h>

#define sb_max_size 20
#define MY_RAMFS_MAGIC 0x508711
#define MY_RAMFS_DEFAULT_PERM 0755

extern struct inode* get_my_ramfs_inode(struct super_block *sb, const struct inode* dir, umode_t mode, dev_t dev);
extern struct dentry *my_ramfs_lookup (struct inode *inode, struct dentry *dentry, unsigned int flag);
extern int my_ramfs_setattr(struct dentry *dentry, struct iattr *attr);
extern int my_ramfs_getattr(struct vfsmount *mnt, struct dentry *dentry, struct kstat *stat);
extern int my_ramfs_mknod(struct inode *dir, struct dentry *dentry, umode_t mode, dev_t dev);
extern int my_ramfs_mkdir(struct inode * dir, struct dentry * dentry, umode_t mode);
extern int myramfs_create_node(struct inode *dir,struct dentry *dentry, umode_t mode, bool excl);
extern void myramfs_evict_inode (struct inode * inode);
extern int myramfs_unlink (struct inode *ino,struct dentry *dent);
extern int myramfs_link (struct dentry *dent,struct inode *ino,struct dentry *dentry);
#endif /* __MY_INODE_H__ */
