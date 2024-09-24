#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace std;
using namespace cv;

void energy_func(Mat& image, int height ,int width , Mat& energy){
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
    // cout<<image.at<Vec3b>(i, width-1)<<endl;
    // cout<<image.at<Vec3b>(i, j+1)<<endl;
    // cout<<image.at<Vec3b>(height-1, 0)<<endl;
    // cout<<image.at<Vec3b>(i+1, j)<<endl;
            Vec3b left,right,up,down;
            if(j==0){
                left = image.at<Vec3b>(i, width-1);
            }
            else{
                left = image.at<Vec3b>(i, j-1);
            }
            if(j==width-1){
                right = image.at<Vec3b>(i, 0);
            }
            else{
                right = image.at<Vec3b>(i, j+1);
            }
            if(i==0){
                up = image.at<Vec3b>(height-1, j);
            }
            else{
                up = image.at<Vec3b>(i-1, j);
            }
            if(i==height-1){
                down = image.at<Vec3b>(0, j);
            }
            else{
                down = image.at<Vec3b>(i+1, j);
            }

            int b_x,r_x,g_x,b_y,r_y,g_y;
            b_x = right[0] - left[0];
            g_x = right[1] - left[1];
            r_x = right[2] - left[2];
            // cout<<b_x<<endl<<g_x<<endl<<r_x<<endl;
            int del_x = b_x*b_x + g_x*g_x + r_x*r_x;

            b_y = down[0] - up[0];
            g_y = down[1] - up[1];
            r_y = down[2] - up[2];
            // cout<<b_y<<endl<<g_y<<endl<<r_y<<endl;
            int del_y = b_y*b_y + g_y*g_y + r_y*r_y;

            // cout<<del_x<<endl<<del_y<<endl;

            energy.at<uchar>(i, j) = sqrt(del_x+del_y);
            // cout<<energy.at<uchar>(i, j)<<"  ";
        }
    }
}

void find_vertical_seam(Mat &energy,int* lowest_seam,int &height,int &width){

    int dp[height][width];
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            dp[i][j] = energy.at<uchar>(i,j);
        }
    }

    for(int i=1;i<height;i++){
        for(int j=0;j<width;j++){
            if(j==0){
                dp[i][j] += min(dp[i-1][j],dp[i-1][j+1]);
            }
            else if(j==width-1){
                dp[i][j] += min(dp[i-1][j],dp[i-1][j-1]);
            }
            else{
                dp[i][j] += min(min(dp[i-1][j],dp[i-1][j-1]),dp[i-1][j+1]);
            }
        }
    }

    int min_row = 0;
    for(int i=0;i<width;i++){
        if(dp[height-1][i]<dp[height-1][min_row]){
            min_row = i;
        }
    }
    lowest_seam[height-1] = min_row;


    for(int i = height-2; i >= 0; i--){
        int current_col = lowest_seam[i+1];

        if(current_col == 0){
            if(dp[i][current_col] <= dp[i][current_col+1]){
                lowest_seam[i] = current_col;
            } else {
                lowest_seam[i] = current_col + 1;
            }
        }
        else if(current_col == width-1){
            if(dp[i][current_col] <= dp[i][current_col-1]){
                lowest_seam[i] = current_col;
            } else {
                lowest_seam[i] = current_col - 1;
            }
        }
        else{
            if(dp[i][current_col-1] <= dp[i][current_col] && dp[i][current_col-1] <= dp[i][current_col+1]){
                lowest_seam[i] = current_col - 1;
            }
            else if(dp[i][current_col] <= dp[i][current_col-1] && dp[i][current_col] <= dp[i][current_col+1]){
                lowest_seam[i] = current_col;
            }
            else{
                lowest_seam[i] = current_col + 1;
            }
        }
    }

}

void display_vertical_seam(Mat &energy,Mat &image,int* lowest_seam,int &height,int &width){
    for(int i=0;i<height;i++){
        image.at<Vec3b>(i , lowest_seam[i])[0] = 0;
        image.at<Vec3b>(i , lowest_seam[i])[1] = 0;
        image.at<Vec3b>(i , lowest_seam[i])[2] = 255;
    }
}

void remove_vertical_seam(Mat &energy,Mat &image,int* lowest_seam,int &height,int &width){
    // cout<<width<<" ";
    Mat new_image(height, width - 1, image.type());
    Mat new_energy(height, width - 1, energy.type());

    for(int i=0;i<height;i++){
        int y=0;
        for(int j=0;j<width;j++){
            if(j == lowest_seam[i]){
                continue;
            }
            new_image.at<Vec3b>(i,y) = image.at<Vec3b>(i,j);
            new_energy.at<uchar>(i,y) = energy.at<uchar>(i,j);
            y++;
        }
    }

    image = new_image;
    energy = new_energy;
}

