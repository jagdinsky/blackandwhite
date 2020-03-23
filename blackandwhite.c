#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

int maximum(int a, int b, int c, int d) {
    if (a < b)
        a = b;
    if (c < d)
        c = d;
    if (a < c)
        return c;
    return a;
}

struct Pixel {
    unsigned char r, g, b;
};

struct Image {
    char preheader[18];
    int w, h;
    char postheader[28];
    struct Pixel **pixmap;
};

struct Image openImage(char *filename) {
    struct Image img;
    int fd = open(filename, O_RDONLY);
    read(fd, &img.preheader, sizeof(img.preheader));
    read(fd, &img.w, sizeof(img.w));
    read(fd, &img.h, sizeof(img.h));
    read(fd, &img.postheader, sizeof(img.postheader));
    img.pixmap = (struct Pixel **) malloc(img.h * sizeof(struct Pixel *));
    for (int i = 0; i < img.h; i++)
        img.pixmap[i] = (struct Pixel *) malloc(img.w * sizeof(struct Pixel));
    for (int i = 0; i < img.h; i++)
        for (int j = 0; j < img.w; j++)
            read(fd, &(img.pixmap[i][j]), sizeof(img.pixmap[i][j]));
    close(fd);
    return img;
}

void saveImage(struct Image img, char *filename, int value) {
    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC,
                    S_IWUSR | S_IWGRP | S_IWOTH |
                    S_IRUSR | S_IRGRP | S_IROTH);
    write(fd, &img.preheader, sizeof(img.preheader));
    write(fd, &img.w, sizeof(img.w));
    write(fd, &img.h, sizeof(img.h));
    write(fd, &img.postheader, sizeof(img.postheader));
    int **intensity = malloc(img.h * sizeof(int *));
    for (int i = 0; i < img.h; i++) {
        intensity[i] = malloc(img.w * sizeof(int));
        for (int j = 0; j < img.w; j++) {
            intensity[i][j] = (img.pixmap[i][j]).r +
                        (img.pixmap[i][j]).g + (img.pixmap[i][j]).b;
        }
    }
    struct Pixel *temp = malloc(sizeof(struct Pixel));
    int maxi, one, two, three, four;
    for (int j = 0; j < img.w; j++) {
        temp -> r = 255;
        temp -> g = 255;
        temp -> b = 255;
        write(fd, temp, sizeof(*temp));
    }
    for (int i = 1; i < img.h - 1; i++) {
        temp -> r = 255;
        temp -> g = 255;
        temp -> b = 255;
        write(fd, temp, sizeof(*temp));
        for (int j = 1; j < img.w - 1; j++) {
            one = intensity[i][j] - intensity[i][j + 1];
            if (one < 0)
                one *= -1;
            two = intensity[i][j] - intensity[i][j - 1];
            if (two < 0)
                two *= -1;
            three = intensity[i][j] - intensity[i - 1][j];
            if (three < 0)
                three *= -1;
            four = intensity[i][j] - intensity[i + 1][j];
            if (four < 0)
                four *= -1;
            maxi = maximum(one, two, three, four);
            if (maxi > value) {
                temp -> r = 0;
                temp -> g = 0;
                temp -> b = 0;
            } else {
                temp -> r = 255;
                temp -> g = 255;
                temp -> b = 255;
            }
            write(fd, temp, sizeof(*temp));
        }
        temp -> r = 255;
        temp -> g = 255;
        temp -> b = 255;
        write(fd, temp, sizeof(*temp));
    }
    for (int j = 0; j < img.w; j++) {
        temp -> r = 255;
        temp -> g = 255;
        temp -> b = 255;
        write(fd, temp, sizeof(*temp));
    }
    free(temp);
    for (int i = 0; i < img.h; i++)
        free(intensity[i]);
    free(intensity);
    close(fd);
}

int main(int argc, char **argv) {
    struct Image img = openImage(argv[1]);
    int value = atoi(argv[3]);
    saveImage(img, argv[2], value);
    for (int i = 0; i < img.h; i++)
        free(img.pixmap[i]);
    free(img.pixmap);
    return 0;
}
