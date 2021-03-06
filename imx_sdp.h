/*
 * imx_sdp:
 * Interface of the Serial Download Protocol (SDP) for i.MX/Vybrid
 * series processors.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __IMX_SDP_H__
#define __IMX_SDP_H__

struct ram_area {
	unsigned start;
	unsigned size;
};

struct mem_work {
	struct mem_work *next;
	unsigned type;
#define MEM_TYPE_READ		0
#define MEM_TYPE_WRITE		1
#define MEM_TYPE_MODIFY		2
	unsigned vals[3];
};

struct sdp_work;
struct sdp_work {
	struct sdp_work *next;
	struct mem_work *mem;
	char filename[256];
	unsigned char dcd;
	unsigned char clear_dcd;		//means clear dcd_ptr
	unsigned char clear_boot_data;		//means clear boot data ptr
	unsigned char plug;
#define J_ADDR		1
#define J_HEADER	2
#define J_HEADER2	3
	unsigned char jump_mode;
	unsigned load_addr;
	unsigned jump_addr;
	unsigned load_size;
};

struct sdp_dev {
	char name[64];
	unsigned short max_transfer;
#define MODE_HID	0
#define MODE_BULK	1
	unsigned char mode;
#define HDR_NONE	0
#define HDR_MX51	1
#define HDR_MX53	2
#define HDR_UBOOT	3
	unsigned char header_type;
	unsigned dcd_addr;
	struct ram_area ram[8];
	struct sdp_work *work;
	/*
	 * dev - SDP devce (this structure)
	 * report - HID Report
	 * p - pointer to buffer
	 * size - size of buffer (used for send and USB receive length)
	 * expected - the expected amount of data (used for UART receive)
	 * last_trans - the actually transfered bytes
	 */
	int (*transfer)(struct sdp_dev *dev, int report, unsigned char *p, unsigned int size,
			unsigned int expected, int* last_trans);
	void *priv;
};

#define HAB_SECMODE_PROD 0x12343412
#define HAB_SECMODE_DEV  0x56787856

/*
 * Section 8.7.2 of the i.MX6DQ/UL/SoloX RM:
 * The maximum size of the DCD limited to 1768 bytes.
 */
#define HAB_MAX_DCD_SIZE 1768

#define SDP_READ_REG     0x0101
#define SDP_WRITE_REG    0x0202
#define SDP_WRITE_FILE   0x0404
#define SDP_ERROR_STATUS 0x0505
#define SDP_WRITE_DCD    0x0a0a
#define SDP_JUMP_ADDRESS 0x0b0b

#pragma pack (1)
struct sdp_command {
	uint16_t cmd;
	uint32_t addr;
	uint8_t format;
	uint32_t cnt;
	uint32_t data;
	uint8_t rsvd;
};
#pragma pack ()

int get_val(const char** pp, int base);
const char *move_string(char *dest, const char *src, unsigned cnt);
void dump_bytes(unsigned char *src, unsigned cnt, unsigned addr);

char const *get_global_conf_path(void);
char const *get_base_path(char const *argv0);
char const *conf_file_name(char const *file, char const *base_path, char const *conf_path);
struct sdp_dev *parse_conf(const char *filename);
struct sdp_work *parse_cmd_args(int argc, char * const *argv);

void perform_mem_work(struct sdp_dev *dev, struct mem_work *mem);
int do_status(struct sdp_dev *dev);

int DoIRomDownload(struct sdp_dev *dev, struct sdp_work *curr, int verify);

#endif /* __IMX_SDP_H__ */
