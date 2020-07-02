#ifndef __MKFTL_H__
#define __MKFTL_H__

#define DEVELOP

#define MT_MAGIC_NUMBER			0x3105
#define NAND_DEFAULT_VALUE		0xFFFFFFFF

#define SPARSE_HEADER_MAGIC	0xed26ff3a
#define SPARSE_HEADER_MAJOR_VER 1

#define NAND_VOLUME			(8 * 1024 * 1024) /* Unit: kbytes */
#define NAND_PAGE_SIZE			(16 * 1024)
#define NAND_PAGE_NUM_PER_BLOCK		256
#define NAND_SECTOR_PER_PAGE		16
/* NAND_OOB_SIZE is not accurate, OOB size can be found from ubi_device->mtd->oobsize */
#define NAND_OOB_SIZE			(NAND_SECTOR_PER_PAGE * 8)
#define NAND_BLOCK_SIZE			(NAND_PAGE_SIZE * NAND_PAGE_NUM_PER_BLOCK)
#define NAND_TOTAL_PAGE_NUM		(NAND_VOLUME / (NAND_PAGE_SIZE >> 10))
#define NAND_TOTAL_BLOCK_NUM		(NAND_VOLUME / (NAND_BLOCK_SIZE >> 10))

#define FS_PAGE_SIZE			(4 * 1024)
#define FS_TOTAL_PAGE_NUM		(NAND_VOLUME / (FS_PAGE_SIZE >> 10))

/* Page Mapping per nand page (must be power of 2)*/
#define PM_PER_NANDPAGE			(NAND_PAGE_SIZE >> 2)
/*#define MAX_OFFSET_PER_BLOCK		(NAND_PAGE_SIZE * (NAND_PAGE_NUM_PER_BLOCK - 3))*/

#define PMT_ADDRESSES_PER_PAGE		4
#define PMT_INDICATOR_PAGE_SHIFT	10
#define PMT_INDICATOR_DIRTY_SHIFT	1
#define PMT_INDICATOR_CACHED_SHIFT	3
#define PMT_LEB_PAGE_INDICATOR_PAGE_SHIFT	12
#define PMT_PAGE_SHIFT			12
#define PMT_PART_SHIFT			12
#define PMT_DATACACHE_BUFF_NUM_SHIFT	4
#define PMT_TOTAL_CLUSTER_NUM		(FS_TOTAL_PAGE_NUM / PM_PER_NANDPAGE)
#define PMT_CACHE_NUM			4 /* Max: 7 */
#define PMT_BLOCK_NUM			3

#define CONFIG_START_BLOCK		0
#define REPLAY_BLOCK			2
#define PMT_START_BLOCK			3
#define DATA_START_BLOCK		PMT_START_BLOCK + PMT_BLOCK_NUM
#define MTKFTL_MAX_DATA_NUM_PER_PAGE	(1 << PMT_PART_SHIFT)

/**** MACROS ****/
#define PMT_LEB_PAGE_INDICATOR_SET_BLOCKPAGE(p, blk, page) \
		((p) = (((blk) << PMT_LEB_PAGE_INDICATOR_PAGE_SHIFT) | (page)))

#define PMT_LEB_PAGE_INDICATOR_GET_BLOCK(p)	((p) >> PMT_LEB_PAGE_INDICATOR_PAGE_SHIFT)
#define PMT_LEB_PAGE_INDICATOR_GET_PAGE(p)	((p) & ((1 << PMT_LEB_PAGE_INDICATOR_PAGE_SHIFT) - 1))



#define PMT_INDICATOR_SET_BLOCKPAGE(p, blk, page, dirty, cache_num) \
		((p) = (((blk) << \
		(PMT_INDICATOR_PAGE_SHIFT + PMT_INDICATOR_DIRTY_SHIFT + PMT_INDICATOR_CACHED_SHIFT)) \
		| ((page) << (PMT_INDICATOR_DIRTY_SHIFT + PMT_INDICATOR_CACHED_SHIFT)) | \
		((dirty) << PMT_INDICATOR_CACHED_SHIFT) | ((cache_num) + 1)))

#define PMT_INDICATOR_GET_BLOCK(p)	((p) >> (PMT_INDICATOR_PAGE_SHIFT + PMT_INDICATOR_DIRTY_SHIFT \
					+ PMT_INDICATOR_CACHED_SHIFT))

#define PMT_INDICATOR_GET_PAGE(p)	(((p) >> (PMT_INDICATOR_DIRTY_SHIFT + PMT_INDICATOR_CACHED_SHIFT)) \
					& ((1 << PMT_INDICATOR_PAGE_SHIFT) - 1))
					
