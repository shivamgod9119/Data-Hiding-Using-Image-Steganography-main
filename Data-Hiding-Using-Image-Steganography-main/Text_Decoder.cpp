#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>

using namespace std;
using namespace cv;

vector<string> extractedText;
vector<string> imageFiles;
int fileCount = 0;
string password = "";

// Function to extract data from image
void getData(const Mat& img) {
    string binaryData = "";
    
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            Vec3b pixel = img.at<Vec3b>(i, j);
            
            // Extract LSB from blue channel
            binaryData += (pixel[0] & 1) ? '1' : '0';
            // Extract LSB from green channel  
            binaryData += (pixel[1] & 1) ? '1' : '0';
            // Extract LSB from red channel
            binaryData += (pixel[2] & 1) ? '1' : '0';
        }
    }
    
    // Convert binary to characters
    string decodedData = "";
    for (int i = 0; i < binaryData.length(); i += 8) {
        if (i + 8 <= binaryData.length()) {
            string byte = binaryData.substr(i, 8);
            int charCode = 0;
            for (int j = 0; j < 8; j++) {
                charCode = charCode * 2 + (byte[j] - '0');
            }
            decodedData += (char)charCode;
            
            // Check for delimiter
            if (decodedData.length() >= 3 && decodedData.substr(decodedData.length() - 3) == "###") {
                break;
            }
        }
    }
    
    // Process the decoded data
    char seqNo = decodedData[decodedData.length() - 7];
    int sequenceNumber = seqNo - '0';
    
    if (sequenceNumber == 0) {
        // Find password in first sequence
        size_t pos1 = decodedData.find("@%#/");
        size_t pos2 = decodedData.find("@%#/", pos1 + 4);
        
        if (pos1 != string::npos && pos2 != string::npos) {
            string originalStrPassword = decodedData.substr(pos1 + 4, pos2 - pos1 - 4);
            
            if (password != originalStrPassword) {
                throw runtime_error("Password is not valid");
            }
            
            cout << "Initializing extraction...." << endl;
            
            int total = 7 + 8 + originalStrPassword.length();
            string extractedPart = decodedData.substr(0, decodedData.length() - total);
            
            if (extractedText.size() <= sequenceNumber) {
                extractedText.resize(sequenceNumber + 1);
            }
            extractedText[sequenceNumber] = extractedPart;
        }
    } else {
        string extractedPart = decodedData.substr(0, decodedData.length() - 7);
        if (extractedText.size() <= sequenceNumber) {
            extractedText.resize(sequenceNumber + 1);
        }
        extractedText[sequenceNumber] = extractedPart;
    }
}

// Main decoding function
void decode(const Mat& image) {
    getData(image);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <encoded_images_directory>" << endl;
        return -1;
    }
    
    string imagesDir = argv[1];
    
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
    
    // Get password
    cout << "Enter password for decryption: ";
    cin >> password;
    
    try {
        // Process each image
        for (int i = 0; i < fileCount; i++) {
            string filePath = imagesDir + "/" + imageFiles[i];
            Mat image = imread(filePath);
            
            if (image.empty()) {
                cout << "Could not read image: " << filePath << endl;
                continue;
            }
            
            decode(image);
        }
        
        // Combine all extracted text parts
        string result = "";
        for (const string& text : extractedText) {
            result += text;
        }
        
        // Write result to file
        ofstream outFile("Extracted_msg.txt");
        if (outFile.is_open()) {
            outFile << result;
            outFile.close();
            
            cout << ".\n.\n." << endl;
            cout << ">>> Extraction completed." << endl;
        } else {
            cout << "Could not create output file" << endl;
        }
        
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
        return -1;
    }
    
    return 0;
}
