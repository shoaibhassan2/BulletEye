#include <stdio.h>
#include <inttypes.h>
#include <windows.h>

#include <helpers/icon_changer.h>

extern int access(const char *path, int mode);

bool change_executable_icon(const char *const icon_path, const char *const executable_path)
{
	void *const icon_file = CreateFileA(icon_path, 0x80000000, 0, NULL, 3, 0, NULL);
	void *icon_info = NULL;
	int32_t image_offset = 0;
	int32_t image_size = 0;
	void *icon_read_buffer = NULL;
	void *update_resource = NULL;

	if (-1 == access(icon_path, 0))
	{
		(void)fprintf(stdout, "Icon not found!\n");
		return false;
	}

	if (-1 == access(executable_path, 0))
	{
		(void)fprintf(stdout, "Executable not found!\n");
		return false;
	}

	icon_info = malloc(22UL);
	if (NULL == icon_info)
	{
		(void)fprintf(stdout, "Couldn't allocate memory for icon information! (bytes: %" PRIu64 ")\n", 22UL);
		return false;
	}

	(void)ReadFile(icon_file, icon_info, 22, NULL, NULL);

	if (0 != *(int16_t *)icon_info || 1 != *(int16_t *)((int8_t *)icon_info + 2))
	{
		(void)fprintf(stdout, "Icon is not a valid .ico file!\n");
		free(icon_info);
		(void)CloseHandle(icon_file);

		return false;
	}

	image_offset = *(int32_t *)((int8_t *)icon_info + 18);
	image_size = *(int32_t *)((int8_t *)icon_info + 14);

	icon_read_buffer = malloc((size_t)image_size);
	if (NULL == icon_read_buffer)
	{
		(void)fprintf(stdout, "Couldn't allocate memory for icon buffer! (bytes: %" PRId32 ")\n", image_size);
		free(icon_info);
		(void)CloseHandle(icon_file);

		return false;
	}

	(void)SetFilePointer(icon_file, image_offset, NULL, 0);
	(void)ReadFile(icon_file, icon_read_buffer, image_size, NULL, NULL);
	(void)CloseHandle(icon_file);

	*(int16_t *)((int8_t *)icon_info + 4) = 1;
	*(int16_t *)((int8_t *)icon_info + 18) = 1;

	update_resource = BeginUpdateResourceA(executable_path, 0L);
	(void)UpdateResourceA(update_resource, (char *)3, (char *)1, 0, icon_read_buffer, image_size);
	(void)UpdateResourceA(update_resource, (char *)14, (char *)1, 0, icon_info, 20);
	(void)EndUpdateResourceA(update_resource, FALSE);

	free(icon_info);
	free(icon_read_buffer);

	return true;
}