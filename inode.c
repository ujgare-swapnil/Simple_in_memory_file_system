#include <sys/my_inode.h>

int myramfs_create_node(struct inode *dir,struct dentry *dentry, umode_t mode, bool excl)
{
	return my_ramfs_mknod (dir, dentry, mode | S_IFREG, 0);
}

void myramfs_evict_inode (struct inode * inode) {
	clear_inode(inode);
	iput(inode);
}

struct dentry *my_ramfs_lookup (struct inode *inode, struct dentry *dentry, unsigned int flag) {
        return simple_lookup(inode, dentry, flag);
}

int my_ramfs_setattr(struct dentry *dentry, struct iattr *attr) {
        int err = 0;
        printk(KERN_ALERT "IN SETATTR\n");
        err = simple_setattr(dentry, attr);
        return err;
}

int my_ramfs_getattr(struct vfsmount *mnt, struct dentry *dentry, struct kstat *stat) {
        int err = 0;
        err = simple_getattr(mnt, dentry, stat);
        return err;
}


int my_ramfs_mknod(struct inode *dir, struct dentry *dentry, umode_t mode, dev_t dev)
{
        struct inode * inode = get_my_ramfs_inode(dir->i_sb, dir, mode, dev);
        int error = -ENOSPC;

        if (inode) {
                d_instantiate(dentry, inode);
                dget(dentry);   /* Extra count - pin the dentry in core */
                error = 0;
                dir->i_mtime = dir->i_ctime = CURRENT_TIME;
        }
        return error;
}

int my_ramfs_mkdir(struct inode * dir, struct dentry * dentry, umode_t mode)
{
        int retval = my_ramfs_mknod(dir, dentry, mode | S_IFDIR, 0);
        if (!retval)
                inc_nlink(dir);
        return retval;
}

