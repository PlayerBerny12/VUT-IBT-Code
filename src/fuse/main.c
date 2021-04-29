#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

static void get_full_path(const char *path, char *full_path) {	
	// Base path on hosts filesystem
	char base_path[31] = "/home/user/.local/lib/vdu/fuse";
	
	// Concatenate paths
	strcpy(full_path, base_path);
	strcat(full_path, path);
}

static void *fuse_init(struct fuse_conn_info *conn, struct fuse_config *cfg)
{
	(void)conn;

	// Init setup
	cfg->use_ino = 1;
	cfg->entry_timeout = 0;
	cfg->attr_timeout = 0;
	cfg->negative_timeout = 0;

	return NULL;
}

static int fuse_getattr(const char *path, struct stat *stbuf,
						struct fuse_file_info *fi)
{
	(void)fi;
	
	// Full path
	char full_path[255+13+1];	
	get_full_path(path, full_path);

	// Get information about file
	int res = lstat(full_path, stbuf);	
	if (res == -1)
	{
		return -errno;
	}

	return 0;
}

static int fuse_access(const char *path, int mask)
{	
	// Full path
	char full_path[255+13+1];
	get_full_path(path, full_path);

	// Check access
	int res = access(full_path, mask);
	if (res == -1)
	{
		return -errno;
	}

	return 0;
}

static int fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
						off_t offset, struct fuse_file_info *fi,
						enum fuse_readdir_flags flags)
{
	(void)offset;
	(void)fi;
	(void)flags;

	// Full path
	char full_path[255+13+1];
	get_full_path(path, full_path);

	// Open directory
	DIR *dp = opendir(full_path);
	if (dp == NULL)
	{
		return -errno;
	}

	// Read entries from dir
	struct dirent *de;
	while ((de = readdir(dp)) != NULL)
	{
		struct stat st = {0};
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		
		// Fill dictionary info
		if (filler(buf, de->d_name, &st, 0, 0))
		{
			break;
		}
	}

	closedir(dp);
	return 0;
}

static int fuse_unlink(const char *path)
{	
	// Full path
	char full_path[255+13+1];
	get_full_path(path, full_path);

	// Remove name and its references
	int res = unlink(full_path);
	if (res == -1)
	{
		return -errno;
	}

	return 0;
}

static int fuse_utimens(const char *path, const struct timespec ts[2],
						struct fuse_file_info *fi)
{
	(void)fi;	

	// Full path
	char full_path[255+13+1];
	get_full_path(path, full_path);
	
	// Set file timestamps
	int res = utimensat(0, full_path, ts, AT_SYMLINK_NOFOLLOW);
	if (res == -1)
	{
		return -errno;
	}

	return 0;
}

static int fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	// Full path
	char full_path[255+13+1];
	get_full_path(path, full_path);

	// Open (create) file
	int res = open(full_path, fi->flags, mode);
	if (res == -1) 
	{
		return -errno;
	}

	fi->fh = res;
	return 0;
}

static int fuse_open(const char *path, struct fuse_file_info *fi)
{	 
	// Full path
	char full_path[255+13+1];
	get_full_path(path, full_path);

	// Open file
	int res = open(full_path, fi->flags);
	if (res == -1)
	{
		return -errno;
	}

	fi->fh = res;
	return 0;
}

static int fuse_read(const char *path, char *buf, size_t size, off_t offset,
					 struct fuse_file_info *fi)
{
	// Full path
	char full_path[255+13+1];
	get_full_path(path, full_path);
	
	// Open file if it is already not
	int fd;
	if (fi == NULL) 
	{
		fd = open(full_path, O_RDONLY);
	}
	else
	{
		fd = fi->fh;
	}
	
	if (fd == -1)
	{
		return -errno;
	}

	// Read file
	int res = pread(fd, buf, size, offset);
	if (res == -1)
	{
		res = -errno;
	}

	// Close file if it was opened by this function
	if (fi == NULL) 
	{
		close(fd);
	}

	return res;
}

static int fuse_write(const char *path, const char *buf, size_t size, off_t offset,
					  struct fuse_file_info *fi)
{
	(void)fi;	

	// Full path
	char full_path[255+13+1];
	get_full_path(path, full_path);

	// Open file if it is already not
	int fd;
	if (fi == NULL) 
	{
		fd = open(full_path, O_WRONLY);
	}
	else
	{
		fd = fi->fh;
	}

	if (fd == -1)
	{
		return -errno;
	}

	// Write to file
	int res = pwrite(fd, buf, size, offset);
	if (res == -1)
	{
		res = -errno;
	}
	
	// Close file if it was opened by this function
	if (fi == NULL)
	{
		close(fd);
	}

	return res;
}

static int fuse_release(const char *path, struct fuse_file_info *fi)
{
	(void)path;

	// Close opned file
	close(fi->fh);
	return 0;
}

static const struct fuse_operations fuse_opers = {
	.init = fuse_init,
	.getattr = fuse_getattr,
	.access = fuse_access,
	.readdir = fuse_readdir,
	.unlink = fuse_unlink,
	.utimens = fuse_utimens,
	.open = fuse_open,
	.create = fuse_create,
	.read = fuse_read,
	.write = fuse_write,
	.release = fuse_release};

int main(int argc, char *argv[])
{
	// Set default permission of files on hosts filesystem
	umask(0007);

	// Start daemon
	return fuse_main(argc, argv, &fuse_opers, NULL);
}
