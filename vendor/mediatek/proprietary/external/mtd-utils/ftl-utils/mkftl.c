#define PROGRAM_NAME "mkftl"

#include "mkftl.h"
#include "common.h"
#include <getopt.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>

#include <stdio.h>
#include <stdlib.h>
#include <lzo/lzo1x.h>


struct mkftl_param info_;
static struct mkftl_param *c = &info_;

int verbose;

static char *root;
static int root_len;
static char *output;
static int out_fd;
static int in_fd;
static void *lzo_mem;

static const char *optstring = "d:r:m:o:D:h?vVe:c:g:f:Fp:k:x:X:j:R:l:j:UQqS:";

static const struct option longopts[] = {
	{"root",               1, NULL, 'r'},
	{"min-io-size",        1, NULL, 'm'},
	{"leb-size",           1, NULL, 'e'},
	{"max-leb-cnt",        1, NULL, 'c'},
	{"output",             1, NULL, 'o'},
	{"devtable",           1, NULL, 'D'},
	{"help",               0, NULL, 'h'},
	{"verbose",            0, NULL, 'v'},
	{"version",            0, NULL, 'V'},
	{"debug-level",        1, NULL, 'g'},
	{"jrn-size",           1, NULL, 'j'},
	{"reserved",           1, NULL, 'R'},
	{"compr",              1, NULL, 'x'},
	{"favor-percent",      1, NULL, 'X'},
	{"fanout",             1, NULL, 'f'},
	{"space-fixup",        0, NULL, 'F'},
	{"keyhash",            1, NULL, 'k'},
	{"log-lebs",           1, NULL, 'l'},
	{"orph-lebs",          1, NULL, 'p'},
	{"squash-uids",        0, NULL, 'U'},
	{"selinux",            0, NULL, 'S'},
	{NULL, 0, NULL, 0}
};

static const char *helptext =
"Usage: mkftl [OPTIONS] target\n"
"Make a MTK FTL compatible file system image from an existing file system image\n\n"
"Examples:\n"
"Build file system from system.img, writting the result in the system.mtftl.img file\n"
"\tmkftl -m 16384 -e 4161536 -c 100 -r /system.img system.mtftl.img\n"
"Options:\n"
"-r, -d, --root=DIR       build file system from directory DIR\n"
"-m, --min-io-size=SIZE   minimum I/O unit size\n"
"-e, --leb-size=SIZE      logical erase block size\n"
"-c, --max-leb-cnt=COUNT  maximum logical erase block count\n"
"-o, --output=FILE        output to FILE\n"
"-v, --verbose            verbose operation\n"
"-V, --version            display version information\n"
"-h, --help               display this help text\n\n"
"Note, SIZE is specified in bytes, but it may also be specified in Kilobytes,\n"
"Megabytes, and Gigabytes if a KiB, MiB, or GiB suffix is used.\n\n";

#ifndef O_BINARY
#define O_BINARY 0
#endif

/**
 * same_dir - determine if two file descriptors refer to the same directory.
 * @fd1: file descriptor 1
 * @fd2: file descriptor 2
 */
static int same_dir(int fd1, int fd2)
{
	struct stat stat1, stat2;

	if (fstat(fd1, &stat1) == -1)
		return -1;
	if (fstat(fd2, &stat2) == -1)
		return -1;
	return stat1.st_dev == stat2.st_dev && stat1.st_ino == stat2.st_ino;
}

/**
 * do_openat - open a file in a directory.
 * @fd: file descriptor of open directory
 * @path: path relative to directory
 * @flags: open flags
 *
 * This function is provided because the library function openat is sometimes
 * not available.
 */
static int do_openat(int fd, const char *path, int flags)
{
	int ret;
	char *cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
		return -1;
	ret = fchdir(fd);
	if (ret != -1)
		ret = open(path, flags);
	if (chdir(cwd) && !ret)
		ret = -1;
	free(cwd);
	return ret;
}

/**
 * in_path - determine if a file is beneath a directory.
 * @dir_name: directory path name
 * @file_name: file path name
 */
static int in_path(const char *dir_name, const char *file_name)
{
	char *fn = strdup(file_name);
	char *dn;
	int fd1, fd2, fd3, ret = -1, top_fd;

	if (!fn)
		return -1;
	top_fd = open("/", O_RDONLY);
	if (top_fd != -1) {
		dn = dirname(fn);
		fd1 = open(dir_name, O_RDONLY);
		if (fd1 != -1) {
			fd2 = open(dn, O_RDONLY);
			if (fd2 != -1) {
				while (1) {
					int same;

					same = same_dir(fd1, fd2);
					if (same) {
						ret = same;
						break;
					}
					if (same_dir(fd2, top_fd)) {
						ret = 0;
						break;
					}
					fd3 = do_openat(fd2, "..", O_RDONLY);
					if (fd3 == -1)
						break;
					close(fd2);
					fd2 = fd3;
				}
				close(fd2);
			}
			close(fd1);
		}
		close(top_fd);
	}
	free(fn);
	return ret;
}

