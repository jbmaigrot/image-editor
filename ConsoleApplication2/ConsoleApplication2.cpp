#include <stdafx.h>
#include <opencv2\opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio/videoio.hpp>
//#include <opencv2\stitching\stitcher.hpp>

using namespace cv;

//Global variables
Mat image;
int W = 0;
int H = 0;
int mode = 0;
int strength = 0;

int choix()
{
	int r = -1;
	printf("\n\tFunctions : \n");
	printf("\n\tExit : 0");
	printf("\n\tDilatation / Erosion : 1");
	printf("\n\tResizing : 2");
	printf("\n\tLighten / Darken : 3");
	printf("\n\tPanorama / stitching : 4");

	while (r<0 || r>4)
	{
		printf("\n\n\tChoix : ");
		scanf("%d", &r);
	}
	return r;
}

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

void stitch(int value, void*)
{
	printf("Pas encore disponible ...");
	/* WIP
	
	//Mat new_image;
	Mat image2;

	char lien[500] = "";
	while (!image2.data)
	{
		printf("\t2e image : ");
		scanf("%s", lien);
		image2 = imread(lien);
		if (!image2.data)
			printf("\tFile not found.\n");
	}

	Stitcher stitcher = Stitcher::createDefault();
	Stitcher::Status status = stitcher.stitch(image, image2);

	imshow("Image", image2);*/
}

int main(int argc, char** argv)
{
	int response = 0;

	printf("\n\tIMG Editor\n\n");

	if (argc<2)
	{
		char lien[500] = "";
		while (!image.data)
		{
			printf("\tImage : ");
			scanf("%s", lien);
			image = imread(lien);
			if (!image.data)
				printf("\tFile not found.\n");
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
		else
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
			int ini = 100;
			stitch(ini, 0);
		}

		waitKey(0);
	}

	return 0;
}
