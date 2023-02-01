# opencv-my-bilateral_filter

## 环境要求

- Linux 桌面版 , cmake, c++编译环境
- OpenCV c++库

### 运行效果

```c++
	int ksize = 20;
	double space_sigma = 80;
	double color_sigma = 80;
	myBilateralFilter(src, dst1,ksize,space_sigma,color_sigma);
	bilateralFilter(src, dst2, ksize, space_sigma, color_sigma);
	imshow("src", src);
	imshow("dst1-myBilateralFilter", dst1);
	imshow("dst2-bilateralFilter", dst1);
```



**原图**

![](https://qyzhizi.cn/img/202302011649535.png)

**myBilateralFilter 的效果**

![](https://qyzhizi.cn/img/202302011643006.png)

**opencv bilateralFilter 效果**

![](https://qyzhizi.cn/img/202302011646018.png)

主要代码非原创，来自：https://blog.csdn.net/qq_21603315/article/details/122565811

## 参考：

- [双边滤波原理](https://zhuanlan.zhihu.com/p/127023952)

- [双边滤波代码1](https://www.cnblogs.com/wangguchangqing/p/6416401.html)
- [双边滤波代码2](https://blog.csdn.net/qq_21603315/article/details/122565811)
- [opencv 高斯滤波](https://docs.opencv.org/4.x/dc/dd3/tutorial_gausian_median_blur_bilateral_filter.html)
- [C++ extern声明](https://blog.csdn.net/HayPinF/article/details/113942824)
- [构建 OpenCV C++ 项目](https://zhuanlan.zhihu.com/p/349137661)
- [CMake项目中引入OpenCV](https://blog.csdn.net/u013238941/article/details/118527040)

 
