#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio/videoio.hpp>

#include <iostream>

using namespace cv;


int choix()
{
	int r = -1;
	std::cout << "\nFunctions : \n" << std::endl;
	std::cout << "Exit : 0" << std::endl;
	std::cout << "Dilatation / Erosion : 1" << std::endl;
	std::cout << "Resizing : 2" << std::endl;
	std::cout << "Lighten / Darken : 3" << std::endl;
	std::cout << "Panorama / stitching : 4" << std::endl;

	while (r<0 || r>4)
	{
		std::cout << "\nChoix : " << std::endl;
		std::cin >> r;
	}
	return r;
}

Mat image;
int W = 0;
int H = 0;
int mode = 0;
int strength = 0;

void dilatation_erosion(int value, void*)
{
	Mat new_image;

	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(2 * strength + 1, 2 * strength + 1), Point(strength, strength));

	if (mode == 0)
		erode(image, new_image, element);
	else
		dilate(image, new_image, element);
	imshow("Image", new_image);
}

void light(int value, void*)
{
	Mat new_image = image*(double)(value) / 100.0;

	imshow("Image", new_image);
}

void resizing(int value, void*)
{
	Mat new_image;
	if (H<1)
		H = 1;
	if (W<1)
		W = 1;

	double ratio_x = image.cols / (double)W;
	double ratio_y = image.rows / (double)H;
	new_image.create(H, W, CV_8UC(3));
	for (int i = 0; i<new_image.rows; i++)
	{
		for (int j = 0; j<new_image.cols; j++)
		{
			Vec3b color = image.at<Vec3b>(Point(j*ratio_x, i*ratio_y));

			new_image.at<Vec3b>(Point(j, i)) = color;
		}
	}

	imshow("Image", new_image);
}

int main(int argc, char** argv)
{
	int response = 0;

	std::cout << "IMG Editor\n" << std::endl;

	if (argc<2)
	{
		while (!image.data)
		{
			char lien[500] = "";
			std::cout << "Image : " << std::endl;
			std::cin >> lien;
			image = imread(lien);
			if (!image.data)
				std::cout << "File not found." << std::endl;
		}
	}
	else
		image = imread(argv[1]);

	int continuer = 1;
	while (continuer)
	{
		int chx = choix();

		if (chx == 0)
			continuer = 0;
		else if (chx>0 && chx<4)
		{
			namedWindow("Image", WINDOW_NORMAL);
			cvResizeWindow("Image", 500, 500);
		}

		if (chx == 1)
		{
			int ini = 0;
			createTrackbar("E/D", "Image", &mode, 1, dilatation_erosion);
			createTrackbar("Value", "Image", &strength, 10, dilatation_erosion);
			dilatation_erosion(ini, 0);
		}
		else if (chx == 2)
		{
			int ini = 0;
			W = image.cols;
			H = image.rows;
			createTrackbar("Width", "Image", &W, 1000, resizing);
			createTrackbar("Height", "Image", &H, 1000, resizing);
			resizing(ini, 0);
		}
		else if (chx == 3)
		{
			int ini = 100;
			createTrackbar("Brightness", "Image", &ini, 500, light);
			light(ini, 0);
		}
		else if (chx == 4)
		{
			std::cout << "not yet available in this release." << std::endl;
		}

		waitKey(0);
	}

	return 0;
}
