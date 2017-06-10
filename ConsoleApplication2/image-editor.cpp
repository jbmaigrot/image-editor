#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/stitching.hpp>
#include "opencv2/objdetect/objdetect.hpp"

#include <iostream>
#include <stdio.h>

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
Mat image_to_save;


void save()
{
	char name[500] = "";
	std::cout << "Name : " << std::endl;
	std::cin >> name;
	std::stringstream ss;
	ss << name << ".jpg";
	imwrite(ss.str(), image_to_save);
	std::cout << "Saved." << std::endl;
	image = image_to_save;
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
	std::cout << "Face detection : 5" << std::endl;
	std::cout << "Save : 6" << std::endl;

	while (r<0 || r>6)
	{
		std::cout << "\nChoix : " << std::endl;
		std::cin >> r;
	}
	return r;
}

void dilate_erode(int value, void*)
{
	Mat new_image;

	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(2 * strength + 1, 2 * strength + 1), Point(strength, strength));

	if (mode == 0)
		erode(image, new_image, element);
	else
		dilate(image, new_image, element);
	imshow("Image", new_image);
	image_to_save=new_image;
}

void light(int value, void*)
{
    Mat new_image;

	new_image = image*(double)(value) / 100.0;

	imshow("Image", new_image);
	image_to_save=new_image;
}

void resize(int value, void*)
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
	image_to_save=new_image;
}

void stitch(int argc, char** argv)
{
    Mat new_image;

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

	Ptr<Stitcher> stitcher = Stitcher::create(Stitcher::PANORAMA, false);
	Stitcher::Status status = stitcher->stitch(imgs, new_image);

	if (status != Stitcher::OK)
	{
		cout << "Can't stitch images, error code = " << int(status) << endl;
	}
	imshow("Image", new_image);
	image_to_save=new_image;
}

void faceDetect()
{
	Mat frame = image;

	CascadeClassifier face_cascade;
	CascadeClassifier eyes_cascade;
	if (!face_cascade.load("haarcascade_frontalface_alt.xml")) { printf("--(!)Error loading1\n"); return; };
	if (!eyes_cascade.load("haarcascade_eye_tree_eyeglasses.xml")) { printf("--(!)Error loading2\n"); return; };

	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

	for (size_t i = 0; i < faces.size(); i++)
	{
		Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
		ellipse(frame, center, Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);

		Mat faceROI = frame_gray(faces[i]);
		std::vector<Rect> eyes;

		//-- In each face, detect eyes
		eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

		for (size_t j = 0; j < eyes.size(); j++)
		{
			Point center(faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5);
			int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
			circle(frame, center, radius, Scalar(255, 0, 0), 4, 8, 0);
		}
	}
	imshow("Image", frame);
	image_to_save = frame;
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

	image_to_save = image;

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
			faceDetect();
		}
		else if (chx == 6)
		{
			save();
		}
		else
		{
			cout << "Bad input :(" << endl;
		}

		waitKey(0);
	}

	return 0;
}
