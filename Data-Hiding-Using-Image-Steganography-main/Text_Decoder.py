
# importing the required packages
import cv2
import numpy as np
import argparse
import os
import getpass
#global list
text = []
count=0
d = []
password = ""

# Function to convert the the content into it's relevant binary form
def to_binary(msg):
    msg_type = type(msg)
    if msg_type == str:
        return ''.join([format(ord(i), "08b") for i in msg])
    elif msg_type == bytes or msg_type == np.ndarray:
        return [format(i, "08b") for i in msg]
    elif msg_type == int or msg_type == np.uint8:
        return format(msg, "08b")
    else:
        raise TypeError("Input Type is not supported")
        


# Getting the hidden data from the image
def getData(img):
    b_data = ''
    for values in img:
        for pixel in values:

            # convert RGB values to binary format
            r, g, b = to_binary(pixel)
            # extracting the data from the LSB of red pixel
            b_data += r[-1]
            # extracting the data from the LSB of green pixel
            b_data += g[-1]
            # extracting the data from the LSB of blue pixel
            b_data += b[-1]

    # split by 8-bits
    bytes_data = [b_data[i:i + 8] for i in range(0, len(b_data), 8)]
    # convert the bits into characters
    decoded_data = ""
    for byte in bytes_data:
        decoded_data += chr(int(byte, 2))
        # check if we have reached the delimiter string
        if decoded_data[-3:] == '###':
            break
    # remove the delimiter string from the final decoded message
    seqNo = decoded_data[-7]    
    
    if (seqNo == '0'):
      originalStr = decoded_data[:-7]
      originalStrPassword = originalStr.split('@%#/')[1]
      if (password != originalStrPassword):        
        raise Exception("Password is not valid")
      else:
        print("Initializing extraction....")
      total = 7 + 8 + len(originalStrPassword)
      text.insert(int(seqNo), decoded_data[:-total])
    else:
        text.insert(int(seqNo), decoded_data[:-7])
    # return decoded_data[:-7]


# Decode the data from the image
def decode(filename):
    # img = cv2.imread(image)
    getData(filename)
    # return text

# directory path
def dir_path(string):
    if os.path.isdir(string):
        return string
    else:
        raise NotADirectoryError(string)

if __name__ == '__main__':

    # Building a command line argument
    # parser = argparse.ArgumentParser(
    #     description='Decode text inside an image generated with encoded data by the Encoder.')

    # # Collecting the filename of the image which is to be decoded
    # parser.add_argument('image', type=str,
    #                     help='Enter the filename of the image which contains the encoded text.')

    # args = parser.parse_args()
    # filename = args.image
    parser = argparse.ArgumentParser(description='Enter directory in which decoded files are present')

    # Collecting the filename of the image and file containing the text to be hidden
    # parser.add_argument('image_filename', type=str,
    #                     help='Enter the filename of your target image into which you want to hide the text')    
    
    # argument parser for getting directtory of images
    parser.add_argument('--path', type=dir_path, help='Enter the folder name in which you want to split images')

    args = parser.parse_args()
    imagesDirectoryPath = args.path

    # Count number of files/images in directory
    
    for path in os.listdir(imagesDirectoryPath):
    # check if current path is a file
        if (os.path.isfile(os.path.join(imagesDirectoryPath, path))):                  
          d.append(path)          
          count += 1
    
    try:
      password = getpass.getpass(prompt='Enter password for decryption: ')                    
    except Exception as error:
      print('ERROR', error)
      exit(0)


    result=""
    for x in range(0,count):
        file_path = f'./{imagesDirectoryPath}/{d[x]}'
        image = cv2.imread(file_path)
        decode(image)
    
    for x in range(0,count):
        result = result + text[x]       
    # Print the decoded text into a text file in the same directory as the code file 

              
    
    file = open('Extracted_msg.txt', 'w')
    file.write(result)
    print(".\n.\n.")
    print(">>> Extraction completed.")
    file.close()
