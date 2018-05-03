// hough.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace cv;

//----------------------------------------------------------------
struct t_linie {
	int r, theta;
};
//----------------------------------------------------------------
struct t_cerc {
	int a, b, r;
};
//----------------------------------------------------------------
class t_hough {
public:
	void get_linii(Mat img_gray, int prag, t_linie* linii, int max_linii, int &num_linii, Mat & acc_grafic);
	void get_cercuri(Mat img_gray, int prag, t_cerc* cercuri, int max_cercuri, int &num_cercuri, Mat & acc_grafic);
};
//----------------------------------------------------------------
void t_hough::get_cercuri(Mat img_gray, int prag, t_cerc* cercuri, int max_cercuri, int &num_cercuri, Mat & acc_grafic)
{
	int ***accumulator;
	int raza_maxima = img_gray.cols < img_gray.rows ? img_gray.cols : img_gray.rows;
	raza_maxima /= 2;
	// alocare memorie acumulator + initializare
	accumulator = new int**[img_gray.cols];
	for (int i = 0; i < img_gray.cols; i++) {
		accumulator[i] = new int*[img_gray.rows];
		for (int j = 0; j < img_gray.rows; j++) {
			accumulator[i][j] = new int[raza_maxima];
			for (int r = 0; r < raza_maxima; r++)
				accumulator[i][j][r] = 0;
		}
	}
	// scanare matrice + construire acumulator
	for (int y = 0; y < img_gray.rows; y++)
		for (int x = 0; x < img_gray.cols; x++) {
			unsigned char culoare = img_gray.at<unsigned char>(y, x);
			if (culoare < 255) {
				for (int r = 10; r < raza_maxima; r++)
					for (int theta = 0; theta < 360; theta++) {
						int a = x - r * cos(theta / 180.0 * 3.14);
						int b = y - r * sin(theta / 180.0 * 3.14);
						if (a >= 0 && a < img_gray.cols && b >=0 && b < img_gray.rows)
						accumulator[a][b][r]++;
					}
			}
		}
	// gasesc cercuri cu voturi peste prag
	num_cercuri = 0;
	for (int y = 0; y < img_gray.rows; y++) {
		for (int x = 0; x < img_gray.cols; x++) {
			for (int r = 10; r < raza_maxima; r++)
				if (accumulator[x][y][r] > prag) {
					cercuri[num_cercuri].a = x;
					cercuri[num_cercuri].b = y;
					cercuri[num_cercuri].r = r;
					num_cercuri++;
					if (num_cercuri == max_cercuri)
						break;
				}
			if (num_cercuri == max_cercuri)
				break;
		}
		if (num_cercuri == max_cercuri)
			break;
	}

	// eliberare memorie
	
	for (int i = 0; i < img_gray.cols; i++) {
		for (int j = 0; j < img_gray.rows; j++) {
			delete[] accumulator[i][j];
		}
		delete[] accumulator[i];
	}
	delete[] accumulator;
}
		
