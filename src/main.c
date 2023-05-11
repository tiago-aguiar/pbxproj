#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <unistd.h>
#include <time.h>

#define array_size(array) (sizeof(array) / sizeof(array[0]))

typedef struct Dict
{
    const char* key;
    const char* value;
} Dict;

void create_dir(char *result, const char *root, const char *path)
{
    char absolute_path[128];
    strncpy(absolute_path, root, strlen(root) + 1);
    strncat(absolute_path, path, strlen(path));
    strncat(absolute_path, "/", 1);

    if (mkdir(absolute_path, 0755) != 0) {
        fprintf(stderr, "ERROR(%d) %s: Could not create a directory `%s`\n", errno, strerror(errno), absolute_path);
        exit(1);
    }

    memmove(result, absolute_path, sizeof(absolute_path));
}

void search_pattern(char *buf, const Dict dicts[], const size_t n)
{
    char *begin = strchr(buf, '{');
    if (begin == NULL) return;

    if (begin != NULL && begin[1] == '{') {
        begin += 2;
        char *end = strchr(begin, '}');

        if (end != NULL && end[1] == '}') {

            char identifier[255] = "";
            strncpy(identifier, begin, strlen(begin) - strlen(end));

            for (size_t i = 0; i < n; ++i) {
                Dict d = dicts[i];

                if (strcmp(identifier, d.key) == 0) { // find key
                    char new_line[255] = "";
                    strncat(new_line, buf, strlen(buf) - strlen(begin) - 2);
                    strncat(new_line, d.value, strlen(d.value));
                    strncat(new_line, end + 2, strlen(end));
                    strcpy(buf, new_line);

                    break;

                }
            }

            search_pattern(begin+1, dicts, n);
        }
    } else {
        search_pattern(begin+1, dicts, n);
    }
}

FILE *open_file(const char *dir, const char *filename, const char *mode)
{
    char path[128];
    strncpy(path, dir, sizeof(path));
    strncat(path, filename, sizeof(path) - strlen(path) - 1);
 
    FILE *file = fopen(path, mode);
    if (!file) {
        fprintf(stderr, "ERROR: File %s not found.", path);
        exit(1);
    }
    
    return file;
}


void write_template(FILE *src, FILE *dest, const Dict dicts[], const size_t n)
{
    // NOTE: This used a stack buffer and only accept a string line with max 511 bytes (0x1ff)
    // We must change to dynamic memory ?
    size_t buf_size = 0x1ff;
    int line_count = 1;
    while(!feof(src)) {
        char buf[buf_size];
        if (fgets(buf, buf_size, src)) {
            
            if (!strrchr(buf, '\n')) {
                fprintf(stderr, "ERROR: The line '%d' is greater than %d chars\n", line_count, (int)buf_size);
                exit(1);
            }
            line_count++;

            search_pattern(buf, dicts, n);

            fputs(buf, dest);
        }
    }
}

void make_app(const char *src_absolute_path, const char *project_name, const char *current_time)
{
    FILE *src = open_file("../data", "/App.swift", "r");
    char filename[127];
    strncat(filename, "/", 1);
    strncat(filename, project_name, strlen(project_name));
    strncat(filename, "App.swift", 9);
    
    FILE *dest = open_file(src_absolute_path, filename, "w");

    Dict dicts[] = {
        {
            .key = "project_name",
            .value = project_name
        },
        {
            .key = "date",
            .value = current_time
        },
    };

    write_template(src, dest, dicts, array_size(dicts));

    fclose(dest);
    fclose(src);
}

void make_json_preview(const char *src_absolute_path)
{
    FILE *src = open_file("../data", "/Previews.json", "r");
    FILE *dest = open_file(src_absolute_path, "/Contents.json", "w");

    write_template(src, dest, NULL, 0);

    fclose(dest);
    fclose(src);
}

void make_json_assets(const char *src_absolute_path)
{
    FILE *src = open_file("../data", "/AssetsContents.json", "r");
    FILE *dest = open_file(src_absolute_path, "/Contents.json", "w");

    write_template(src, dest, NULL, 0);

    fclose(dest);
    fclose(src);
}

void make_json_appicon(const char *src_absolute_path)
{
    FILE *src = open_file("../data", "/AppIcon.json", "r");
    FILE *dest = open_file(src_absolute_path, "/Contents.json", "w");

    write_template(src, dest, NULL, 0);

    fclose(dest);
    fclose(src);
}

void make_json_color(const char *src_absolute_path)
{
    FILE *src = open_file("../data", "/AccentColor.json", "r");
    FILE *dest = open_file(src_absolute_path, "/Contents.json", "w");

    write_template(src, dest, NULL, 0);

    fclose(dest);
    fclose(src);
}

