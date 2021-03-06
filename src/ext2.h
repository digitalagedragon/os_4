#ifndef EXT2_H
#define EXT2_H

#define EXT2_MAGIC 0xef53

struct ext2_superblock {
  unsigned int n_inodes;        // s_inodes_count
  unsigned int n_blocks;        // s_blocks_count
  unsigned int n_resb;          // s_r_blocks_count
  unsigned int n_ua_blocks;     // s_free_blocks_count
  unsigned int n_ua_inodes;     // s_free_inodes_count
  unsigned int sb_off;          // s_first_data_block
  unsigned int block_size;      // s_log_block_size
  unsigned int fragment_size;   // s_log_fragment_size
  unsigned int bg_size;         // s_blocks_per_group
  unsigned int fg_size;         // s_fragments_per_group
  unsigned int ig_size;         // s_inodes_per_group
  unsigned int rtime;           // 44
  unsigned int wtime;           // 48
  unsigned short nm_since_cons; // 52
  unsigned short nm_remain;     // 54
  unsigned short magic;         // 56
  unsigned short state;         // 58
  unsigned short err;           // 60
  unsigned short vers_minor;    // 62
  unsigned int ctime;           // 64
  unsigned int check_interval;  // 68
  unsigned int osid;            // 72
  unsigned int vers_major;      // 76
  unsigned short res_user;      // 80
  unsigned short res_group;     // 82
  // if vers_major > 0
  unsigned int first_nr_inode;
  unsigned short inode_size;
  unsigned int optf_pres;
  unsigned int reqf_pres;
  unsigned int errf_pres;
  unsigned char fsid[16];
  unsigned char vname[16];
  unsigned char mpath[64];
  unsigned int comp;
  unsigned char n_file_prealloc;
  unsigned char n_dir_prealloc;
  unsigned short res0;
  unsigned char jid[16];
  unsigned int jinode;
  unsigned int jdev;
  unsigned int orphan_list;
} __attribute__ ((packed));

struct ext2_bg_desc {
  unsigned int block_bitmap;
  unsigned int inode_bitmap;
  unsigned int inode_table;
  unsigned short n_ua_blocks;
  unsigned short n_ua_inodes;
  unsigned short n_dirents;
  char pad[14];
} __attribute__ ((packed));

struct ext2_block_bitmaps {
  unsigned char *blocks;
  unsigned char *inodes;
} __attribute__ ((packed));

struct ext2_inode {
  unsigned short type;
  unsigned short uid;
  unsigned int size_low;
  unsigned int atime;
  unsigned int ctime;
  unsigned int mtime;
  unsigned int dtime;
  unsigned short gid;
  unsigned short refcnt;
  unsigned int n_sectors;
  unsigned int flags;
  unsigned int oss1;
  unsigned int bp[12];
  unsigned int bpp;
  unsigned int bppp;
  unsigned int bpppp;
  unsigned int gen;
  unsigned int res0;
  unsigned int res1;
  unsigned int f_addr;
  unsigned char oss2[12];
} __attribute__ ((packed));

struct ext2_dirent {
  unsigned int inode;
  unsigned short entsize;
  unsigned char nlen;
  unsigned char type;
  char *name;
} __attribute__ ((packed));

struct ext2_dirstate {
  unsigned int ent_idx, n_dirents;
  struct ext2_inode *inode;
  struct ext2_dirent *last;
};

typedef struct ext2_dirstate * ext2_dirstate_t;
typedef struct ext2_dirent * ext2_dirent_t;

void read_superblock(struct ext2_superblock *s);
void read_block_group(struct ext2_superblock *s, struct ext2_bg_desc *desc, int idx);
void read_inode(struct ext2_superblock *s, struct ext2_inode *inode, int idx);
void get_block(struct ext2_inode *inode, int block_idx, unsigned char *block);

ext2_dirstate_t opendir(struct ext2_inode *inode);
ext2_dirent_t dirwalk(ext2_dirstate_t s);
void closedir(ext2_dirstate_t s);

void parse_inode_type(unsigned short type, char *out);

int get_file_inode(struct ext2_superblock *s, int dir_inode, const char *name);
int get_path_inode(struct ext2_superblock *s, const char *path);

void list_directory(struct ext2_superblock *s, struct ext2_inode *d);
#endif
