#include <opencv2/opencv.hpp> //������� opencv2/opencv.hpp ����
#include <iostream> //������� iostream ����
#include<math.h> //������� math.h ����
#include<cmath> //������� cmath ����
#include<ctime> //������� ctime ����
using namespace cv; //cv ����
using namespace cv::dnn; //cv::dnn ����
using namespace std; //std ����
Mat frame; //Mat ��ü ����
Net net = readNet("frozen_model.pb"); //�н��� �������� �ҷ���
double d_abrad = 0; //������ ������ ���� ����
double d_distance = 0.0; //���� ���̸� ������ ���� ����
class Button { //Ŭ���� ����
private: //���� ������ private
	Point pos; //��ġ�� ������ ���� ����
	string text; //���ڿ��� ������ ���� ����
	int b=0; //bgr������ �� b�� ������ ���� ���� 
	int g=0; //bgr������ �� g�� ������ ���� ����
	int r=0; //bgr������ �� r�� ������ ���� ����
public:
	void drawButton(Point pos, string text,int b=0,int g=0,int r=0) //��ư�� �׷��ִ� ��� �Լ� ����
	{
		this->pos = pos, this->text = text, this->b = b, this->g = g, this->r = r; //����Լ� ������ ������ this �����͸� �̿� 
		rectangle(frame, Rect(pos, Size(50, 50)), Scalar(b, g, r), -1); //��ư�� �簢���� �׷���
		putText(frame, text, Point(pos.x + 15, pos.y + 25), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0)); //��ư�� �簢���ȿ� �ش� �ܾ ��� 
	}
};
void LWG_Button(string str,Point button_location); //��ư�� ������ְ�, ���� ���̿� ���� �Է� �Ǵ��� �ϴ� �Լ� �����
string LWG_train(Mat frame); //�н��� ���� �Լ� �����
Point LWG_Mouse_Hand(Mat frame); //�� �ν� �Լ� �����
int main() //���� �Լ�
{
	clock_t start, finish;
	double time=0;
	double fps = 0;
	VideoCapture cap(0); //������ �ҷ���
	if (!cap.isOpened()) { cerr << "cam load failed" << endl; return -1; } //������ ���ٸ� �����޼��� ����� ����

	if (net.empty()) { cerr << "Network load failed!" << endl; return -1; } //�о�帰 �н� ���� ���ٸ� �����޼��� ����� ����

	while (1) //���ѹݺ�
	{
		start = clock();
		cap >> frame; //������ Mat��ü�� ������ ���

		string text = LWG_train(frame); //�н� �Լ��� ���

		Point mouse = LWG_Mouse_Hand(frame); //���콺 �����͸� 

		LWG_Button(text, mouse); //��ư�� ������ְ�, ���� ���̿� ���� �Է� �Ǵ�

		imshow("frame", frame); //�̹����� ī�޶� �� �����
		finish = clock();
		time = (double)(finish - start);
		fps = 1000 / time;
		cout << fps << endl;
		if (waitKey(1) == 27) break; //ESC�� ������ ����
	}
}
void LWG_Button(string str, Point button_location) //��ư�� ������ְ�, ���� ���̿� ���� �Է� �Ǵ��� �ϴ� �Լ� ����
{
	string string_text[26] = { "a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z" }; //���� �ҹ��ڸ� ������ string �迭 ����
	string string_num[10] = { "0","2","3","4","5","6","7","8","9" }; //���ڸ� ������  string �迭 ����
	Button mybutton; //Button Ŭ���� ��ü ����
	int count = 0; //int�� ���� ����(string_text, string_num�� ����� index)
	vector<Point> thispos; //���� ��ġ�� ������ vector point ����
	string str_last; //�ԷµǴ� ���ڸ� ������ string ���� ����
	rectangle(frame, Rect(100, 400, 50, 50), Scalar(255, 255, 0), -1); //���â �����
	if (str == "hand1") //�ո���� 1�̸� 
	{	
		for (int i = 1; i <= 3; i++) //3�� �ݺ�
		{
			if (i == 3) //����i�� 3�̸�
			{
				for (int j = 1; j <= 5; j++) //5�� �ݺ�
				{
					mybutton.drawButton(Point(60 * j + 5, 60 * i + 10), string_text[count], 255, 255, 0); 
					//���ҹ��� ��ư�� ����� �ش� ��ư�� ������ư���� ���� ���
					thispos.push_back(Point(60 * j + 5, 60 * i + 10)); 
					//���� Ű������ ��ġ�� push_back�� �̿��� ����
					for (int h = 0; h < thispos.size(); h++) 
					//Ű������ ��ġ ��ŭ �ݺ�
					{
						if ((thispos[h].x <= button_location.x && button_location.x <= thispos[h].x + 50) && (thispos[h].y <= button_location.y 
							&& button_location.y <= thispos[h].y + 50)) 
							//�� ������ �ش� Ű���� ������ ��ġ�� ������ ����
							if ((d_abrad > -1.1 && d_abrad < -0.7) && (d_distance > 120.0 && d_distance < 140.0)) 
							//������ �Ÿ��� ���� ��� ��ġ�ϸ� ����
							{
								mybutton.drawButton(Point(thispos[h].x, thispos[h].y), string_text[h], 0, 255, 255); 
								//�ش� ��ư�� ���� �ٸ��� ����ϰ� �Է����� �Ǵ�
								str_last = string_text[h]; 
								//���� �Էµǰ� �ִ� ������ ���ڸ� ������
								putText(frame, str_last, Point(100 + 15, 400 + 25), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0)); 
								//�� ���ڸ� ���â�� ǥ��
							}
							else //�ƴϸ�
								mybutton.drawButton(Point(thispos[h].x, thispos[h].y), string_text[h], 255, 128, 0); 
								//����Ű�� �ִ� ��ư�� ���� �ٸ��� ���
					}
					count++; //count 1����
				}
			}
			else //�ƴϸ�
			{
				for (int j = 1; j <= 10; j++) //10�� �ݺ�
				{
					mybutton.drawButton(Point(60 * j + 5, 60 * i + 10), string_text[count], 255, 255, 0); 
					//���ҹ��� ��ư�� ����� �ش� ��ư�� ������ư���� ���� ���
					thispos.push_back(Point(60 * j + 5, 60 * i + 10)); 
					//���� Ű������ ��ġ�� push_back�� �̿��� ����
					for (int h = 0; h < thispos.size(); h++) 
					//Ű������ ��ġ�� �� ��ŭ �ݺ�
					{
						if ((thispos[h].x <= button_location.x && button_location.x <= thispos[h].x + 50) && (thispos[h].y <= button_location.y 
							&& button_location.y <= thispos[h].y + 50)) 
							//�� ������ �ش� Ű���� ������ ��ġ�� ������ ����
							if ((d_abrad > -1.1 && d_abrad < -0.7) && (d_distance > 120.0 && d_distance < 140.0)) 
							//������ �Ÿ��� ���� ��� ��ġ�ϸ� ����
							{
								mybutton.drawButton(Point(thispos[h].x, thispos[h].y), string_text[h], 0, 255, 255); 
								//�ش� ��ư�� ���� �ٸ��� ����ϰ� �Է����� �Ǵ�
								str_last = string_text[h]; 
								//���� �Էµǰ� �ִ� ������ ���ڸ� ������
								putText(frame, str_last, Point(100 + 15, 400 + 25), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0)); 
								//�� ���ڸ� ���â�� ǥ��
							}
							else //�ƴϸ�
								mybutton.drawButton(Point(thispos[h].x, thispos[h].y), string_text[h], 255, 128, 0); 
								//����Ű�� �ִ� ��ư�� ���� �ٸ��� ���
					}
					count++; //count 1����
				}
			}
		}
	}
	else if (str == "hand2") //�ո���� 2�̸�
	{
		for (int i = 1; i <= 3; i++) //3�� �ݺ�
		{
			for (int j = 1; j <= 3; j++) //3�� �ݺ�
			{
				mybutton.drawButton(Point(100 * j + 5, 60 * i + 10), string_num[count], 255, 255, 0); 
				//���� ��ư�� ����� �ش� ��ư�� ������ư���� ���� ���
				thispos.push_back(Point(100 * j + 5, 60 * i + 10)); 
				//���� Ű������ ��ġ�� push_back�� �̿��� ����
				for (int h = 0; h < thispos.size(); h++) 
				//Ű������ ��ġ�� �� ��ŭ �ݺ�
				{
					if ((thispos[h].x <= button_location.x && button_location.x <= thispos[h].x + 50) && (thispos[h].y <= button_location.y 
						&& button_location.y <= thispos[h].y + 50)) 
						//�� ������ �ش� Ű���� ������ ��ġ�� ������ ����
						if ((d_abrad > -0.7 && d_abrad < -0.35) && (d_distance > 150.0 && d_distance < 200.0)) 
						//������ �Ÿ��� ���� ��� ��ġ�ϸ� ����
						{
							mybutton.drawButton(Point(thispos[h].x, thispos[h].y), string_num[h], 0, 255, 255); 
							//�ش� ��ư�� ���� �ٸ��� ����ϰ� �Է����� �Ǵ�
							str_last = string_num[h]; 
							//���� �Էµǰ� �ִ� ������ ���ڸ� ������
							putText(frame, str_last, Point(100 + 15, 400 + 25), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0)); 
							//�� ���ڸ� ���â�� ǥ��
						}
						else
							mybutton.drawButton(Point(thispos[h].x, thispos[h].y), string_num[h], 255, 128, 0); 
							//����Ű�� �ִ� ��ư�� ���� �ٸ��� ���
				}
				count++; //count 1����
			}
		}
	}
}
string LWG_train(Mat frame) //�н��� ���� �Լ� ������
{
	vector<String> classNames = { "none","hand1","hand2" }; //�н��� ���� ����

	Mat inputBlob = blobFromImage(frame, 1.0 / 127.5, Size(244, 244), Scalar(-1, -1, -1)); //��ӻ��
	net.setInput(inputBlob); //��Ʈ��ũ�� �Է����� ����
	Mat prob = net.forward(); //��Ʈ��ũ�� ����
	double maxVal; //�ִ밪�� ������ ���� ����
	Point maxLoc; //�ִ밪�� ��ġ�� ������ ���� ����
	minMaxLoc(prob, NULL, &maxVal, NULL, &maxLoc); //�ִ밪�� �ִ밪�� ��ġ�� ������
	String str = classNames[maxLoc.x] + format("(%4.2lf%%)", maxVal * 100); //���� �νĵǰ� �ִ� �Ű�ü�� Ȯ���� ������
	cout << str << endl; //str ��� 
	return classNames[maxLoc.x]; //�νĵǰ� �ִ� �Ű�ü ����
}
Point LWG_Mouse_Hand(Mat frame) // ���ν� �Լ� ������
{
	Mat hsv; //������ ��ȯ�� ���� mat��ü ����
	Mat hsv_res; //inrange ��ȯ�� ������ mat��ü ����
	Mat mopol; // �������� ����� ������ mat��ü ����
	Mat mopol_res; //�������� �Ŀ� ������ mat ��ü ����

	vector<vector<Point>> contours; //����� �ܰ��� ����
	vector<Vec4i> hierarchy; //�ܰ��� ��������
	vector<Point> approx; //approxPolyDP �� �Ŀ� ������ vector point ����
	vector<Point> hull; //convexhull�� ���� ������ vector point ����

	Point mouse_pointer; //���Ͻ�ų point ���� ����

	double a_x = 0, a_y = 0, b_x = 0, b_y = 0, c_x = 0, c_y = 0; //x�� y���� ������ ���� ����
	double area = 0, max_area = 17000; //������ ������ ��������, ������ ����� �ִ� ���� ���� ����

	cvtColor(frame, hsv, COLOR_BGR2HSV); //hsv���� ���

	inRange(hsv, Scalar(0, 30, 0), Scalar(15, 255, 255), hsv_res); //����κ� ����(����� ���, ����� �ƴϸ� ������)

	mopol = getStructuringElement(MORPH_ELLIPSE, Size(7, 7)); //������ ���� ũ�⿡ �ش��ϴ� ������������ ��ȯ
	morphologyEx(hsv_res, mopol_res, MORPH_CLOSE, mopol); //morph_�ݱ��� ������ ����� ���� �������� ����

	findContours(mopol_res, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE); //����κ��� �ܰ��� ã��

	for (int i = 0; i < contours.size(); i++) //����� ������ŭ �ݺ�
	{
		area = contourArea(contours[i]); //����� ������ ũ��

		if (area > max_area && area <30000) //17000���� ũ�� 30000���� ������ ����
		{
			approxPolyDP(contours[i], approx, arcLength(contours[i], true) * 0.03, true); //�ٰ����� �ܼ�ȭ ��Ŵ
			convexHull(approx, hull); //���� �������� ��������
			polylines(frame, hull, -1, Scalar(0, 0, 255)); //convexhull�� ����� �ܰ����� �׷���
			circle(frame, hull[hull.size()-1], 2, Scalar(0, 255, 255), 2); // ������ ���� ���� �׷���
			mouse_pointer = hull[hull.size() - 1]; //�ճ��κ��� ������
			a_x = hull[0].x; //x�� ����
			a_y = hull[0].y; //y�� ����
			b_x = hull[hull.size() - 2].x; //x�� ����
			b_y = hull[hull.size() - 2].y; //y�� ����
			c_x = hull[hull.size() - 1].x; //x�� ����
			c_y = hull[hull.size() - 1].y; //y�� ����
			if (!((c_x == a_x && c_y == a_y) || (c_x == b_x && c_y == b_y))) //3���� ���� ������ ����
				d_abrad = atan((b_y - c_y) / (b_x - c_x) - atan(a_y - c_y) / (a_x - c_x)); //3���� ���� ������

			d_distance = sqrt(pow(a_x - b_x, 2) + pow(a_y - b_y, 2)); //������ �Ÿ��� ������
		}
	}
	return mouse_pointer; //�� �� ��ġ���� ����
}