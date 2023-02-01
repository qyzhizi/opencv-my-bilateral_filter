#include<iostream>
#include<opencv2/core/core.hpp>

using namespace std;
using namespace cv;

void myBilateralFilter(const Mat &src, Mat &dst, int ksize, double space_sigma, double color_sigma)
{
	int channels = src.channels();	// channels 为 src 的通道数
	CV_Assert(channels == 1 || channels == 3);	// src 的通道数为1或3时，继续执行后面的代码
	double space_coeff = -0.5 / (space_sigma * space_sigma);	// 公式一的系数
	double color_coeff = -0.5 / (color_sigma * color_sigma);	// 公式二的系数
	int radius = ksize / 2;	// 设 ksize = 3，则 radius = 1
	Mat temp;	// 计算 src 最外面一圈像素的新值时：若直接计算，则会访问不存在的像素。因此，需要补齐计算所需的像素。
	// void copyMakeBorder(InputArray src, OutputArray dst,int top, int bottom, int left, int right,int borderType, const Scalar& value = Scalar());
	copyMakeBorder(src, temp, radius, radius, radius, radius, BorderTypes::BORDER_REFLECT);	// 最上、最下各加1行，最左、最右各加1列，增加的部分与 src 关于边界对称，temp为补齐后的图像。
	vector<double> _color_weight(channels * 256);	// 由“像素值之差”决定的权重（模板上某像素与模板中心像素）。若 channels =1，“像素值之差”的值域为 [0,255] 。若 channels =3，“像素值之差”的值域为 [0,255*3] 。
	vector<double> _space_weight(ksize * ksize);	// 由“像素之间的距离”决定的权重，是一维向量，大小为 ksize * ksize
	vector<int> _space_ofs(ksize * ksize);	// 模板上某像素相对模板中心像素的位置偏移量
	double *color_weight = &_color_weight[0];	// color_weight 指向 _color_weight 的首元素
	double *space_weight = &_space_weight[0];	// space_weight 指向 _space_weight 的首元素
	int    *space_ofs = &_space_ofs[0];	// space_ofs 指向 _space_ofs 的首元素
	for (int i = 0; i < channels * 256; i++)	// 计算由“像素值之差”决定的权重
		color_weight[i] = exp(i * i * color_coeff);	// 见公式一

	int maxk = 0;	// 循环结束后，maxk = 9
	for (int i = -radius; i <= radius; i++)	// i = -1，0，1
	{
		for (int j = -radius; j <= radius; j++)	// j = -1，0，1
		{
			double r = sqrt(i*i + j * j);	// 计算模板上某像素（i，j）和模板中心像素（0，0）的欧式距离
			if (r > radius)
				continue;
			space_weight[maxk] = exp(r * r * space_coeff);	// 计算由“像素之间的距离”决定的权重。
			space_ofs[maxk++] = i * temp.step + j * channels;	// 设 temp 被 mask 覆盖的部分为 temp_mask ，计算 temp_mask 上的某像素相对 temp_mask 中心像素的位置偏移量
			// space_weight 与 space_ofs 均为一维向量，且长度相等，元素之间一一对应。某像素相对于中心像素的位置偏移量为 space_ofs[maxk]，该像素的空间权重为 space_weight[maxk]
		}
	}
	// 滤波过程
	for (int i = 0; i < src.rows; i++)	// i 表示行
	{
		// temp.data 指向 temp[0][0][B] ，temp.step 为 temp.cols * channels；i = 0 时，temp.data + (i + radius) * temp.step 指向 temp[radius][0][B]
		// i = 0 时，temp.data + (i + radius) * temp.step + radius * channels 指向 temp[radius][radius][B]，即 src[0][0][B] ，即 sptr 指向 src[0][0][B]
		// 一般的，sptr 指向 src[i][0][B]
		const uchar *sptr = temp.data + (i + radius) * temp.step + radius * channels;
		// dst.data 指向 dst[0][0][B]，dptr 指向 dst[i][0][B]
		uchar *dptr = dst.data + i * dst.step;
		if (channels == 1)
		{
			for (int j = 0; j < src.cols; j++)	// j 表示列，处理位于第 i 行第 j 列的像素
			{
				double sum = 0, wsum = 0;	// sum 为 temp_mask 中所有像素值的加权和，wsum 为 mask 中所有权重的和
				int val0 = sptr[j];	// temp_mask 中心的像素值。j = 0 时，(sptr+j) 指向 src[i][0]。一般的，(sptr+j) 指向 src[i][j] ，即 temp_mask 的中心像素
				for (int k = 0; k < maxk; k++)
				{
					int val = sptr[j + space_ofs[k]];	// 某像素 k 相对于中心像素的位置偏移量为 space_ofs[k]，该像素的空间权重为 space_weight[k]
					double w = space_weight[k] * color_weight[abs(val - val0)];	// 模板系数 = 由像素距离决定的权重 * 由像素差值决定的权重
					sum += val * w;
					wsum += w;
				}
				dptr[j] = (uchar)cvRound(sum / wsum);	// 将mask归一化，(dptr+j) 指向 dst[i][j]
			}
		}
		else if (channels == 3)
		{
			for (int j = 0; j < src.cols * 3; j += 3)	// j 表示列，注意 j 每次加3(即 j 为3的倍数)，以处理下一列的 [B][G][R] 子列
			{
				double sum_b = 0, sum_g = 0, sum_r = 0, wsum = 0;
				int b0 = sptr[j];	// sptr 指向 src[i][0][B] ，j = 0 时，(sptr+j) 指向 src[i][0][B] 。一般的，(sptr+j) 指向 src[i][j/3][B]
				int g0 = sptr[j + 1];	// j = 0 时，(sptr+j+1) 指向 src[i][0][G] 。一般的，(sptr+j+1) 指向 src[i][j/3][G]
				int r0 = sptr[j + 2];	// j = 0 时，(sptr+j+2) 指向 src[i][0][R] 。一般的，(sptr+j+2) 指向 src[i][j/3][R]
				for (int k = 0; k < maxk; k++)
				{
					// j = 0 时，(sptr+j) 指向 src[i][0][B] ，即 temp_mask 的中心像素的 [B]；若 k 也为0，则 sptr + j + space_ofs[k] 指向中心像素左上角的那个像素的 [B]
					// 某像素 k 相对于中心像素的位置偏移量为 space_ofs[k]，该像素的空间权重为 space_weight[k]
					const uchar *sptr_k = sptr + j + space_ofs[k];
					int b = sptr_k[0];	// sptr_k+0 指向中心像素左上角的那个像素的 [B]
					int g = sptr_k[1];	// sptr_k+1 指向中心像素左上角的那个像素的 [G]
					int r = sptr_k[2];		// sptr_k+2 指向中心像素左上角的那个像素的 [R]
					double w = space_weight[k] * color_weight[abs(b - b0) + abs(g - g0) + abs(r - r0)];
					sum_b += b * w;
					sum_g += g * w;
					sum_r += r * w;
					wsum += w;
				}
				wsum = 1.0f / wsum;
				b0 = cvRound(sum_b * wsum);
				g0 = cvRound(sum_g * wsum);
				r0 = cvRound(sum_r * wsum);
				dptr[j] = (uchar)b0;	// dptr 指向 dst[i][0][B] ，(dptr+j) 指向 dst[i][j/3][B]
				dptr[j + 1] = (uchar)g0;	// (dptr+j+1) 指向 dst[i][j/3][G]
				dptr[j + 2] = (uchar)r0;	// (dptr+j+2) 指向 dst[i][j/3][R]
			}
		}
	}
}

