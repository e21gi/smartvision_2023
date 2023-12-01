#include <opencv2/opencv.hpp> //헤더파일 opencv2/opencv.hpp 포함
#include <iostream> //헤더파일 iostream 포함
#include<math.h> //헤더파일 math.h 포함
#include<cmath> //헤더파일 cmath 포함
#include<ctime> //헤더파일 ctime 포함
using namespace cv; //cv 생략
using namespace cv::dnn; //cv::dnn 생략
using namespace std; //std 생략
Mat frame; //Mat 객체 선언
Net net = readNet("frozen_model.pb"); //학습된 모델파일을 불러옴
double d_abrad = 0; //각도를 저장할 변수 선언
double d_distance = 0.0; //두점 길이를 저장할 변수 선언
class Button { //클래스 선언
private: //접근 지정자 private
	Point pos; //위치를 저장할 변수 선언
	string text; //문자열을 저장할 변수 선언
	int b=0; //bgr색공간 중 b를 저장할 변수 선언 
	int g=0; //bgr색공간 중 g를 저장할 변수 선언
	int r=0; //bgr색공간 중 r를 저장할 변수 선언
public:
	void drawButton(Point pos, string text,int b=0,int g=0,int r=0) //버튼을 그려주는 멤버 함수 선언
	{
		this->pos = pos, this->text = text, this->b = b, this->g = g, this->r = r; //멤버함수 변수의 값들을 this 포인터를 이용 
		rectangle(frame, Rect(pos, Size(50, 50)), Scalar(b, g, r), -1); //버튼의 사각형을 그려줌
		putText(frame, text, Point(pos.x + 15, pos.y + 25), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0)); //버튼의 사각형안에 해당 단어를 출력 
	}
};
void LWG_Button(string str,Point button_location); //버튼을 만들어주고, 각과 길이에 따라 입력 판단을 하는 함수 선언부
string LWG_train(Mat frame); //학습을 위한 함수 선언부
Point LWG_Mouse_Hand(Mat frame); //손 인식 함수 선언부
int main() //메인 함수
{
	clock_t start, finish;
	double time=0;
	double fps = 0;
	VideoCapture cap(0); //비디오를 불러옴
	if (!cap.isOpened()) { cerr << "cam load failed" << endl; return -1; } //비디오가 없다면 에러메세지 출력후 종료

	if (net.empty()) { cerr << "Network load failed!" << endl; return -1; } //읽어드린 학습 모델이 없다면 에러메세지 출력후 종료

	while (1) //무한반복
	{
		start = clock();
		cap >> frame; //비디오를 Mat객체에 저장해 사용

		string text = LWG_train(frame); //학습 함수를 사용

		Point mouse = LWG_Mouse_Hand(frame); //마우스 포인터를 

		LWG_Button(text, mouse); //버튼을 만들어주고, 각과 길이에 따라 입력 판단

		imshow("frame", frame); //이미지를 카메라 상에 띄워줌
		finish = clock();
		time = (double)(finish - start);
		fps = 1000 / time;
		cout << fps << endl;
		if (waitKey(1) == 27) break; //ESC를 누르면 종료
	}
}
void LWG_Button(string str, Point button_location) //버튼을 만들어주고, 각과 길이에 따라 입력 판단을 하는 함수 선언
{
	string string_text[26] = { "a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z" }; //영어 소문자를 저장할 string 배열 선언
	string string_num[10] = { "0","2","3","4","5","6","7","8","9" }; //숫자를 저장할  string 배열 선언
	Button mybutton; //Button 클래스 객체 선언
	int count = 0; //int형 변수 선언(string_text, string_num에 사용할 index)
	vector<Point> thispos; //현재 위치를 저장할 vector point 선언
	string str_last; //입력되는 문자를 저장할 string 변수 선언
	rectangle(frame, Rect(100, 400, 50, 50), Scalar(255, 255, 0), -1); //출력창 만들기
	if (str == "hand1") //손모양이 1이면 
	{	
		for (int i = 1; i <= 3; i++) //3번 반복
		{
			if (i == 3) //만약i가 3이면
			{
				for (int j = 1; j <= 5; j++) //5번 반복
				{
					mybutton.drawButton(Point(60 * j + 5, 60 * i + 10), string_text[count], 255, 255, 0); 
					//영소문자 버튼을 만들고 해당 버튼이 무슨버튼인지 문자 출력
					thispos.push_back(Point(60 * j + 5, 60 * i + 10)); 
					//각각 키보드의 위치를 push_back을 이용해 저장
					for (int h = 0; h < thispos.size(); h++) 
					//키보드의 위치 만큼 반복
					{
						if ((thispos[h].x <= button_location.x && button_location.x <= thispos[h].x + 50) && (thispos[h].y <= button_location.y 
							&& button_location.y <= thispos[h].y + 50)) 
							//손 끝점과 해당 키보드 자판의 위치가 같으면 실행
							if ((d_abrad > -1.1 && d_abrad < -0.7) && (d_distance > 120.0 && d_distance < 140.0)) 
							//각도와 거리가 값과 모두 일치하면 실행
							{
								mybutton.drawButton(Point(thispos[h].x, thispos[h].y), string_text[h], 0, 255, 255); 
								//해당 버튼의 색을 다르게 출력하고 입력으로 판단
								str_last = string_text[h]; 
								//현재 입력되고 있는 자판의 문자를 가져옴
								putText(frame, str_last, Point(100 + 15, 400 + 25), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0)); 
								//그 문자를 출력창에 표시
							}
							else //아니면
								mybutton.drawButton(Point(thispos[h].x, thispos[h].y), string_text[h], 255, 128, 0); 
								//가리키고 있는 버튼의 색을 다르게 출력
					}
					count++; //count 1증가
				}
			}
			else //아니면
			{
				for (int j = 1; j <= 10; j++) //10번 반복
				{
					mybutton.drawButton(Point(60 * j + 5, 60 * i + 10), string_text[count], 255, 255, 0); 
					//영소문자 버튼을 만들고 해당 버튼이 무슨버튼인지 문자 출력
					thispos.push_back(Point(60 * j + 5, 60 * i + 10)); 
					//각각 키보드의 위치를 push_back을 이용해 저장
					for (int h = 0; h < thispos.size(); h++) 
					//키보드의 위치의 수 만큼 반복
					{
						if ((thispos[h].x <= button_location.x && button_location.x <= thispos[h].x + 50) && (thispos[h].y <= button_location.y 
							&& button_location.y <= thispos[h].y + 50)) 
							//손 끝점과 해당 키보드 자판의 위치가 같으면 실행
							if ((d_abrad > -1.1 && d_abrad < -0.7) && (d_distance > 120.0 && d_distance < 140.0)) 
							//각도와 거리가 값과 모두 일치하면 실행
							{
								mybutton.drawButton(Point(thispos[h].x, thispos[h].y), string_text[h], 0, 255, 255); 
								//해당 버튼의 색을 다르게 출력하고 입력으로 판단
								str_last = string_text[h]; 
								//현재 입력되고 있는 자판의 문자를 가져옴
								putText(frame, str_last, Point(100 + 15, 400 + 25), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0)); 
								//그 문자를 출력창에 표시
							}
							else //아니면
								mybutton.drawButton(Point(thispos[h].x, thispos[h].y), string_text[h], 255, 128, 0); 
								//가리키고 있는 버튼의 색을 다르게 출력
					}
					count++; //count 1증가
				}
			}
		}
	}
	else if (str == "hand2") //손모양이 2이면
	{
		for (int i = 1; i <= 3; i++) //3번 반복
		{
			for (int j = 1; j <= 3; j++) //3번 반복
			{
				mybutton.drawButton(Point(100 * j + 5, 60 * i + 10), string_num[count], 255, 255, 0); 
				//숫자 버튼을 만들고 해당 버튼이 무슨버튼인지 문자 출력
				thispos.push_back(Point(100 * j + 5, 60 * i + 10)); 
				//각각 키보드의 위치를 push_back을 이용해 저장
				for (int h = 0; h < thispos.size(); h++) 
				//키보드의 위치의 수 만큼 반복
				{
					if ((thispos[h].x <= button_location.x && button_location.x <= thispos[h].x + 50) && (thispos[h].y <= button_location.y 
						&& button_location.y <= thispos[h].y + 50)) 
						//손 끝점과 해당 키보드 자판의 위치가 같으면 실행
						if ((d_abrad > -0.7 && d_abrad < -0.35) && (d_distance > 150.0 && d_distance < 200.0)) 
						//각도와 거리가 값과 모두 일치하면 실행
						{
							mybutton.drawButton(Point(thispos[h].x, thispos[h].y), string_num[h], 0, 255, 255); 
							//해당 버튼의 색을 다르게 출력하고 입력으로 판단
							str_last = string_num[h]; 
							//현재 입력되고 있는 자판의 문자를 가져옴
							putText(frame, str_last, Point(100 + 15, 400 + 25), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0)); 
							//그 문자를 출력창에 표시
						}
						else
							mybutton.drawButton(Point(thispos[h].x, thispos[h].y), string_num[h], 255, 128, 0); 
							//가리키고 있는 버튼의 색을 다르게 출력
				}
				count++; //count 1증가
			}
		}
	}
}
string LWG_train(Mat frame) //학습을 위한 함수 구현부
{
	vector<String> classNames = { "none","hand1","hand2" }; //학습의 라벨을 지정

	Mat inputBlob = blobFromImage(frame, 1.0 / 127.5, Size(244, 244), Scalar(-1, -1, -1)); //블롭사용
	net.setInput(inputBlob); //네트워크를 입력으로 설정
	Mat prob = net.forward(); //네트워크를 실행
	double maxVal; //최대값을 저장할 변수 선언
	Point maxLoc; //최대값의 위치를 저장할 변수 선언
	minMaxLoc(prob, NULL, &maxVal, NULL, &maxLoc); //최대값과 최대값의 위치를 구해줌
	String str = classNames[maxLoc.x] + format("(%4.2lf%%)", maxVal * 100); //현재 인식되고 있는 매개체와 확률을 구해줌
	cout << str << endl; //str 출력 
	return classNames[maxLoc.x]; //인식되고 있는 매개체 리턴
}
Point LWG_Mouse_Hand(Mat frame) // 손인식 함수 구현부
{
	Mat hsv; //색공간 변환을 위한 mat객체 선언
	Mat hsv_res; //inrange 변환후 저장할 mat객체 선언
	Mat mopol; // 모폴로지 행렬을 저정할 mat객체 선언
	Mat mopol_res; //모폴로지 후에 저장할 mat 객체 선언

	vector<vector<Point>> contours; //검출된 외각선 정보
	vector<Vec4i> hierarchy; //외각선 계층정보
	vector<Point> approx; //approxPolyDP 한 후에 저장할 vector point 선언
	vector<Point> hull; //convexhull을 한후 저정할 vector point 선언

	Point mouse_pointer; //리턴시킬 point 변수 선언

	double a_x = 0, a_y = 0, b_x = 0, b_y = 0, c_x = 0, c_y = 0; //x축 y축을 저장할 변수 선언
	double area = 0, max_area = 17000; //영역을 저장할 변수선언, 설정해 저장된 최대 영역 변수 선언

	cvtColor(frame, hsv, COLOR_BGR2HSV); //hsv공간 사용

	inRange(hsv, Scalar(0, 30, 0), Scalar(15, 255, 255), hsv_res); //살색부분 색출(살색은 흰색, 살색이 아니면 검정색)

	mopol = getStructuringElement(MORPH_ELLIPSE, Size(7, 7)); //지정한 모양과 크기에 해당하는 구조요소행렬을 반환
	morphologyEx(hsv_res, mopol_res, MORPH_CLOSE, mopol); //morph_닫기의 열거형 상수에 따라 모폴로지 연산

	findContours(mopol_res, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE); //흰색부분의 외각선 찾기

	for (int i = 0; i < contours.size(); i++) //검출된 갯수만큼 반복
	{
		area = contourArea(contours[i]); //검출된 영역의 크기

		if (area > max_area && area <30000) //17000보다 크고 30000보다 작으면 실행
		{
			approxPolyDP(contours[i], approx, arcLength(contours[i], true) * 0.03, true); //다각형을 단순화 시킴
			convexHull(approx, hull); //가장 끝점들을 연결해줌
			polylines(frame, hull, -1, Scalar(0, 0, 255)); //convexhull로 검출된 외곽들을 그려줌
			circle(frame, hull[hull.size()-1], 2, Scalar(0, 255, 255), 2); // 마지막 점에 원을 그려줌
			mouse_pointer = hull[hull.size() - 1]; //손끝부분을 저장함
			a_x = hull[0].x; //x축 저장
			a_y = hull[0].y; //y축 저장
			b_x = hull[hull.size() - 2].x; //x축 저장
			b_y = hull[hull.size() - 2].y; //y축 저장
			c_x = hull[hull.size() - 1].x; //x축 저장
			c_y = hull[hull.size() - 1].y; //y축 저장
			if (!((c_x == a_x && c_y == a_y) || (c_x == b_x && c_y == b_y))) //3점이 같지 않으면 실행
				d_abrad = atan((b_y - c_y) / (b_x - c_x) - atan(a_y - c_y) / (a_x - c_x)); //3점의 각을 구해줌

			d_distance = sqrt(pow(a_x - b_x, 2) + pow(a_y - b_y, 2)); //두점의 거리를 구해줌
		}
	}
	return mouse_pointer; //손 끝 위치점을 리턴
}