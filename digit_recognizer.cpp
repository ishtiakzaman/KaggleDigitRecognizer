/*

Make train and test images from train.csv and test.csv
g++ digit_recognizer.cpp -o dr
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
	if ( strcmp(argv[1], "train") == 0)
	{
		const int validate_size = 8400;
		int file_index_validate = 0, file_index_train = 0;
		int count = 0;	
		int target;
		ofstream ofs_train("train.txt");
		ofstream ofs_validate("validate.txt");
		ifstream ifs("train.csv");
		string line;
		getline(ifs, line); // First line is the header, skip	
		Mat I(28, 28, CV_8UC1);
		while (ifs.eof() == false)
		{		
			getline(ifs, line);		
			istringstream iss(line);		
			string pixel;
			int pixel_value;			
			getline(iss, pixel, ',');
			istringstream iss2(pixel);
			iss2 >> target;
			for (int i = 0; i < 784; ++i)
			{			
				getline(iss, pixel, ',');
				istringstream iss3(pixel);			
				iss3 >> pixel_value;
				I.at<unsigned char>(i/28, i%28) = pixel_value;					
			}		
			int rnd = rand() % 4;
			if (rnd == 0 && file_index_validate < validate_size) // validate
			{			
				ostringstream oss;
				oss << "validate/" << ++file_index_validate << ".jpg";
				ofs_validate << oss.str() << " " << target << endl;
				imwrite(oss.str(), I);	
			}
			else
			{
				ostringstream oss;
				oss << "train/" << ++file_index_train << ".jpg";
				ofs_train << oss.str() << " " << target << endl;
				imwrite(oss.str(), I);	
			}
			++count;
			if (count % 500 == 0)
				cout << count << " files processed." << endl;
		}
		cout << count << " files processed." << endl;
	}
	else
	{		
		int count = 0;	
		int target;		
		ofstream ofs("result.csv");
		ifstream ifs("test.csv");
		string line;
		getline(ifs, line); // First line is the header, skip	
		ofs << "ImageId,Label" << endl;
		Mat I(28, 28, CV_8UC1);
		while (ifs.eof() == false)
		{		
			getline(ifs, line);		
			istringstream iss(line);		
			string pixel;
			int pixel_value;						
			for (int i = 0; i < 784; ++i)
			{			
				getline(iss, pixel, ',');
				istringstream iss2(pixel);			
				iss2 >> pixel_value;
				I.at<unsigned char>(i/28, i%28) = pixel_value;					
			}													
			imwrite("_temp.jpg", I);

			string cmd = "./../caffe/build/examples/cpp_classification/classification.bin ";
			cmd += "deploy.prototxt ";
			cmd += "dr_iter_50000.caffemodel ";
			cmd += "dr_mean.binaryproto ";
			cmd += "map_words.txt ";
			cmd += "_temp.jpg >> _temp.txt";
			system(cmd.c_str());

			ifstream ifs2("_temp.txt");
			getline(ifs2, line); // first line of output, skip
			getline(ifs2, line);
			ofs << ++count << "," << line.substr(line.length()-2, 1) << endl;

			system("rm -f _temp.jpg _temp.txt");
			
			if (count % 100 == 0)
			{
				cout << count << " files processed." << endl;

				//break;
			}
		}
		cout << count << " files processed." << endl;
	}
}