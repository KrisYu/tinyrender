# 500行代码学懂OpenGL


项目来自[tinyrenderer](https://github.com/ssloy/tinyrenderer)，作者尝试用500行代码来写一个tiny render，让我们来明白OpenGL是怎样工作的，这个系列可以理解为我的读书笔记或者心得或翻译。

这500多行代码包含的内容太多，它模仿了OpenGL，从零开始构建了一个光栅化渲染器，而一切都是始于从在屏幕上画点开始。

为了生成图片，在屏幕上画点，我们使用的是 TGAImage，它用起来很简单：



```C++
#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

int main(int argc, char** argv){
  TGAImage image(100, 100, TGAImage::RGB);
  image.set(52, 41, red);
  image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
  image.write_tga_file("output.tga");
  return 0;
}
```

最终就凭着画点的思想我们一步一步构建，目前我们可以生成的图像是这样：

![](images/phong_light.png)


Very cool!