void make_launch_script(const char *src_absolute_path, const char *project_name, const char *sim_uuid)
{
    FILE *src = open_file("../data", "/launch.sh", "r");
    FILE *dest = open_file(src_absolute_path, "/launch.sh", "w");
    char buf[255];
    strncpy(buf, src_absolute_path, strlen(src_absolute_path) + 1);
    strncat(buf, "/launch.sh", 10);

    chmod(buf, 0755);

    Dict dicts[] = {
        {
            .key = "project_name",
            .value = project_name
        },
        {
            .key = "sim_uuid",
            .value = sim_uuid
        },
    };

    write_template(src, dest, dicts, array_size(dicts));

    fclose(dest);
    fclose(src);
}

void make_build_script(const char *src_absolute_path, const char *project_name, const char *sim_uuid)
{
    FILE *src = open_file("../data", "/build.sh", "r");
    FILE *dest = open_file(src_absolute_path, "/build.sh", "w");
    char buf[255];
    strncpy(buf, src_absolute_path, strlen(src_absolute_path) + 1);
    strncat(buf, "/build.sh", 9);

    chmod(buf, 0755);

    Dict dicts[] = {
        {
            .key = "project_name",
            .value = project_name
        },
        {
            .key = "sim_uuid",
            .value = sim_uuid
        },
    };

    write_template(src, dest, dicts, array_size(dicts));

    fclose(dest);
    fclose(src);
}

void make_content_view(const char *src_absolute_path, const char *project_name, const char *current_time)
{
    FILE *src = open_file("../data", "/ContentView.swift", "r");
    FILE *dest = open_file(src_absolute_path, "/ContentView.swift", "w");

    Dict dicts[] = {
        {
            .key = "project_name",
            .value = project_name
        },
        {
            .key = "date",
            .value = current_time
        },
    };

    write_template(src, dest, dicts, array_size(dicts));

    fclose(dest);
    fclose(src);
}

void make_pbxproj(const char *src_absolute_path, const char *project_name, const char *current_time)
{
    FILE *src = open_file("../data", "/project.pbxproj", "r");
    FILE *dest = open_file(src_absolute_path, "/project.pbxproj", "w");

    Dict dicts[] = {
        {
            .key = "project_name",
            .value = project_name
        },
        {
            .key = "date",
            .value = current_time
        },
    };

    write_template(src, dest, dicts, array_size(dicts));

    fclose(dest);
    fclose(src);
}

int main()
{
    // TODO: get bundle identifier
    const char *project_name = "HelloWorld";
    const char *sim_uuid = "C34C5900-105F-482E-A62D-EF785812C1E2";
    
    // current time
    time_t tm_now = time(NULL);
    char *now = ctime(&tm_now);
    now[strlen(now) - 1] = '\0';


    // TODO: ask for overwrite!
    char root_path[255];
    create_dir(root_path, "./", project_name);

    char src_path[255];
    create_dir(src_path, root_path, project_name); // create src folder

    char assets_path[255];
    create_dir(assets_path, src_path, "Assets.xcassets");
    make_json_assets(assets_path);

    char accent_color_path[255];
    create_dir(accent_color_path, assets_path, "AccentColor.colorset");
    make_json_color(accent_color_path);

    char app_icon_path[255];
    create_dir(app_icon_path, assets_path, "AppIcon.appiconset");
    make_json_appicon(app_icon_path);

    char preview_path[255];
    create_dir(preview_path, src_path, "Preview Content");

    char preview_content_path[255];
    create_dir(preview_content_path, preview_path, "Preview Assets.xcassets");
    make_json_preview(preview_content_path);

    //
    // Make Source Code
    //
    make_content_view(src_path, project_name, now);
    make_app(src_path, project_name, now);
    make_build_script(root_path, project_name, sim_uuid);
    make_launch_script(root_path, project_name, sim_uuid);

    //
    // Xcodeproj
    //
    char xcodeproj_folder[128];
    strncpy(xcodeproj_folder, project_name, sizeof(xcodeproj_folder));
    strncat(xcodeproj_folder, ".xcodeproj", 10);


    char xcodeproj_path[255];
    create_dir(xcodeproj_path, root_path, xcodeproj_folder);
    make_pbxproj(xcodeproj_path, project_name, now);

#if 0

    /*
    // NOTE: This used a stack buffer and only accept a string line with max 511 bytes (0x1ff)
    // We must change to dynamic memory ?
    size_t buf_size = 0x1ff;
    int line_count = 1;
    while(!feof(content_view_src)) {
        char buf[buf_size];
        if (fgets(buf, buf_size, content_view_src)) {
            
            if (!strrchr(buf, '\n')) {
                fprintf(stderr, "ERROR: The line '%d' is greater than %d chars\n", line_count, (int)buf_size);
                exit(1);
            }
            line_count++;

            replace_template(buf, "date", now);
            replace_template(buf, "project_name", project_name);

            printf("%s", buf);

            fputs(buf, content_view_dest);
        }
    }
    */


#endif
    return 0;
}