//----------------------------------------------------------------
void t_hough::get_linii(Mat img_gray, int prag, t_linie* linii, int max_linii, int &num_linii, Mat & acc_grafic)
{
	int diagonala = sqrt(img_gray.rows * img_gray.rows + img_gray.cols * img_gray.cols) + 1;

	// alocare memorie
	int **acumulator;
	acumulator = new int*[diagonala];
	for (int i = 0; i < diagonala; i++)
		acumulator[i] = new int[180];
	// initializare
	for (int i = 0; i < diagonala; i++)
		for (int j = 0; j < 180; j++)
			acumulator[i][j] = 0;

	for (int y = 0; y < img_gray.rows; y++)
		for (int x = 0; x < img_gray.cols; x++) {
			unsigned char culoare = img_gray.at<unsigned char>(y, x);
			if (culoare < 255) {
				for (int theta = 0; theta < 180; theta++) {
					double r = (x - img_gray.cols / 2) * cos(theta / 180.0 * 3.14) + (y - img_gray.rows / 2) * sin(theta / 180.0 * 3.14);
					acumulator[(int)r + diagonala / 2][theta] ++;
				}
			}
		}
	// calculare maxim din acumulator
	int max_acc = 0;
	for (int r = 0; r < diagonala; r++)
		for (int theta = 0; theta < 180; theta++)
			if (max_acc < acumulator[r][theta])
				max_acc = acumulator[r][theta];

	// scalare acumulator la [0..255]
	for (int r = 0; r < diagonala; r++)
		for (int theta = 0; theta < 180; theta++)
			acumulator[r][theta] = acumulator[r][theta] / (double)max_acc * 255;

	// creez imagine acumulator
	acc_grafic.create(diagonala, 180, CV_8UC1);
	// punem in imagine acumulator
	for (int r = 0; r < diagonala; r++)
		for (int theta = 0; theta < 180; theta++)
			acc_grafic.at<unsigned char>(r, theta) = acumulator[r][theta];

	// stergere memorie
	for (int i = 0; i < diagonala; i++)
		delete[] acumulator[i];
	delete[] acumulator;

	num_linii = 0;
	for (int r = 0; r < diagonala; r++)
		for (int theta = 0; theta < 180; theta++)
			if (acc_grafic.at<unsigned char>(r, theta) > prag) {
				linii[num_linii].r = r;
				linii[num_linii].theta = theta;
				num_linii++;
				if (num_linii == max_linii)
					return;
			}
}
//----------------------------------------------------------------
int main()
{
	printf("%s\n", CV_VERSION);
	Mat imagine;
	Mat img_gray;

	//imagine = imread("c:/Mihai/uab/hough/data/linii.png");
	//imagine = imread("C:/Mihai/Dropbox/uab/vr/2018/hough/road.jpg");
	imagine = imread("c:/Mihai/uab/hough/data/cercuri.png");

	//resize(imagine, imagine, Size(300, 300));


	cvtColor(imagine, img_gray, COLOR_BGR2GRAY);
	//blur(img_gray, img_gray, Size(3, 3));
	//Canny(img_gray, img_gray, 1, 250);
	//img_gray = 255 - img_gray;

	//namedWindow("canny");
	//imshow("canny", img_gray);

	t_hough hough;
	t_linie linii[100];
	int num_linii = 0;

	Mat acc_grafic;

	hough.get_linii(img_gray, 180, linii, 100, num_linii, acc_grafic);
	// afisare linii

	// imaginea initiala
	cvNamedWindow("fereastra");
	imshow("fereastra", imagine);

	// afisam acumulatorul
	cvNamedWindow("acumulator");
	imshow("acumulator", acc_grafic);

	// afisam liniile care au mai multe voturi decat un prag
	Mat img_linii_si_cercuri(img_gray.rows, img_gray.cols, CV_8UC1, 255);

	int diagonala = sqrt(img_gray.rows * img_gray.rows + img_gray.cols * img_gray.cols) + 1;

	for (int i = 0; i < num_linii; i++){
		Point p1;
		p1.x = 0;
		p1.y = (linii[i].r - diagonala / 2.0 - (p1.x - img_gray.cols / 2) * cos(linii[i].theta / 180.0 * 3.14)) / sin(linii[i].theta / 180.0 * 3.14) + img_gray.rows / 2;

		Point p2;
		p2.x = img_gray.cols - 1;
		p2.y = (linii[i].r - diagonala / 2.0 - (p2.x - img_gray.cols / 2) * cos(linii[i].theta / 180.0 * 3.14)) / sin(linii[i].theta / 180.0 * 3.14) + img_gray.rows / 2;
		line(img_linii_si_cercuri, p1, p2, 0);
	}

	//--------------------------------------------------------------------

	t_cerc cercuri[100];
	int num_cercuri = 0;
	hough.get_cercuri(img_gray, 100, cercuri, 100, num_cercuri, acc_grafic);

	for (int i = 0; i < num_cercuri; i++) {
		circle(img_linii_si_cercuri, Point(cercuri[i].a, cercuri[i].b), cercuri[i].r, 0);
	}
// ---------------------------------------
	// afiseaza cercuri si linii

	cvNamedWindow("linii");
	imshow("linii", img_linii_si_cercuri);

	cvWaitKey(0);

	return 0;
}
