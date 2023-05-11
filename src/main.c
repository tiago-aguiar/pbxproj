#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <time.h>

void create_dir(const char *dir_name)
{
    if (mkdir(dir_name, 0755) != 0) {
        fprintf(stderr, "ERROR(%d) %s: Could not create a directory `%s`\n", errno, strerror(errno), dir_name);
        exit(1);
    }
}

void create_subdir(const char *root, const char *path)
{
    char folder[128];

    strncpy(folder, root, sizeof(folder));
    strncat(folder, path, sizeof(folder) - strlen(folder) - 1);
    create_dir(folder);
}

void replace_template(char *str, const char *old_key, const char *new_string)
{
    char *begin = strchr(str, '{');
    if (begin != NULL && begin[1] == '{') {
        begin += 2;
        char *end = strchr(begin, '}');

        if (end != NULL && end[1] == '}') {

            char identifier[255] = "";
            strncpy(identifier, begin, strlen(begin) - strlen(end));

            if (strcmp(identifier, old_key) == 0) { // find key
                char new_line[255] = "";
                strncat(new_line, str, strlen(str) - strlen(begin) - 2);
                strncat(new_line, new_string, strlen(new_string));
                strncat(new_line, end + 2, strlen(str) - strlen(end));
                strcpy(str, new_line);
            }
            /*
            char tmp_end[127] = "";

            strcpy(tmp_end, end + 2);

            if (strcmp(identifier, old_key) == 0) { // find key
                memset(begin - 2, '\0', strlen(str));
                memmove(begin - 2, new_string, strlen(new_string));
                memmove(str + strlen(str), tmp_end, strlen(tmp_end));
            }
            */
        }
    }
}

int main()
{
    // TODO: ask for overwrite!
    const char absolute_project_path[] = "./Template2";
    char *last_path = strrchr(absolute_project_path, '/');
    char project_name[0x7f]; 
    memmove(project_name, last_path + 1, strlen(last_path));

    // current time
    time_t tm_now = time(NULL);
    char *now = ctime(&tm_now);
    now[strlen(now) - 1] = '\0';

    create_dir(absolute_project_path);
    create_subdir(absolute_project_path, "/Assets.xcassets");
    create_subdir(absolute_project_path, "/Preview Content");

    char xcodeproj_folder[128];
    strncpy(xcodeproj_folder, absolute_project_path, sizeof(xcodeproj_folder));
    strncpy(xcodeproj_folder, ".xcodeproj", sizeof(xcodeproj_folder));
    create_subdir(absolute_project_path, xcodeproj_folder);

    const char *data_folder = "../data";

    char content_view_path[128];
    strncpy(content_view_path, data_folder, sizeof(content_view_path));
    strncat(content_view_path, "/ContentView.swift", sizeof(content_view_path) - strlen(content_view_path) - 1);
    
    FILE *fcontent_view = fopen(content_view_path, "r");
    if (!fcontent_view) {
        fprintf(stderr, "ERROR: File %s not found.", content_view_path);
        exit(1);
    }

    // NOTE: This used a stack buffer and only accept a string line with max 511 bytes (0x1ff)
    // We must change to dynamic memory ?
    size_t buf_size = 0x1ff;
    int line_count = 1;
    while(!feof(fcontent_view)) {
        char buf[buf_size];
        if (fgets(buf, buf_size, fcontent_view)) {
            
            if (!strrchr(buf, '\n')) {
                fprintf(stderr, "ERROR: The line '%d' is greater than %d chars\n", line_count, (int)buf_size);
                exit(1);
            }
            line_count++;

            replace_template(buf, "date", now);
            replace_template(buf, "project_name", project_name);

            printf("%s", buf);
        }
    }

    fclose(fcontent_view);
    return 0;
}