#define PMT_INDICATOR_IS_INCACHE(p)			((p) & ((1 << PMT_INDICATOR_CACHED_SHIFT) - 1))
#define PMT_INDICATOR_CACHE_BUF_NUM(p)			(((p) & ((1 << PMT_INDICATOR_CACHED_SHIFT) - 1)) - 1)
#define PMT_INDICATOR_IS_DIRTY(p)			(((p) & (1 << PMT_INDICATOR_CACHED_SHIFT)) >> PMT_INDICATOR_CACHED_SHIFT)
#define PMT_INDICATOR_SET_CACHE_BUF_NUM(p, num)		((p) = (((p) & (~((1 << PMT_INDICATOR_CACHED_SHIFT) - 1))) \
							| ((num) + 1)))
#define PMT_INDICATOR_SET_DIRTY(p)	((p) |= (1 << PMT_INDICATOR_CACHED_SHIFT))
#define PMT_INDICATOR_RESET_INCACHE(p)	((p) &= (~((1 << PMT_INDICATOR_CACHED_SHIFT) - 1)))
#define PMT_INDICATOR_RESET_DIRTY(p)	((p) &= (~(1 << PMT_INDICATOR_CACHED_SHIFT)))



#define PMT_SET_BLOCKPAGE(p, blk, page) \
		((p) = (((blk) << PMT_PAGE_SHIFT) | (page)))
			
#define META_PMT_SET_DATA(p, data_size, part, cache_num) \
		((p) = (((data_size) << (PMT_PART_SHIFT + PMT_DATACACHE_BUFF_NUM_SHIFT)) \
			| ((part) << PMT_DATACACHE_BUFF_NUM_SHIFT) \
			| ((cache_num) + 1)))

#define PMT_GET_BLOCK(p)	((p) >> PMT_PAGE_SHIFT)
#define PMT_GET_PAGE(p)		((p) & ((1 << PMT_PAGE_SHIFT) - 1))
#define PMT_GET_DATASIZE(p)	((p) >> (PMT_PAGE_SHIFT + PMT_DATACACHE_BUFF_NUM_SHIFT))
#define PMT_GET_PART(p)		(((p) >> PMT_DATACACHE_BUFF_NUM_SHIFT) & ((1 << PMT_PART_SHIFT) - 1))
#define PMT_GET_DATACACHENUM(p)	(((p) & ((1 << PMT_DATACACHE_BUFF_NUM_SHIFT) - 1)) - 1)
#define PMT_SET_DATACACHE_BUF_NUM(p, num)	((p) = (((p) & (~((1 << PMT_DATACACHE_BUFF_NUM_SHIFT) - 1))) \
						| ((num) + 1)))
#define PMT_IS_DATA_INCACHE(p)	((p) & ((1 << PMT_DATACACHE_BUFF_NUM_SHIFT) - 1))
#define PMT_RESET_DATA_INCACHE(p)	((p) &= (~((1 << PMT_DATACACHE_BUFF_NUM_SHIFT) - 1)))

#define BIT_UPDATE(p, size)			((p) += (size))

#define err_msg(fmt, ...) ({                                \
	fprintf(stderr, "Error: " fmt "\n", ##__VA_ARGS__); \
	-1;                                                 \
})

/**
 * struct mt_ftl_data_header - mt_ftl data header.
 * @sector: the sector number of data
 * @offset_len: the page offset of data and data length
 *
 * A header store data information.
 */
struct mt_ftl_data_header {
	unsigned long long sector;
	unsigned long long offset_len;	/* offset(2bytes):length(2bytes) */
};

/**
 * struct mt_ftl_param - mt_ftl parameters.
 * @u4NextReplayOffsetIndicator: Indicate next used replay page
 * @u4NextLebPageIndicator: Indicate next used leb/page. leb(20bits):Page(12bits)
 * @u4CurrentPMTLebPageIndicator: Indicate current used leb/page for PMT indicator. leb(20bits):Page(12bits)
 * @u4NextFreeLebIndicator: Indicate next free Leb or invalid number for no free leb
 * @u4NextFreePMTLebIndicator: Indicate next free PMT Leb or invalid number for no free leb
 * @u4GCReserveLeb: Reserved leb for Garbage Collection
 * @u4GCReservePMTLeb: Reserved PMT leb for Garbage Collection
 * @cc: Compressor handler
 * @u4PMTIndicator: Page Mapping Table Indicator, used to indicate PMT position in NAND
 * @u4PMTCache: Page Mapping Table in cache
 * @u4MetaPMTCache: Meta Page Mapping Table in cache
 * @i4CurrentPMTClusterInCache: Current cluster in PMT cache
 * @u4BIT: Block Invalid Table. Store invalid page amount of a block
 * @u1DataCache: Block Data Cache Buffer, collect data and store to NAND flash when it is full
 * @u4Header: Data information, including address (4bytes), page offset (2bytes) and data length (2bytes)
 * @u4DataNum: Total data number in a page
 *
 * The structure is used to store the parameters that ftl process needed.
 * And the information that will store to out image.
 */