static int validate_options(void)
{
	if (!output)
		return err_msg("no output file or UBI volume specified");
	if (root && in_path(root, output))
		return err_msg("output file cannot be in the UBIFS root "
			       "directory");
	if (!is_power_of_2(c->min_io_size))
		return err_msg("min. I/O unit size should be power of 2");
	if (c->leb_size < c->min_io_size)
		return err_msg("min. I/O unit cannot be larger than LEB size");
	if (c->leb_size % c->min_io_size)
		return err_msg("LEB should be multiple of min. I/O units");
	if (c->leb_size % 8)
		return err_msg("LEB size has to be multiple of 8");
	return 0;
}

/**
 * get_multiplier - convert size specifier to an integer multiplier.
 * @str: the size specifier string
 *
 * This function parses the @str size specifier, which may be one of
 * 'KiB', 'MiB', or 'GiB' into an integer multiplier. Returns positive
 * size multiplier in case of success and %-1 in case of failure.
 */
static int get_multiplier(const char *str)
{
	if (!str)
		return 1;

	/* Remove spaces before the specifier */
	while (*str == ' ' || *str == '\t')
		str += 1;

	if (!strcmp(str, "KiB"))
		return 1024;
	if (!strcmp(str, "MiB"))
		return 1024 * 1024;
	if (!strcmp(str, "GiB"))
		return 1024 * 1024 * 1024;

	return -1;
}

/**
 * get_bytes - convert a string containing amount of bytes into an
 *             integer.
 * @str: string to convert
 *
 * This function parses @str which may have one of 'KiB', 'MiB', or 'GiB' size
 * specifiers. Returns positive amount of bytes in case of success and %-1 in
 * case of failure.
 */
static long long get_bytes(const char *str)
{
	char *endp;
	long long bytes = strtoull(str, &endp, 0);

	if (endp == str || bytes < 0)
		return err_msg("incorrect amount of bytes: \"%s\"", str);

	if (*endp != '\0') {
		int mult = get_multiplier(endp);

		if (mult == -1)
			return err_msg("bad size specifier: \"%s\" - "
				       "should be 'KiB', 'MiB' or 'GiB'", endp);
		bytes *= mult;
	}

	return bytes;
}

static int get_options(int argc, char**argv)
{
	int opt, i;
	struct stat st;

	while (1) {
		opt = getopt_long(argc, argv, optstring, longopts, &i);
		if (opt == -1)
			break;
		switch (opt) {
		case 'r':
		case 'd':
			root_len = strlen(optarg);
			root = malloc(root_len + 2);
			if (!root)
				return err_msg("cannot allocate memory");

			memcpy(root, optarg, root_len);
			root[root_len] = 0;

			/* Make sure the root directory exists */
			if (stat(root, &st))
				return err_msg("bad root directory '%s'",
						   root);
			break;
		case 'm':
			c->min_io_size = get_bytes(optarg);
			if (c->min_io_size <= 0)
				return err_msg("bad min. I/O size");
			break;
		case 'e':
			c->leb_size = get_bytes(optarg);
			if (c->leb_size <= 0)
				return err_msg("bad LEB size");
			break;
		case 'c':
			c->max_leb_cnt = get_bytes(optarg);
			if (c->max_leb_cnt <= 0)
				return err_msg("bad maximum LEB count");
			break;
		case 'o':
			output = strdup(optarg);
			break;
		case 'h':
		case '?':
			printf("%s", helptext);
			exit(0);
		case 'v':
			verbose = 1;
			break;
		case 'V':
			common_print_version();
			exit(0);
		}
	}

	if (optind != argc && !output)
		output = strdup(argv[optind]);

	if (!output)
		return err_msg("no output device or file specified");

	if (c->min_io_size == -1)
		return err_msg("min. I/O unit was not specified "
			       "(use -h for help)");

	if (c->leb_size == -1)
		return err_msg("LEB size was not specified (use -h for help)");

	if (c->max_leb_cnt == -1)
		return err_msg("Maximum count of LEBs was not specified "
			       "(use -h for help)");

	if (c->min_io_size < 8)
		c->min_io_size = 8;

	if (verbose) {
		printf("mkftl\n");
		printf("\troot:         %s\n", root);
		printf("\tmin_io_size:  %d\n", c->min_io_size);
		printf("\tleb_size:     %d\n", c->leb_size);
		printf("\tmax_leb_cnt:  %d\n", c->max_leb_cnt);
		printf("\toutput:       %s\n", output);
	}

	if (validate_options())
		return -1;

	return 0;
}

/**
 * open_target - open the output target.
 *
 * Open the output target. The target can be an image file.
 *
 * Returns %0 in case of success and %-1 in case of failure.
 */
