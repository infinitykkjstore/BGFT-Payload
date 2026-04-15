#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include "struct.h"

struct
{
	uint32_t addr;
	uint16_t port;

} __attribute__((packed)) volatile Connection = { 0xb4b4b4b4, 0xb4b4 };

uint32_t get_uint32(int sock) {
	char dword[4];
	read(sock, dword, sizeof(dword));
	return *(uint32_t*)dword;
}

uint64_t get_uint64(int sock) {
	char qword[8];
	read(sock, qword, sizeof(qword));
	return *(uint64_t*)qword;
}

int strlen(char* buffer) {
	int Len = 0;
	while (buffer[Len] != 0)
		Len++;
	return Len;
}

int get_string(int sock, char* Buffer, int MAX_SIZE) {

	uint32_t str_len = get_uint32(sock);

	Buffer[0] = 0;
	if (str_len > MAX_SIZE)
		return -1;

	read(sock, Buffer, str_len);
	return 0;
}

void append_string(char* buffer, char* new_content) {
	int ori_len = strlen(buffer);
	char* pNew = (char*)(buffer + ori_len);

	int nLen = strlen(new_content);
	for (int i = 0; i < nLen; i++)
		pNew[i] = new_content[i];
}

void get_file_path(char* path, char* fname) {
	path[0] = 0;

	char dir[16] = "/user/data/tmp_";

	append_string(path, dir);
	append_string(path, fname);
}

int get_file(int sock, char* out_path, char* name) {
	out_path[0] = 0;
	int len = get_uint32(sock);
	if (len == 0)
		return 0;

	get_file_path(out_path, name);

	int hFile = open(out_path, O_WRONLY | O_CREAT | O_TRUNC, 0777);

	char buffer[4096];

	int buffer_size = sizeof(buffer);

	ssize_t readed = -1;
	while ((readed = read(sock, buffer, (len < buffer_size) ? len : buffer_size)) > 0) {
		len -= readed;
		char* pBuff = buffer;
		while (readed > 0) {
			ssize_t written = write(hFile, pBuff, readed);
			if (written <= 0)
				return 1;

			pBuff += written;
			readed -= written;
		}

		if (len <= 0)
			break;
	}

	close(hFile);
	return 1;
}


int get_pkg_info(struct bgft_download_param* params)
{
    static char url[] = "http://orbisdev.infinitydev.shop/PS4%200/Jogos/bully/UP9000-CUSA03507_00-SLUS212690000001-A0100-V0102.pkg";
    static char name[] = "Teste Bully";
    static char id[] = "UP9000-CUSA03507_00-SLUS212690000001";
    static char icon_path[] = "http://orbisdev.infinitydev.shop/icons/7.png";
    static char pkg_type[] = "PS4GD";

    params->id = id;
    params->content_url = url;
    params->content_name = name;
    params->package_size = 2593128448; // opcional
    params->icon_path = icon_path;
    params->package_type = pkg_type;

    return 0;
}