struct mt_ftl_param {
	/* Indicate next used replay page. */
	unsigned int u4NextReplayOffsetIndicator;  /* 4 bytes */
	
	/* Indicate next used leb/page. leb(20bits):Page(12bits) */
	unsigned int u4NextLebPageIndicator;  /* 4 bytes */
	
	/* Indicate current used leb/page for PMT indicator. leb(20bits):Page(12bits) */
	/* Leb From 3 to PMT_BLOCK_NUM + 3 - 1 */
	unsigned int u4CurrentPMTLebPageIndicator;  /* 4 bytes */
	
	/* Indicate next free Leb or invalid number for no free leb.
	 * It is at most desc->vol->ubi->volumes[0]->reserved_pebs - 1 */	// TODO: volume number need to change
	unsigned int u4NextFreeLebIndicator;  /* 4 bytes */
	/* Leb From 3 to PMT_BLOCK_NUM + 3 - 1 */
	unsigned int u4NextFreePMTLebIndicator;  /* 4 bytes */
	
	/* Reserved leb for Garbage Collection */
	unsigned int u4GCReserveLeb; /* 4 bytes */
	/* Leb From 3 to PMT_BLOCK_NUM + 3 - 1 */
	unsigned int u4GCReservePMTLeb; /* 4 bytes */
	
	/* Compressor handler */
	struct crypto_comp *cc;
	
	/* Page Mapping Table Indicator, used to indicate PMT position in NAND.
	 * (Block_num(18bits):Page_num(10bits):Dirty(1bit):CachedBuffer(3bits))*/
	unsigned int *u4PMTIndicator;
	
	/* Page Mapping Table in cache. */
	/* Block_num(20bits):Page_num(12bits) */
	unsigned int *u4PMTCache;
	/* Data_size(16bits):Part_num(12bits):CachedBuffer(4bits) */
	unsigned int *u4MetaPMTCache;
	int *i4CurrentPMTClusterInCache;
	
	/* Block Invalid Table. Store invalid page amount of a block
	 * Unit of content: bytes */
	unsigned int *u4BIT;   /* 8K */
	
	/* Block Data Cache Buffer, collect data and store to NAND flash when it is full */
	unsigned char *u1DataCache;	/* 16K */
	
	/* Data information, including address (4bytes), page offset (2bytes) and data length (2bytes) */
	struct mt_ftl_data_header *u4Header;
	unsigned int u4DataNum;
	
	unsigned int *commit_page_buffer;
	unsigned char *cmpr_page_buffer;
};

typedef struct sparse_header {
  unsigned int	magic;		/* 0xed26ff3a */
  unsigned short int	major_version;	/* (0x1) - reject images with higher major versions */
  unsigned short int	minor_version;	/* (0x0) - allow images with higer minor versions */
  unsigned short int	file_hdr_sz;	/* 28 bytes for first revision of the file format */
  unsigned short int	chunk_hdr_sz;	/* 12 bytes for first revision of the file format */
  unsigned int	blk_sz;		/* block size in bytes, must be a multiple of 4 (4096) */
  unsigned int	total_blks;	/* total blocks in the non-sparse output image */
  unsigned int	total_chunks;	/* total chunks in the sparse input image */
  unsigned int	image_checksum; /* CRC32 checksum of the original data, counting "don't care" */
				/* as 0. Standard 802.3 polynomial, use a Public Domain */
				/* table implementation */
} sparse_header_t;

#define CHUNK_TYPE_RAW		0xCAC1
#define CHUNK_TYPE_FILL		0xCAC2
#define CHUNK_TYPE_DONT_CARE	0xCAC3
#define CHUNK_TYPE_CRC32    0xCAC4

typedef struct chunk_header {
  unsigned short int	chunk_type;	/* 0xCAC1 -> raw; 0xCAC2 -> fill; 0xCAC3 -> don't care */
  unsigned short int	reserved1;
  unsigned int	chunk_sz;	/* in blocks in output image */
  unsigned int	total_sz;	/* in bytes of chunk input file including chunk header and data */
} chunk_header_t;

/**
 * struct mkftl_param - mkftl parameters.
 * @chunk_buffer: a buffer store trunk data
 * @mt_ftl_buffer: a buffer store the data that will transfer to mt_ftl
 * @mt_ftl_buffer_index: index to the position that mt_ftl_buffer can start to fill
 *
 * The structure is used to store the parameters that make ftl needed.
 */
struct mkftl_param {
	unsigned int *chunk_buffer;
	
	unsigned long long address;
	
	int min_io_size;
	int leb_size;
	int max_leb_cnt;
	
	sparse_header_t sparse_header;
	chunk_header_t chunk_header;
	
	struct mt_ftl_param mtftl_param;
};

#endif	// #ifndef __MKFTL_H__