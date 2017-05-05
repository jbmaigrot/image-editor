#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/stitching.hpp>

#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

//Variables globales
Mat image;
int W = 0;
int H = 0;
int mode = 0;
int strength = 0;
Mat new_image;


void save()
{
	char name[500] = "";
	std::cout << "Name : " << std::endl;
	std::cin >> name;
	std::stringstream ss;
	ss << name << ".jpg";
	imwrite(ss.str(), new_image);
	std::cout << "Saved." << std::endl;
}

int choice()
{
	int r = -1;
	std::cout << "\nFunctions : \n" << std::endl;
	std::cout << "Exit : 0" << std::endl;
	std::cout << "Dilatation / Erosion : 1" << std::endl;
	std::cout << "Resizing : 2" << std::endl;
	std::cout << "Lighten / Darken : 3" << std::endl;
	std::cout << "Panorama / stitching : 4" << std::endl;
	std::cout << "Save : 5" << std::endl;

	while (r<0 || r>5)
	{
		std::cout << "\nChoix : " << std::endl;
		std::cin >> r;
	}
	return r;
}

void dilate_erode(int value, void*)
{
	//Mat new_image;

	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(2 * strength + 1, 2 * strength + 1), Point(strength, strength));

	if (mode == 0)
		erode(image, new_image, element);
	else
		dilate(image, new_image, element);
	imshow("Image", new_image);
}

void light(int value, void*)
{
	new_image = image*(double)(value) / 100.0;

	imshow("Image", new_image);
}

void resize(int value, void*)
{
	//Mat new_image;
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

void stitch(int argc, char** argv)
{
	vector<Mat> imgs;

	for (int i = 1; i < argc; ++i)
	{
		Mat img = imread(argv[i]);
		if (img.empty())
		{
			cout << "Can't read image '" << argv[i] << "'\n";
		}
		imgs.push_back(img);
	}

	//Mat new_image;
	Ptr<Stitcher> stitcher = Stitcher::create(Stitcher::PANORAMA, false);
	Stitcher::Status status = stitcher->stitch(imgs, new_image);

	if (status != Stitcher::OK)
	{
		cout << "Can't stitch images, error code = " << int(status) << endl;
	}
	imshow("Image", new_image);
}

// MAIN
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
	
	new_image = image;

	int continuer = 1;
	while (continuer)
	{
		int chx = choice();

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
			createTrackbar("E/D", "Image", &mode, 1, dilate_erode);
			createTrackbar("Value", "Image", &strength, 10, dilate_erode);
			dilate_erode(ini, 0);
		}
		else if (chx == 2)
		{
			int ini = 0;
			W = image.cols;
			H = image.rows;
			createTrackbar("Width", "Image", &W, 1000, resize);
			createTrackbar("Height", "Image", &H, 1000, resize);
			resize(ini, 0);
		}
		else if (chx == 3)
		{
			int ini = 100;
			createTrackbar("Brightness", "Image", &ini, 500, light);
			light(ini, 0);
		}
		else if (chx == 4)
		{
			if (argc < 3)
			{
				std::cout << "To use the panorama function:\nLaunch the program with all your pictures as arguments,\nor drop the files on the executable." << std::endl;
			}
			else
			{
				stitch(argc, argv);
			}			
		}
		else if (chx == 5)
		{
			save();
		}

		waitKey(0);
	}

	return 0;
}