static int open_target(void)
{
	out_fd = open(output, O_CREAT | O_RDWR | O_TRUNC,
		      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if (out_fd == -1)
		return err_msg("cannot create output file '%s'", output);

	return 0;
}

/**
 * close_target - close the output target.
 *
 * Close the output target.
 *
 * Returns %0 in case of success and %-1 in case of failure.
 */
static int close_target(void)
{
	if (out_fd >= 0 && close(out_fd) == -1)
		return err_msg("cannot close the target '%s'", output);
	if (output)
		free(output);
	
	return 0;
}

static int read_all(int fd, void *buf, size_t len)
{
	size_t total = 0;
	int ret;
	char *ptr = buf;

	while (total < len) {
		ret = read(fd, ptr, len - total);

		if (ret <= 0)
			return err_msg("read fail, ret = %d", ret);

		ptr += ret;
		total += ret;
	}

	return 0;
}

static int write_all(int fd, void *buf, size_t len)
{
	size_t total = 0;
	int ret;
	char *ptr = buf;

	while (total < len) {
		ret = write(fd, ptr, len - total);

		if (ret <= 0)
			return err_msg("write fail, ret = %d", ret);

		ptr += ret;
		total += ret;
	}

	return 0;
}

static int get_sparse_header(void)
{
	int err = 0;
	
	err = read_all(in_fd, &c->sparse_header, sizeof(sparse_header_t));
	if (err < 0)
		return err_msg("Read sparse header fail");
	
	if (c->sparse_header.magic != SPARSE_HEADER_MAGIC)
		return err_msg("sparse header magic is wrong");

	if (c->sparse_header.major_version != SPARSE_HEADER_MAJOR_VER)
		return err_msg("sparse header major version is wrong");

	if (c->sparse_header.file_hdr_sz < sizeof(sparse_header_t))
		return err_msg("sparse header size is wrong");

	if (c->sparse_header.chunk_hdr_sz < sizeof(chunk_header_t))
		return err_msg("chunk header size is wrong");
		
	if (c->sparse_header.file_hdr_sz > sizeof(sparse_header_t)) {
		/* Skip the remaining bytes in a header that is longer than
		 * we expected.
		 */
		lseek(in_fd, c->sparse_header.file_hdr_sz - sizeof(sparse_header_t), SEEK_CUR);
	}
	
#ifdef DEVELOP
	printf("sparse header content\n");
	printf("sparse_header.magic = 0x%x\n", c->sparse_header.magic);
	printf("sparse_header.major_version = 0x%x\n", c->sparse_header.major_version);
	printf("sparse_header.minor_version = 0x%x\n", c->sparse_header.minor_version);
	printf("sparse_header.file_hdr_sz = %d\n", c->sparse_header.file_hdr_sz);
	printf("sparse_header.chunk_hdr_sz = %d\n", c->sparse_header.chunk_hdr_sz);
	printf("sparse_header.blk_sz = %d\n", c->sparse_header.blk_sz);
	printf("sparse_header.total_blks = %d\n", c->sparse_header.total_blks);
	printf("sparse_header.total_chunks = %d\n", c->sparse_header.total_chunks);
	printf("sparse_header.image_checksum = 0x%x\n", c->sparse_header.image_checksum);
#endif
	
	return err;
}

static int get_chunk_header(void)
{
	int err = 0;
	
	err = read_all(in_fd, &c->chunk_header, sizeof(chunk_header_t));
	if (err < 0)
		return err_msg("Read chunk header fail");
		
	if (c->sparse_header.chunk_hdr_sz > sizeof(chunk_header_t)) {
		/* Skip the remaining bytes in a header that is longer than
		 * we expected.
		 */
		lseek(in_fd, c->sparse_header.chunk_hdr_sz - sizeof(chunk_header_t), SEEK_CUR);
	}
	
#ifdef DEVELOP
	printf("chunk header content\n");
	printf("chunk_header.chunk_type = 0x%x\n", c->chunk_header.chunk_type);
	printf("chunk_header.chunk_sz = %d\n", c->chunk_header.chunk_sz);
	printf("chunk_header.total_sz = %d\n", c->chunk_header.total_sz);
#endif
	
	return err;
}

static int get_raw_data()
{
	int err = 0;
	
	err = read_all(in_fd, c->chunk_buffer, c->sparse_header.blk_sz);
	if (err < 0)
		return err_msg("Read raw data from input file fail");
		
	return err;
}

static int get_fill_data()
{
	int err = 0;
	unsigned int fill_val;
	unsigned int i = 0;
	
	err = read_all(in_fd, &fill_val, sizeof(fill_val));
	if (err < 0)
		return err_msg("read fill value from input file fail");
		
	for (i = 0; i < c->sparse_header.blk_sz / sizeof(unsigned int); i++) {
		c->chunk_buffer[i] = fill_val;
	}
	
	return err;
}

static int validate_header()
{
	unsigned int chunk_data_size;
	unsigned int fill_val;
	
	chunk_data_size = c->chunk_header.total_sz - c->sparse_header.chunk_hdr_sz;
	
	if (c->chunk_header.chunk_type == CHUNK_TYPE_RAW) {
		if (chunk_data_size % c->sparse_header.blk_sz != 0)
			return err_msg("chunk data size is not multiple of block size");
		if (chunk_data_size / c->sparse_header.blk_sz != c->chunk_header.chunk_sz)
			return err_msg("chunk block number is wrong");
	} else if (c->chunk_header.chunk_type == CHUNK_TYPE_FILL) {
		if (chunk_data_size != sizeof(fill_val))
			return err_msg("chunk data size is not 4 byte long");
	}
	
	return 0;
}

static int write_to_file(unsigned int *buffer, unsigned int file_offset)
{
	int err = 0;
	
	lseek(out_fd, file_offset, SEEK_SET);
	err = write_all(out_fd, buffer, c->min_io_size);
	if (err)
		return err_msg("write_all fail");
	
	return err;
}

static int mt_ftl_write_page(void)
{
	int err = 0;
	unsigned int leb = 0, page = 0;
	unsigned int data_hdr_offset = 0;
	unsigned int file_offset = 0;
	
	leb = PMT_LEB_PAGE_INDICATOR_GET_BLOCK(c->mtftl_param.u4NextLebPageIndicator);
	page = PMT_LEB_PAGE_INDICATOR_GET_PAGE(c->mtftl_param.u4NextLebPageIndicator);
	
	data_hdr_offset = c->min_io_size - (c->mtftl_param.u4DataNum * sizeof(struct mt_ftl_data_header) + 4);
	
	memcpy(&c->mtftl_param.u1DataCache[data_hdr_offset], &c->mtftl_param.u4Header[MTKFTL_MAX_DATA_NUM_PER_PAGE - c->mtftl_param.u4DataNum], c->mtftl_param.u4DataNum * sizeof(struct mt_ftl_data_header));
	memcpy(&c->mtftl_param.u1DataCache[c->min_io_size - 4], &c->mtftl_param.u4DataNum, 4);
	
	file_offset = leb * c->leb_size + page * c->min_io_size;
	err = write_to_file((unsigned int *)c->mtftl_param.u1DataCache, file_offset);
	if (err)
		return err_msg("write to file fail");
	
	page++;
	if (page == (unsigned int)(c->leb_size / c->min_io_size)) {
		leb++;
		c->mtftl_param.u4NextFreeLebIndicator++;
		page = 0;
	}
	PMT_LEB_PAGE_INDICATOR_SET_BLOCKPAGE(c->mtftl_param.u4NextLebPageIndicator, leb, page);
	
	memset(c->mtftl_param.u1DataCache, 0xFF, c->min_io_size * sizeof(unsigned char));
	memset(c->mtftl_param.u4Header, 0xFF, MTKFTL_MAX_DATA_NUM_PER_PAGE * sizeof(struct mt_ftl_data_header));
	c->mtftl_param.u4DataNum = 0;
	
	return err;
}

static int mt_ftl_commitPMT(void)
{
	int err = 0;
	int i = 0;
	unsigned int file_offset = 0;
	int pmt_block = 0, pmt_page = 0;
	
	for (i = 0; i < PMT_CACHE_NUM; i++) {
		if ((unsigned int)c->mtftl_param.i4CurrentPMTClusterInCache[i] == 0xFFFFFFFF)
			continue;
		if (!PMT_INDICATOR_IS_DIRTY(c->mtftl_param.u4PMTIndicator[c->mtftl_param.i4CurrentPMTClusterInCache[i]])){
			PMT_INDICATOR_RESET_INCACHE(c->mtftl_param.u4PMTIndicator[c->mtftl_param.i4CurrentPMTClusterInCache[i]]);
			c->mtftl_param.i4CurrentPMTClusterInCache[i] = 0xFFFFFFFF;
			continue;
		}
		
		pmt_block = PMT_LEB_PAGE_INDICATOR_GET_BLOCK(c->mtftl_param.u4CurrentPMTLebPageIndicator);
		pmt_page = PMT_LEB_PAGE_INDICATOR_GET_PAGE(c->mtftl_param.u4CurrentPMTLebPageIndicator);
		pmt_page += 2;
		if (pmt_page >= (c->leb_size / c->min_io_size)) {
			pmt_block++;
			c->mtftl_param.u4NextFreePMTLebIndicator++;
			pmt_page = 0;
		}
		PMT_LEB_PAGE_INDICATOR_SET_BLOCKPAGE(c->mtftl_param.u4CurrentPMTLebPageIndicator, pmt_block, pmt_page);
		
		file_offset = pmt_block * c->leb_size + pmt_page * c->min_io_size;
		err = write_to_file(&c->mtftl_param.u4PMTCache[i * PM_PER_NANDPAGE], file_offset);
		file_offset = pmt_block * c->leb_size + (pmt_page + 1) * c->min_io_size;
		err = write_to_file(&c->mtftl_param.u4MetaPMTCache[i * PM_PER_NANDPAGE], file_offset);
			
		PMT_INDICATOR_SET_BLOCKPAGE(c->mtftl_param.u4PMTIndicator[c->mtftl_param.i4CurrentPMTClusterInCache[i]], pmt_block,
						pmt_page, 0, i);
		PMT_INDICATOR_RESET_INCACHE(c->mtftl_param.u4PMTIndicator[c->mtftl_param.i4CurrentPMTClusterInCache[i]]);
		c->mtftl_param.i4CurrentPMTClusterInCache[i] = 0xFFFFFFFF;
	}
	
	return err;
}

static int mt_ftl_downloadPMT(int cluster, int cache_num)
{
	int err = 0;
	
	memset(&c->mtftl_param.u4PMTCache[cache_num * PM_PER_NANDPAGE], 0xFF, PM_PER_NANDPAGE * sizeof(unsigned int));
	memset(&c->mtftl_param.u4MetaPMTCache[cache_num * PM_PER_NANDPAGE], 0xFF, PM_PER_NANDPAGE * sizeof(unsigned int));
	
	c->mtftl_param.i4CurrentPMTClusterInCache[cache_num] = cluster;
	PMT_INDICATOR_SET_CACHE_BUF_NUM(c->mtftl_param.u4PMTIndicator[cluster], cache_num);
	
	return err;
}

static int mt_ftl_updatePMT(int cluster, int sec_offset, int lnum, int offset,
			int part, unsigned int cmpr_data_size)
{
	int err = 0;
	int i = 0;
	unsigned int *pmt = NULL;
	unsigned int *meta_pmt = NULL;
	
	if (!PMT_INDICATOR_IS_INCACHE(c->mtftl_param.u4PMTIndicator[cluster])) {	/* cluster is not in cache */
		for (i = 0; i < PMT_CACHE_NUM; i++) {
			if ((unsigned int)c->mtftl_param.i4CurrentPMTClusterInCache[i] == 0xFFFFFFFF)
				break;
			if (!PMT_INDICATOR_IS_DIRTY(c->mtftl_param.u4PMTIndicator[c->mtftl_param.i4CurrentPMTClusterInCache[i]]))
				break;
		}
		
		if (i == PMT_CACHE_NUM) {
			err = mt_ftl_commitPMT();
			if (err)
				return err_msg("commit PMT fail");
			i = 0;
		}
		
		if ((unsigned int)c->mtftl_param.i4CurrentPMTClusterInCache[i] != 0xFFFFFFFF) {
			PMT_INDICATOR_RESET_INCACHE(c->mtftl_param.u4PMTIndicator[c->mtftl_param.i4CurrentPMTClusterInCache[i]]);
			c->mtftl_param.i4CurrentPMTClusterInCache[i] = 0xFFFFFFFF;
		}
		
		err = mt_ftl_downloadPMT(cluster, i);
		if (err)
			return err_msg("download PMT fail");
	} else
		i = PMT_INDICATOR_CACHE_BUF_NUM(c->mtftl_param.u4PMTIndicator[cluster]);
		
	pmt = &c->mtftl_param.u4PMTCache[i * PM_PER_NANDPAGE + sec_offset];
	meta_pmt = &c->mtftl_param.u4MetaPMTCache[i * PM_PER_NANDPAGE + sec_offset];
	
	/* Update c->mtftl_param.u4PMTCache and c->mtftl_param.u4MetaPMTCache */
	PMT_SET_BLOCKPAGE(*pmt, lnum, offset / c->min_io_size);
	META_PMT_SET_DATA(*meta_pmt, cmpr_data_size, part, -1);	/* Data not in cache */
	PMT_INDICATOR_SET_DIRTY(c->mtftl_param.u4PMTIndicator[cluster]);
	
	return err;
}

static int write_ftl(void)
{
	int err = 0;
	unsigned int leb = 0, page = 0;
	unsigned int cluster = 0, sec_offset = 0;
	lzo_uint cmpr_len = 0;
	unsigned int data_offset = 0;
	unsigned int total_consumed_size = 0;
	unsigned long long sector = c->address >> 9;
	
	err = lzo1x_999_compress((void *)c->chunk_buffer, c->sparse_header.blk_sz, c->mtftl_param.cmpr_page_buffer, &cmpr_len, lzo_mem);
	if (err != LZO_E_OK)
		return err_msg("compress fail");
		
	if (c->mtftl_param.u4DataNum > 0) {
		data_offset = ((c->mtftl_param.u4Header[MTKFTL_MAX_DATA_NUM_PER_PAGE - c->mtftl_param.u4DataNum].offset_len >> 16) & 0xFFFF)
				+ (c->mtftl_param.u4Header[MTKFTL_MAX_DATA_NUM_PER_PAGE -c->mtftl_param.u4DataNum].offset_len & 0xFFFF);
	}
	total_consumed_size = data_offset + cmpr_len + (c->mtftl_param.u4DataNum + 1) * sizeof(struct mt_ftl_data_header) + 4;
	
	if ((total_consumed_size > (unsigned int)c->min_io_size) || (c->mtftl_param.u4DataNum >= MTKFTL_MAX_DATA_NUM_PER_PAGE)) {
		err = mt_ftl_write_page();
		if (err)
			return err_msg("Write data to page fail");
		data_offset = 0;
	}
	
	leb = PMT_LEB_PAGE_INDICATOR_GET_BLOCK(c->mtftl_param.u4NextLebPageIndicator);
	page = PMT_LEB_PAGE_INDICATOR_GET_PAGE(c->mtftl_param.u4NextLebPageIndicator);
	
	c->mtftl_param.u4Header[MTKFTL_MAX_DATA_NUM_PER_PAGE - c->mtftl_param.u4DataNum - 1].sector = (sector / (FS_PAGE_SIZE >> 9)) * (FS_PAGE_SIZE >> 9);
	c->mtftl_param.u4Header[MTKFTL_MAX_DATA_NUM_PER_PAGE - c->mtftl_param.u4DataNum - 1].offset_len = (data_offset << 16) | cmpr_len;
	if (c->mtftl_param.u4DataNum < 10) {
		printf("c->mtftl_param.u4Header[%d].sector = 0x%x\n", c->mtftl_param.u4DataNum, c->mtftl_param.u4Header[MTKFTL_MAX_DATA_NUM_PER_PAGE - c->mtftl_param.u4DataNum - 1].sector);
		printf("c->mtftl_param.u4Header[%d].offset_len = 0x%x\n", c->mtftl_param.u4DataNum, c->mtftl_param.u4Header[MTKFTL_MAX_DATA_NUM_PER_PAGE - c->mtftl_param.u4DataNum - 1].offset_len);
		printf("leb = %d, page = %d\n", leb, page);
	}
	
	memcpy(&c->mtftl_param.u1DataCache[data_offset], c->mtftl_param.cmpr_page_buffer, cmpr_len);
	c->mtftl_param.u4DataNum++;
	
	cluster = (sector / (FS_PAGE_SIZE >> 9)) / PM_PER_NANDPAGE;
	sec_offset = (sector / (FS_PAGE_SIZE >> 9)) & (PM_PER_NANDPAGE - 1);
	mt_ftl_updatePMT(cluster, sec_offset, leb, page * c->min_io_size, c->mtftl_param.u4DataNum - 1, cmpr_len);
	
	return err;
}

static int mt_ftl_commit_indicators(void)
{
	int err = 0;
	int index = 0;
	unsigned int file_offset = 0;
	
	memset(c->mtftl_param.commit_page_buffer, 0, (c->min_io_size >> 2) * sizeof(unsigned int));
	c->mtftl_param.commit_page_buffer[0] = MT_MAGIC_NUMBER;
	c->mtftl_param.commit_page_buffer[1] = c->mtftl_param.u4NextReplayOffsetIndicator;
	c->mtftl_param.commit_page_buffer[2] = c->mtftl_param.u4NextLebPageIndicator;
	c->mtftl_param.commit_page_buffer[3] = c->mtftl_param.u4CurrentPMTLebPageIndicator;
	c->mtftl_param.commit_page_buffer[4] = c->mtftl_param.u4NextFreeLebIndicator;
	c->mtftl_param.commit_page_buffer[5] = c->mtftl_param.u4NextFreePMTLebIndicator;
	c->mtftl_param.commit_page_buffer[6] = c->mtftl_param.u4GCReserveLeb;
	c->mtftl_param.commit_page_buffer[7] = c->mtftl_param.u4GCReservePMTLeb;
	index = 8;
	memcpy(&c->mtftl_param.commit_page_buffer[index], c->mtftl_param.u4PMTIndicator,
		PMT_TOTAL_CLUSTER_NUM * sizeof(unsigned int));
	index += ((PMT_TOTAL_CLUSTER_NUM * sizeof(unsigned int)) >> 2);
	memcpy(&c->mtftl_param.commit_page_buffer[index], c->mtftl_param.u4BIT,
		NAND_TOTAL_BLOCK_NUM * sizeof(unsigned int));
	index += ((NAND_TOTAL_BLOCK_NUM * sizeof(unsigned int)) >> 2);
	memcpy(&c->mtftl_param.commit_page_buffer[index], c->mtftl_param.i4CurrentPMTClusterInCache,
		PMT_CACHE_NUM * sizeof(unsigned int));
		
#ifdef DEVELOP
	printf("u4NextReplayOffsetIndicator = 0x%x\n", c->mtftl_param.u4NextReplayOffsetIndicator);
	printf("u4NextLebPageIndicator = 0x%x\n", c->mtftl_param.u4NextLebPageIndicator);
	printf("u4CurrentPMTLebPageIndicator = 0x%x\n", c->mtftl_param.u4CurrentPMTLebPageIndicator);
	printf("u4NextFreeLebIndicator = 0x%x\n", c->mtftl_param.u4NextFreeLebIndicator);
	printf("u4NextFreePMTLebIndicator = 0x%x\n", c->mtftl_param.u4NextFreePMTLebIndicator);
	printf("u4GCReserveLeb = 0x%x\n", c->mtftl_param.u4GCReserveLeb);
	printf("u4GCReservePMTLeb = 0x%x\n", c->mtftl_param.u4GCReservePMTLeb);
	printf("u4PMTIndicator = 0x%x, 0x%x, 0x%x, 0x%x\n",
			c->mtftl_param.u4PMTIndicator[0], c->mtftl_param.u4PMTIndicator[1], c->mtftl_param.u4PMTIndicator[2], c->mtftl_param.u4PMTIndicator[3]);
	printf("u4BIT = 0x%x, 0x%x, 0x%x, 0x%x\n",
			c->mtftl_param.u4BIT[0], c->mtftl_param.u4BIT[1], c->mtftl_param.u4BIT[2], c->mtftl_param.u4BIT[3]);
	printf("i4CurrentPMTClusterInCache = 0x%x, 0x%x, 0x%x, 0x%x\n",
			c->mtftl_param.i4CurrentPMTClusterInCache[0], c->mtftl_param.i4CurrentPMTClusterInCache[1],
			c->mtftl_param.i4CurrentPMTClusterInCache[2], c->mtftl_param.i4CurrentPMTClusterInCache[3]);
#endif
	
	file_offset = 0;
	err = write_to_file((unsigned int *)c->mtftl_param.commit_page_buffer, file_offset);
	if (err)
		return err_msg("write indicator to file fail");
	
	return err;
}

static int mt_ftl_commit(void)
{
	int err = 0;
	int i = 0;
	
	err = mt_ftl_commitPMT();
	if (err)
		return err_msg("mt_ftl commit PMT fail");
		
	for (i = 0; i < PMT_CACHE_NUM; i++) {
		if ((unsigned int)c->mtftl_param.i4CurrentPMTClusterInCache[i] == 0xFFFFFFFF)
			continue;
		PMT_INDICATOR_RESET_INCACHE(c->mtftl_param.u4PMTIndicator[c->mtftl_param.i4CurrentPMTClusterInCache[i]]);
		c->mtftl_param.i4CurrentPMTClusterInCache[i] = 0xFFFFFFFF;
	}
	
	/* Force to store c->mtftl_param.u1DataCache into flash */
	if (c->mtftl_param.u4DataNum) {
		err = mt_ftl_write_page();
		if (err)
			return err_msg("Write data to page fail");
	}
	
	err = mt_ftl_commit_indicators();
	if (err)
		return err_msg("mt_ftl commit indicators fail");
		
	return err;
}

static int write_data(void)
{
	int err = 0;
	unsigned int i = 0, j = 0;
	
	err = get_sparse_header();
	if (err)
		return err_msg("Get sparse header fail");
		
	if (c->sparse_header.blk_sz != 4096)
		return err_msg("sparse_header.blk_sz(%d) != 4096", c->sparse_header.blk_sz);
		
	/* allocate trunk buffer */
	c->chunk_buffer = malloc(c->sparse_header.blk_sz);
	if (c->chunk_buffer == NULL)
		return err_msg("allocate trunk buffer fail");
		
	for (i = 0; i < c->sparse_header.total_chunks; i++) {
		err = get_chunk_header();
		if (err)
			return err_msg("Get trunk header fail");
			
		if (c->chunk_header.chunk_type == CHUNK_TYPE_DONT_CARE) {
			unsigned long long len = (unsigned long long)c->chunk_header.chunk_sz * c->sparse_header.blk_sz;
			c->address += len;
			continue;
		}
		
		err = validate_header();
		if (err)
			return err_msg("Header information is wrong");
			
		if (c->chunk_header.chunk_type == CHUNK_TYPE_FILL) {
			err = get_fill_data();
			if (err)
				return err_msg("Get fill data fail");
		}
		
		for (j = 0; j < c->chunk_header.chunk_sz; j++) {
			/* get trunk data into trunk buffer */
			if (c->chunk_header.chunk_type == CHUNK_TYPE_RAW) {
				err = get_raw_data();
				if (err)
					return err_msg("Get raw data fail");
			} else if (c->chunk_header.chunk_type != CHUNK_TYPE_FILL)
				return err_msg("Unknown chunk type (%d)", c->chunk_header.chunk_type);
				
			/* write ftl */
			err = write_ftl();
			if (err)
				return err_msg("Write FTL fail");
			c->address += c->sparse_header.blk_sz;
		}
	}
	
	err = mt_ftl_commit();
	if (err)
		return err_msg("mt_ftl commit fail");
	
	/* free trunk buffer */
	free(c->chunk_buffer);
		
	return err;
}

static int mt_ftl_free_buffers(void)
{
	free(c->mtftl_param.u4PMTIndicator);
	free(c->mtftl_param.u4PMTCache);
	free(c->mtftl_param.u4MetaPMTCache);
	free(c->mtftl_param.i4CurrentPMTClusterInCache);
	free(c->mtftl_param.u4BIT);
	free(c->mtftl_param.u1DataCache);
	free(c->mtftl_param.u4Header);
	free(c->mtftl_param.commit_page_buffer);
	free(c->mtftl_param.cmpr_page_buffer);

	return 0;
}

static int mt_ftl_alloc_single_buffer(unsigned int **buf, int size, char *str)
{
	if (*buf == NULL) {
		*buf = malloc(size);
		if (!*buf)
			return err_msg("%s allocate memory fail", str);
		printf("%s = 0x%x, size = %d\n", str, (unsigned int)*buf, size);
	}
	memset(*buf, 0xFF, size);
	
	return 0;
}

static int mt_ftl_alloc_buffers(void)
{
	int err = 0;
	
	err = mt_ftl_alloc_single_buffer(&c->mtftl_param.u4PMTIndicator,
					PMT_TOTAL_CLUSTER_NUM * sizeof(unsigned int),
					"c->mtftl_param.u4PMTIndicator");
	if (err)
		return err_msg("allocate memory fail");
	
	err = mt_ftl_alloc_single_buffer(&c->mtftl_param.u4PMTCache,
					PM_PER_NANDPAGE * PMT_CACHE_NUM * sizeof(unsigned int),
					"c->mtftl_param.u4PMTCache");
	if (err)
		return err_msg("allocate memory fail");
	
	err = mt_ftl_alloc_single_buffer(&c->mtftl_param.u4MetaPMTCache,
					PM_PER_NANDPAGE * PMT_CACHE_NUM * sizeof(unsigned int),
					"c->mtftl_param.u4MetaPMTCache");
	if (err)
		return err_msg("allocate memory fail");
		
	err = mt_ftl_alloc_single_buffer((unsigned int **)&c->mtftl_param.i4CurrentPMTClusterInCache,
					PMT_CACHE_NUM * sizeof(unsigned int),
					"c->mtftl_param.i4CurrentPMTClusterInCache");
	if (err)
		return err_msg("allocate memory fail");
		
	err = mt_ftl_alloc_single_buffer(&c->mtftl_param.u4BIT,
					NAND_TOTAL_BLOCK_NUM * sizeof(unsigned int),
					"c->mtftl_param.u4BIT");
	if (err)
		return err_msg("allocate memory fail");
		
	err = mt_ftl_alloc_single_buffer((unsigned int **)&c->mtftl_param.u1DataCache,
					c->min_io_size * sizeof(unsigned char),
					"c->mtftl_param.u1DataCache");
	if (err)
		return err_msg("allocate memory fail");
		
	err = mt_ftl_alloc_single_buffer((unsigned int **)&c->mtftl_param.u4Header,
					MTKFTL_MAX_DATA_NUM_PER_PAGE * sizeof(struct mt_ftl_data_header),
					"c->mtftl_param.u4Header");
	if (err)
		return err_msg("allocate memory fail");
		
	err = mt_ftl_alloc_single_buffer(&c->mtftl_param.commit_page_buffer,
					(c->min_io_size >> 2) * sizeof(unsigned int),
					"c->mtftl_param.commit_page_buffer");
	if (err)
		return err_msg("allocate memory fail");
		
	err = mt_ftl_alloc_single_buffer((unsigned int **)&c->mtftl_param.cmpr_page_buffer,
					c->min_io_size * sizeof(unsigned char),
					"c->mtftl_param.cmpr_page_buffer");
	if (err)
		return err_msg("allocate memory fail");
		
	return err;
}

static int mt_ftl_init(void)
{
	int err = 0;
	
	err = mt_ftl_alloc_buffers();
	if (err)
		return err_msg("mt_ftl allocate buffers fail");
		
	lzo_mem = malloc(LZO1X_999_MEM_COMPRESS);
	if (!lzo_mem)
		return err_msg("allocate lzo memory fail");
		
	memset(c->mtftl_param.u4PMTIndicator, 0, PMT_TOTAL_CLUSTER_NUM * sizeof(unsigned int));
	memset(c->mtftl_param.u4BIT, 0, NAND_TOTAL_BLOCK_NUM * sizeof(unsigned int));
	
	c->mtftl_param.u4NextReplayOffsetIndicator = 0;
	c->mtftl_param.u4NextFreeLebIndicator = DATA_START_BLOCK + 1;
	c->mtftl_param.u4NextFreePMTLebIndicator = PMT_START_BLOCK + 1;
	c->mtftl_param.u4GCReserveLeb = NAND_DEFAULT_VALUE;
	c->mtftl_param.u4GCReservePMTLeb = NAND_DEFAULT_VALUE;
	c->mtftl_param.u4DataNum = 0;
	
	PMT_LEB_PAGE_INDICATOR_SET_BLOCKPAGE(c->mtftl_param.u4CurrentPMTLebPageIndicator, PMT_START_BLOCK, 0);
	PMT_LEB_PAGE_INDICATOR_SET_BLOCKPAGE(c->mtftl_param.u4NextLebPageIndicator, DATA_START_BLOCK, 0);
		
	return err;
}

static int init(void)
{
	int err = 0;
	
	in_fd = open(root, O_RDONLY | O_BINARY);
	if (in_fd == -1)
		return err_msg("cannot create input file '%s'", root);
		
	c->address = 0;
	
	err = mt_ftl_init();
	if (err)
		return err_msg("mt_ftl init fail");
	
	return err;
}

static int mt_ftl_deinit(void)
{
	int err = 0;
	
	free(lzo_mem);
	mt_ftl_free_buffers();
	
	return err;
}

static int deinit(void)
{
	int err = 0;
	
	err = mt_ftl_deinit();
	if (err)
		return err_msg("mt_ftl deinit fail");
	
	if (in_fd >= 0 && close(in_fd) == -1)
		return err_msg("cannot close the input file");	
	
	return err;
}

static int mkftl(void)
{
	int err = 0;

	err = init();
	if (err)
		goto out;
		
	err = write_data();
	
out:
	deinit();
	return err;
}

int main(int argc, char *argv[])
{
	int err;

	err = get_options(argc, argv);
	if (err)
		return err;
		
	err = open_target();
	if (err)
		return err;
		
	err = mkftl();
	if (err) {
		close_target();
		return err;
	}
	
	err = close_target();
	if (err)
		return err;

	if (verbose)
		printf("Success!\n");

	return 0;
}