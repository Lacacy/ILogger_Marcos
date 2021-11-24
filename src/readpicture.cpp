#include <dirent.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#define FILENAME_LENGTH 256  //文件名字符长度


using namespace std;

/**
 * @brief 递归遍历得到一个文件夹下所有子文件夹的图片
 * @param basePath 文件夹名字
 * @param g_image_filename_list 存储所有子文件的路径的二维数组
 * @param image_number 定义传入地址的 int 型变量，用来存放 一共有多少个图片
 * @return int 如果成功返回 1
 */
int readFileList(char *basePath, char g_image_filename_list[][FILENAME_LENGTH], int *image_number)
{
    DIR *dir;  // 文件夹指针
    struct dirent *entry;
    char base[1000];
    int namelen = 0;

    int full_name_len = 0;
    char image_file_ext[] = "png";    // 读取文件的后缀

    char filename[FILENAME_LENGTH] = {0}; 

    static int image_counter = 0; 

    // 基于名称上打开目录流，返回目录上的DIR流，如果无法打开，返回 NULL
    if ((dir = opendir(basePath)) == NULL)
    {
        perror("Open dir error...");
        // 表示非正常运行导致程序退出 
        // exit(0) 程序清除缓存，正常退出
        exit(1); 
    }

    // 当读取目录流结果不为空时
    while ((entry=readdir(dir)) != NULL)
    {   
        // 如果文件名字是“ . || .. ” current dir OR parrent dir
        if(strcmp(entry->d_name,".")==0 || strcmp(entry->d_name,"..")==0)
            continue;
        else if(entry->d_type == 8)    ///file
        {
            // printf("d_name[%d]:%s/%s\n",image_counter, basePath,entry->d_name);  //wjh-
            //////////////////////////////////////////////
            namelen = strlen(entry->d_name);  
            // (手动指定了png), jpg, bmp, jpeg
            if(entry->d_name[namelen-4] == '.' && ( entry->d_name[namelen-3] == image_file_ext[0] || entry->d_name[namelen-3] == 'j' || entry->d_name[namelen-3] == 'b') 
                && ( entry->d_name[namelen-2] == image_file_ext[1] || entry->d_name[namelen-2] == 'p' || entry->d_name[namelen-2] == 'm') && ( entry->d_name[namelen-1] 
                == image_file_ext[2] || entry->d_name[namelen-1] == 'g' || entry->d_name[namelen-1] == 'p') || (entry->d_name[namelen-5] == '.' && entry->d_name[namelen-4] 
                == 'j' && entry->d_name[namelen-3] == 'p' && entry->d_name[namelen-2] == 'e' && entry->d_name[namelen-1] == 'g'))    
            {
                //ptr->d_name[namelen-4] = '\0';
                sprintf(filename, "%s/%s", basePath, entry->d_name);
                // 去掉行尾的空格
                //printf("image: %s\n", filename);

                full_name_len = strlen(filename);
                strcpy(g_image_filename_list[image_counter], filename);
                image_counter++;
            }
        }
        // 文件类型为链接
        else if(entry->d_type == 10)
        {
            ;
            // 如果是链接打印当前 链接的名字
            // printf("d_name: %s/%s\n",basePath,entry->d_name); 
        }
        // 文件类型为文件夹
        else if(entry->d_type == 4)    ///dir
        {
            memset(base,'\0',sizeof(base));
            strcpy(base, basePath);          // 复制父目录 到 base    
            strcat(base, "/");               // 追加字符为 "/"
            strcat(base, entry->d_name);     // 追加子目录名字
            readFileList(base, g_image_filename_list, image_number);  // 进入子目录继续读取图片信息 <-- 递归
        }
    }
    closedir(dir);

    *image_number = image_counter;

    return 1;
}

#if 0 
int main(int argc, char **argv){


    // 测试 OK
    char image_file_list[20000][FILENAME_LENGTH];
    printf("Directory scan of %s\n", argv[1]);
    int nImgNum = 0;
    readFileList(argv[1], image_file_list, &nImgNum);
    int nPathLength = strlen(image_file_list[1]);
    // 改方法确实可以返回二维数组中1维数组的长度，但是返回不了实际被填充的长度
    int nLength = sizeof(image_file_list) / sizeof(image_file_list[0]);
    printf("长度: %d %d %d\n", nLength, nPathLength, nImgNum);
    for( int i=0; i < nImgNum; ++i)
    {
        printf("%s \n", image_file_list[i]);
    }

}
#endif