void find_horizontal_seam(Mat &energy,int* lowest_seam,int &height,int &width){

    int dp[height][width];
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            dp[i][j] = energy.at<uchar>(i,j);
        }
    }

    for(int j=1;j<width;j++){
        for(int i=0;i<height;i++){
            if(i==0){
                dp[i][j] += min(dp[i][j-1],dp[i+1][j-1]);
            }
            else if(i==height-1){
                dp[i][j] += min(dp[i][j-1],dp[i-1][j-1]);
            }
            else{
                dp[i][j] += min(min(dp[i-1][j-1],dp[i][j-1]),dp[i+1][j-1]);
            }
        }
    }

    int min_col = 0;
    for(int i=0;i<height;i++){
        if(dp[i][width-1]<dp[min_col][width-1]){
            min_col = i;
        }
    }
    lowest_seam[width-1] = min_col;


    for(int j = width-2; j >= 0; j--){
        int current_row = lowest_seam[j+1];

        if(current_row == 0){
            if(dp[current_row][j] <= dp[current_row+1][j]){
                lowest_seam[j] = current_row;
            } else {
                lowest_seam[j] = current_row + 1;
            }
        }
        else if(current_row == height-1){
            if(dp[current_row][j] <= dp[current_row-1][j]){
                lowest_seam[j] = current_row;
            } else {
                lowest_seam[j] = current_row - 1;
            }
        }
        else{
            if(dp[current_row-1][j] <= dp[current_row][j] && dp[current_row-1][j] <= dp[current_row+1][j]){
                lowest_seam[j] = current_row - 1;
            }
            else if(dp[current_row][j] <= dp[current_row-1][j] && dp[current_row][j] <= dp[current_row+1][j]){
                lowest_seam[j] = current_row;
            }
            else{
                lowest_seam[j] = current_row + 1;
            }
        }
    }

}

void display_horizontal_seam(Mat &energy,Mat &image,int* lowest_seam,int &height,int &width){
    for(int i=0;i<width;i++){
        image.at<Vec3b>( lowest_seam[i] , i)[0] = 0;
        image.at<Vec3b>( lowest_seam[i] , i)[1] = 0;
        image.at<Vec3b>( lowest_seam[i] , i)[2] = 255;
    }
}

void remove_horizontal_seam(Mat &energy,Mat &image,int* lowest_seam,int &height,int &width){
    // cout<<height<<" ";
    Mat new_image(height - 1, width, image.type());
    Mat new_energy(height - 1, width, energy.type());

    for(int j=0;j<width;j++){
        int x=0;
        for(int i=0;i<height;i++){
            if(i == lowest_seam[j]){
                continue;
            }
            new_image.at<Vec3b>(x,j) = image.at<Vec3b>(i,j);
            new_energy.at<uchar>(x,j) = energy.at<uchar>(i,j);
            x++;
        }
    }

    image = new_image;
    energy = new_energy;
}

int main(){
    Mat image;

    string img_path;
    // abs or relative
    cout<<"Enter the Image path : ";
    cin>>img_path;
    
    image = imread(img_path, 1);
    // cout<<image.cols<<" "<<image.rows<<" "<<image.channels()<<endl;
    if(image.empty()){
        cout<<"Image doesnot exists";
        exit(0);
    }

    int width = image.cols;
    int height = image.rows;
    cout<<"\nheight of input image : "<<height<<"\nWidth of input image : "<<width<<endl;

    int new_height,new_width;
    cout<<"Enter resized height and width : ";
    cin>>new_height>>new_width;
    if(new_height>height || new_width>width || new_height<=0 || new_width<=0){
        cout<<"Invalid height or width\n";
        exit(0);
    }

    Mat energy(height, width, CV_8UC1);

    energy_func(image,height,width,energy);
    int original_width = width;
    int original_height = height;

    while(new_width!=width || new_height!=height){
        if(new_width!=width){
            int lowest_seam_vertical[height];
            find_vertical_seam(energy,lowest_seam_vertical,height,width);

            display_vertical_seam(energy,image,lowest_seam_vertical,height,width);

            namedWindow("Display Image",WINDOW_AUTOSIZE);
            resizeWindow("Display Image",original_height, original_width);
            imshow("Display Image", image);
            waitKey(20);

            remove_vertical_seam(energy,image,lowest_seam_vertical,height,width);
            width--;

        }

        if(new_height!=height){
            int lowest_seam_horizontal[width];
            find_horizontal_seam(energy,lowest_seam_horizontal,height,width);

            display_horizontal_seam(energy,image,lowest_seam_horizontal,height,width);

            namedWindow("Display Image",WINDOW_AUTOSIZE);
            resizeWindow("Display Image",original_height, original_width);
            imshow("Display Image", image);
            waitKey(20);

            remove_horizontal_seam(energy,image,lowest_seam_horizontal,height,width);
            height--;

        }

    }

    namedWindow("Display Image",WINDOW_AUTOSIZE);
    resizeWindow("Display Image",height, original_width);
    imshow("Display Image", image);
    waitKey(0);
    cout<<"height of resized image : "<<image.rows<<"\nWidth of resized image : "<<image.cols<<endl;
    return 0;
}