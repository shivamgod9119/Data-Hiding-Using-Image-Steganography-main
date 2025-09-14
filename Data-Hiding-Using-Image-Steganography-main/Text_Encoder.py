
# importing the required packages
import cv2
import numpy as np
import argparse
from pathlib import Path
import math
import getpass
import os

#Global Variables used in program
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

def encoding_in_image(img, b_msg):
    # Calculating the maximum bytes that can be encoded
    n_bytes = img.shape[0] * img.shape[1] * 3 // 8

    # Checking if the size of the text provided fits in the image provided
    if len(b_msg) > n_bytes:
        #raise ValueError("The size of the secret message is too much. Either reduce the size of the secret message or choose a bigger image.")
        print("The size of the secret message is too much. Either reduce the size of the secret message or choose a bigger image.")   
    data_index = 0

    # convert the message in string format to its equivalent binary format
    # b_msg = to_binary(s_msg)
    len_data = len(b_msg)
    for values in img:
        for pixel in values:

            # convert RGB values to binary format
            r, g, b = to_binary(pixel)

            # Change the least significant bit only if theredir is data to be put in
            if data_index < len_data:
                # hide the data bit into the LSB of red pixel
                pixel[0] = int(r[:-1] + b_msg[data_index], 2)
                data_index += 1
            if data_index < len_data:
                # hide the data bit into the LSB of green pixel
                pixel[1] = int(g[:-1] + b_msg[data_index], 2)
                data_index += 1
            if data_index < len_data:
                # hide the data bit into the LSB of blue pixel
                pixel[2] = int(b[:-1] + b_msg[data_index], 2)
                data_index += 1
                # once the entire data is encoded break out of the loop
            if data_index >= len_data:
                break
    return img
    
    
# Hiding the data inside the picture by using LSB(Least Significant Bit) Image Steganography technique
def hidedata(s_msg):
    # Splitting original string to 5 parts

    length = len(s_msg)
        
    ch = math.ceil(length/count)

    messageList = []
    i=0
    
    # Adding The delimiter string to every list entry
    for x in range(0, count):
        part= s_msg[i:i+ch]
        if(x == 0):
          message_text = part+f"@%#/{password}@%#/{x}seq###"
        else: 
          message_text = part+f"{x}seq###"
        b_msg = to_binary(message_text)
        messageList.append(b_msg)      
        i = i+ch        
            
    return messageList


# Encode the data into the image
#looping all images and encoding message data in those images

def encode(data):

    # Check if data to be encoded is provided
    if len(data) == 0:
        raise ValueError("Text to be encoded is not given")

    
    messageList = hidedata(data)
    i = 0
    for x in d:
        file_path = f"./images/{x}"  
        print(f"Starting encoding in image {x}...")      
        image = cv2.imread(file_path)
        encoded_img = encoding_in_image(image, messageList[i])
        path = '\\'.join(file_path.split('\\')[:-1]) if '\\' in file_path else ''
        encode_filename = file_path.split('\\')[-1].split('.')[0]
        path += ('\\' + encode_filename) if '\\' in file_path else encode_filename
        folderPath = path + './encodedImages/'
        if(x == 0):          
          os.mkdir(folderPath)
          
        cv2.imwrite(f'{folderPath}_encoded{i}.png', encoded_img)
        i=i+1

    print(".\n.\n.\n.")
    print("Encoding completed!")        
        
def dir_path(string):
    if os.path.isdir(string):
        return string
    else:
        raise NotADirectoryError(string)

if __name__ == '__main__':

    # Building a command line argument
    parser = argparse.ArgumentParser(description='Encode any text inside an image of your choice')

    # Collecting the filename of the image and file containing the text to be hidden
    # parser.add_argument('image_filename', type=str,
    #                     help='Enter the filename of your target image into which you want to hide the text')
    parser.add_argument('text_filename', type=str,
                        help='Enter the filename of the text file that contains the text to be hidden')
    
    # argument parser for getting directtory of images
    parser.add_argument('--path', type=dir_path, help='Enter the folder name in which you want to split images')

    args = parser.parse_args()
    filename = open(args.text_filename, 'r')    
    data = filename.read()
    imagesDirectoryPath = args.path

    # Count number of files/images in directory
    
    for path in os.listdir(imagesDirectoryPath):
    # check if current path is a file
        if (os.path.isfile(os.path.join(imagesDirectoryPath, path))):                  
          d.append(path)
          count += 1

    if(count == 0):
        raise EOFError("Directory does not contain any file")
    elif (count > 9): 
        raise ValueError("Only 9 files allowed maximum")

    try:
      password = getpass.getpass(prompt='Set a password for encryption: ')
      confirmPassword = getpass.getpass(prompt='Confirm your password: ')
      if (password != confirmPassword):
        raise Exception
    except Exception as error:
      print('Password does not match', error)    
      exit(0)
    
    
    ## counting of files in directory logic ended

    # filename = args.image_filename

    encode(data)
