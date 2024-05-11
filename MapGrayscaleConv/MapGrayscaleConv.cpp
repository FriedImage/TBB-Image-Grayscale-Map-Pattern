// Library Includes
#include <iostream>
#include <vector>
#include <filesystem>
#include <tbb/tbb.h>
#include <opencv2/opencv.hpp>

// Namespace shortcuts
using namespace std;
using namespace cv;
using namespace tbb;

// Averaging Transformation function for colored image data
/*
    The function is repeated for every y and x pixel of user-input image

    Parameters consist of the below:
    1. The RGB (colored) image data as an "input" of OpenCV type Mat
    2. The now-grayscaled image data as an "output" of OpenCV type Mat
*/
void grayscaleConversion(Mat& input, Mat& output) {

    // Use of "parallel_for" to implement a Map Pattern technique of splitting work into a 2d array of rows/columns (height/width)
    parallel_for(blocked_range2d<int>(0, input.rows, 0, input.cols),
        [&](const blocked_range2d<int>& r) {
            
            // for-loop of the first dimension (y)
            for (int i = r.rows().begin(); i < r.rows().end(); ++i) {

                // for-loop of the second dimension (x)
                for (int j = r.cols().begin(); j < r.cols().end(); ++j) {

                    // Takes the intensity of current pixel located at the (i, j) coordinate which is of type Vec3b (one-dimensional 3byte array)
                    Vec3b intensity = input.at<Vec3b>(i, j);

                    // Compute gray intensity of the specific pixel by averaging
                    int gray_value = (intensity[0] + intensity[1] + intensity[2]) / 3; // (Sum of RGB intensities / 3)

                    // Save computed grayscaled intensity to the output matrix
                    output.at<uchar>(i, j) = static_cast<uchar>(gray_value); // (output is type cv::Mat)
                }
            }
        }
    );
}

int main() {
    cout << "AUTHOR: George Ntolias" << endl;

    // Load colored image using OpenCV
    /*
        1. Instantiate and enter filename from user input
        2. Determine if user added a file extension
        3. Create image array of the input file
        4. Determine whether user added a supported image file extension
        5. Determine whether user's complete filename exists or not
        6. Read colorImage from user
    */
    string file_name;
    cout << "Please insert the name (including the file extension) of the image file you want to grayscale --> ";
    cin >> file_name;

    // Instantiate extension and check whether there is one from user-input
    string extension;
    size_t extension_dot_pos = file_name.find_last_of('.');
    if (extension_dot_pos != string::npos && extension_dot_pos < file_name.length()) {
        extension = file_name.substr(extension_dot_pos);
    }
    else {
        cerr << "ERROR: File extension not found!" << endl;

        // Sleep for 2 seconds
        this_thread::sleep_for(chrono::seconds(2));
        return 1;
    }

    // Instantiate image array of user-input file
    Mat colorImage;

    // Create vector with some supported image extensions
    vector<string> supported_extensions = { ".jpg", ".png", ".bmp", ".jpeg", ".tiff" };

    // Convert user extension to lower-case (case-insensitive user-input)
    transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    // Iterate over each extension to check if file extension of user-input is supported
    if (file_name.length() >= 4 && file_name.length() < 256) {
        bool is_supported = false;
        for (const string& user_ext : supported_extensions) {
            if (user_ext == extension) {
                is_supported = true;
                break;
            }
        }
        if (is_supported) {
            // Check whether the filename (including the supported extension) exists at all, based on user-input
            if (filesystem::exists(file_name)) {
                colorImage = imread(file_name);
            }
            else {
                cerr << "ERROR: Filename with supported extension not found!" << endl;

                // Sleep for 2 seconds
                this_thread::sleep_for(chrono::seconds(2));
                return 2;
            }
        }
        else {
            cerr << "ERROR: Unsupported image file format! (Supported formats: .jpg, .png, .bmp, .jpeg, .tiff)" << endl;

            // Sleep for 2 seconds
            this_thread::sleep_for(chrono::seconds(2));
            return 3;
        }
    }
    else {
        cerr << "ERROR: Full filename length invalid! (filename must be between 4-255 chars)" << endl;

        // Sleep for 2 seconds
        this_thread::sleep_for(chrono::seconds(2));
        return 4;
    }
    cout << "Input Image (colorImage) read successfully!" << endl;

    // Initialize column and row number based on image height and width
    int width = colorImage.cols;
    int height = colorImage.rows;
    cout << "Image width: " << width << "px, " << "Image height: " << height << "px" << endl;

    // Create a grayscale image of the same size as the color image
    Mat grayscaleImage(colorImage.rows, colorImage.cols, CV_8UC1);

    // Convert input image to grayscale using the map pattern function
    grayscaleConversion(colorImage, grayscaleImage);

    // Success message
    cout << "Grayscale of Image: " << file_name << " done!" << endl;

    // Save grayscaled image with user input for image name
    string result;
    cout << "Please name the result grayscaled image --> ";
    cin >> result;
    if (result.empty() || result.length() > 255) {
        cout << "Invalid name, using default filename 'grayscaled_image' instead" << endl;
        result = "grayscaled_image";
    }
    else {
        result += extension;
    }
    imwrite(result, grayscaleImage); // Move temporary grayscaled input image to a new file (example: [result].jpg)

    // Display the original and grayscale images with full window control features (WINDOW_GUI_NORMAL)
    namedWindow("Input Image", WINDOW_GUI_NORMAL);
    imshow("Input Image", colorImage);
    namedWindow("Grayscaled Image", WINDOW_GUI_NORMAL);
    imshow("Grayscaled Image", grayscaleImage);
    waitKey(0);

    return 0;
}