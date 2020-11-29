#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <thread>
#include<algorithm>
#include<System.h>


using std::thread;
using namespace cv;
using namespace std;


void socket_thread(int newsockfd){

	char len_buff[16];	
   	char buff[80000];
	int recieved_bytes;
	while(true){
		bzero(len_buff, 16);
		bzero(buff, 40000);
		recieved_bytes = 0;
		for(int i=0; i<16; i+= recieved_bytes){
			recieved_bytes = recv(newsockfd, len_buff+i, 16-i, 0);
		}

		int len = atoi(len_buff);
		recieved_bytes = 0;
	//	cout<<len<<endl;
		for(int i=0; i<len; i+= recieved_bytes){
			recieved_bytes = recv(newsockfd, buff+i, len-i, 0);
		}

		std::vector<char> data(buff, buff + len);


		double tframe = 0;
		Mat jpegimage = imdecode(data,CV_LOAD_IMAGE_COLOR);

		/*
		cv::Mat Tcw = SLAM.TrackMonocularWithPL(jpegimage,tframe);

       		cv::Mat Rwc = Tcw.rowRange(0,3).colRange(0,3).t();
        	cv::Mat twc = -Rwc*Tcw.rowRange(0,3).col(3);

		*/

		imshow( "Server", jpegimage );
		waitKey(1);
	}

//     SLAM.Shutdown();
     close(newsockfd);

}


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;
     int n;

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = 4966;
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
              error("ERROR on binding");

     listen(sockfd,5);


     clilen = sizeof(cli_addr);

	while(true){
     	newsockfd = accept(sockfd, 
        	         (struct sockaddr *) &cli_addr, 
                	 &clilen);
    	 if (newsockfd < 0) 
    	      error("ERROR on accept");
	

	thread t(socket_thread, newsockfd);
	t.detach();
	
	}

	

     close(sockfd);

     return 0; 
}
