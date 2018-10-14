#include <sys/my_inode.h>

struct kmem_cache * myramfs_inode_cachep = NULL;

struct myramfs_inode_info {
	struct inode vfs_inode;
};

static void init_once(void *obj)
{
        struct myramfs_inode_info *i = obj;

        inode_init_once(&i->vfs_inode);
}


static struct inode *myramfs_alloc_inode(struct super_block *sb)
{
        struct myramfs_inode_info *i;

        i = kmem_cache_alloc(myramfs_inode_cachep, GFP_KERNEL);
        if (!i)
                return NULL;
	
	printk(KERN_INFO "into func %s\n",__func__);

        i->vfs_inode.i_version = 1;
        return &i->vfs_inode;
}
static void myramfs_destroy_inode(struct inode *inode)
{
	printk(KERN_INFO "into func %s\n",__func__);
        kmem_cache_free(myramfs_inode_cachep, container_of(inode, struct myramfs_inode_info, vfs_inode));
}

static struct super_operations myramfs_super_op = {
	.alloc_inode = myramfs_alloc_inode,
	.destroy_inode = myramfs_destroy_inode,
	.evict_inode = myramfs_evict_inode,
};
static struct inode_operations myramfs_inode_operations = {
	.create = myramfs_create_node,
	.lookup = my_ramfs_lookup,
	.setattr = my_ramfs_setattr,
	.getattr = my_ramfs_getattr,
	.mkdir = my_ramfs_mkdir,
	.rmdir = simple_rmdir
};

static struct file_operations myramfs_dir_file_op = {
	.open = dcache_dir_open,
	.iterate = dcache_readdir,
	.read = generic_read_dir,
	.release = dcache_dir_close
};

static struct file_operations myramfs_file_file_op = {
	.open = dcache_dir_open
};

struct inode* get_my_ramfs_inode(struct super_block *sb, const struct inode* dir, umode_t mode, dev_t dev) {
        struct inode* inode = new_inode(sb);
        if (inode) {
                inode->i_ino = get_next_ino();
                inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
                inode_init_owner(inode, dir, mode);
                inode->i_gid = current_fsgid();
                inode->i_mode = mode;
                switch (mode & S_IFMT) {
			case S_IFDIR:
                        	inode->i_op = &myramfs_inode_operations;
                        	inode->i_fop = &myramfs_dir_file_op;
                        	inc_nlink(inode);
				break;
			case S_IFREG:
				inode->i_op = &myramfs_inode_operations;
				inode->i_fop = &myramfs_file_file_op;
				inc_nlink(inode);
				break;
			default:
				printk(KERN_INFO "invalid file type\n");
                }
        }
        return inode;
}

int my_ramfs_fill_super(struct super_block *sb, void *data, int silent) {
	struct inode *inode;

	sb->s_magic = MY_RAMFS_MAGIC;
	sb->s_maxbytes = MAX_LFS_FILESIZE;
        sb->s_blocksize = PAGE_CACHE_SIZE;
        sb->s_blocksize_bits = PAGE_CACHE_SHIFT;
	sb->s_op = &myramfs_super_op;
	
	inode = get_my_ramfs_inode(sb, NULL, MY_RAMFS_DEFAULT_PERM | S_IFDIR, 0);
	
	sb->s_root = d_make_root(inode);
	return 0;
}
struct dentry *my_ramfs_mount(struct file_system_type *fs_type,
        int flags, const char *dev_name, void *data) {
	return mount_nodev(fs_type, flags, data, my_ramfs_fill_super);
}
static void my_ramfs_kill(struct super_block *sb) {
	kill_litter_super(sb);
	printk("unmounting \n");
}
static struct file_system_type my_ramfs_type = {
        .name = "my_ramfs",
        .mount = my_ramfs_mount,
        .kill_sb = my_ramfs_kill,
        .fs_flags = FS_USERNS_MOUNT,
};

static int __init my_ramfs_init(void) {
	myramfs_inode_cachep = kmem_cache_create("myramfs_inode_chache", sizeof(struct myramfs_inode_info), 0,
		SLAB_RECLAIM_ACCOUNT,init_once);
	if(myramfs_inode_cachep == NULL) {
		printk(KERN_ALERT "myramfs chache allocation failed\n");
		return -1;
	}
	printk(KERN_INFO "my_ramfs init\n");
	register_filesystem(&my_ramfs_type);
	return 0;
}

static void __exit my_ramfs_exit(void) {
	kmem_cache_destroy(myramfs_inode_cachep);
	unregister_filesystem(&my_ramfs_type);
	printk(KERN_INFO "my_ramfs exit\n");
}

module_init(my_ramfs_init);
module_exit(my_ramfs_exit);
