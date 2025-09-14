#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <filesystem>
#include <algorithm>

using namespace std;
using namespace cv;

vector<string> imageFiles;
int fileCount = 0;
string password = "";

// Function to convert string to binary
string toBinary(const string& msg) {
    string binary = "";
    for (char c : msg) {
        string charBinary = "";
        int ascii = (int)c;
        for (int i = 7; i >= 0; i--) {
            charBinary += ((ascii >> i) & 1) ? '1' : '0';
        }
        binary += charBinary;
    }
    return binary;
}

// Function to encode binary data into image
Mat encodingInImage(Mat& img, const string& binaryMsg) {
    int maxBytes = (img.rows * img.cols * 3) / 8;
    
    if (binaryMsg.length() > maxBytes * 8) {
        cout << "The size of the secret message is too much. Either reduce the size or choose a bigger image." << endl;
        return img;
    }
    
    int dataIndex = 0;
    int msgLength = binaryMsg.length();
    
    for (int i = 0; i < img.rows && dataIndex < msgLength; i++) {
        for (int j = 0; j < img.cols && dataIndex < msgLength; j++) {
            Vec3b& pixel = img.at<Vec3b>(i, j);
            
            // Process Blue channel
            if (dataIndex < msgLength) {
                pixel[0] = (pixel[0] & 0xFE) | (binaryMsg[dataIndex] - '0');
                dataIndex++;
            }
            
            // Process Green channel
            if (dataIndex < msgLength) {
                pixel[1] = (pixel[1] & 0xFE) | (binaryMsg[dataIndex] - '0');
                dataIndex++;
            }
            
            // Process Red channel
            if (dataIndex < msgLength) {
                pixel[2] = (pixel[2] & 0xFE) | (binaryMsg[dataIndex] - '0');
                dataIndex++;
            }
        }
    }
    
    return img;
}

// Function to hide data by splitting message
vector<string> hideData(const string& message) {
    int length = message.length();
    int chunkSize = ceil((double)length / fileCount);
    vector<string> messageList;
    
    for (int i = 0; i < fileCount; i++) {
        string part = message.substr(i * chunkSize, chunkSize);
        string messageText;
        
        if (i == 0) {
            messageText = part + "@%#/" + password + "@%#/" + to_string(i) + "seq###";
        } else {
            messageText = part + to_string(i) + "seq###";
        }
        
        string binaryMsg = toBinary(messageText);
        messageList.push_back(binaryMsg);
    }
    
    return messageList;
}

// Main encoding function
void encode(const string& data) {
    if (data.empty()) {
        cout << "Text to be encoded is not provided" << endl;
        return;
    }
    
    vector<string> messageList = hideData(data);
    
    // Create encoded images directory
    filesystem::create_directory("encodedImages");
    
    for (int i = 0; i < imageFiles.size(); i++) {
        string filePath = "./images/" + imageFiles[i];
        cout << "Starting encoding in image " << imageFiles[i] << "..." << endl;
        
        Mat image = imread(filePath);
        if (image.empty()) {
            cout << "Could not read image: " << filePath << endl;
            continue;
        }
        
        Mat encodedImg = encodingInImage(image, messageList[i]);
        string outputPath = "./encodedImages/_encoded" + to_string(i) + ".png";
        imwrite(outputPath, encodedImg);
    }
    
    cout << ".\n.\n.\n." << endl;
    cout << "Encoding completed!" << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <text_file> <images_directory>" << endl;
        return -1;
    }
    
    string textFile = argv[1];
    string imagesDir = argv[2];
    
    // Check if directory exists
    if (!filesystem::exists(imagesDir) || !filesystem::is_directory(imagesDir)) {
        cout << "Directory does not exist: " << imagesDir << endl;
        return -1;
    }
    
    // Count files in directory
    for (const auto& entry : filesystem::directory_iterator(imagesDir)) {
        if (entry.is_regular_file()) {
            imageFiles.push_back(entry.path().filename().string());
            fileCount++;
        }
    }
    
    if (fileCount == 0) {
        cout << "Directory does not contain any files" << endl;
        return -1;
    }
    
    if (fileCount > 9) {
        cout << "Only 9 files allowed maximum" << endl;
        return -1;
    }
    
    // Read text file
    ifstream file(textFile);
    if (!file.is_open()) {
        cout << "Could not open text file: " << textFile << endl;
        return -1;
    }
    
    string data((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    
    // Get password
    cout << "Set a password for encryption: ";
    cin >> password;
    
    string confirmPassword;
    cout << "Confirm your password: ";
    cin >> confirmPassword;
    
    if (password != confirmPassword) {
        cout << "Password does not match" << endl;
        return -1;
    }
    
    encode(data);
    
    return 0;
}
