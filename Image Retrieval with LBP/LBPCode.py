import pickle
import cv2, os
import pandas as pd
import numpy as np
import matplotlib.image as img
from matplotlib import pyplot as plt
import matplotlib.cm as cm


# 18011092 ARDA KASIKCI
# !!!!! Kodun calismasi icin samples klasoru ile kod dosyasının aynı dizinde bulunmasi gerekmektedir.!!!!

# Image Class for save image path and histogram together.
class ImageWithHist:
    def __init__(self, path, hist, row, column, dim):
        self.path = path
        self.name = path.split("/")[2]
        self.hist = hist
        self.row = row
        self.column = column
        self.dim = dim


# YCbCr Y Luminance Matrix Conversion
def rgbtogray(image, row, column):
    r = image[:, :, 0]
    g = image[:, :, 1]
    b = image[:, :, 2]
    image_gray = np.zeros((row, column))
    for i in range(0, row):
        for j in range(0, column):
            image_gray[i, j] = int(0.299 * r[i, j] + 0.587 * g[i, j] + 0.114 * b[i, j])

    return image_gray


# LBP Compare pixels
def LBPCompare(center, other):
    if center > other:
        return 1
    else:
        return 0


# LBP Process
def LBPImagetoHist(gray, row, column):
    binary = np.zeros((8), dtype=int)
    hist = np.zeros((256), dtype=int)

    for i in range(1, row - 1):
        for j in range(1, column - 1):

            bin_val = 0
            count = 0
            binary[7] = LBPCompare(gray[i, j], gray[i - 1, j - 1])
            binary[6] = LBPCompare(gray[i, j], gray[i - 1, j])
            binary[5] = LBPCompare(gray[i, j], gray[i - 1, j + 1])
            binary[4] = LBPCompare(gray[i, j], gray[i, j - 1])
            binary[3] = LBPCompare(gray[i, j], gray[i, j + 1])
            binary[2] = LBPCompare(gray[i, j], gray[i + 1, j - 1])
            binary[1] = LBPCompare(gray[i, j], gray[i + 1, j])
            binary[0] = LBPCompare(gray[i, j], gray[i + 1, j + 1])

            for us in range(8):
                bin_val = bin_val + (binary[us] * pow(2, us))
                if us < 7:
                    if binary[us] != binary[us + 1]:
                        count = count + 1

            if count <= 2:
                hist[bin_val] = hist[bin_val] + 1
            else:
                hist[bin_val] = 0  # Uniform LBPye uymayan değerler 0 yapılır.
    # HISTOGRAM NORMALIZATION
    hist = hist / ((row - 2) * (column - 2))

    return hist


# Man. Distance Function
def Manhattanof2Hist(trainedHist, testHist):
    diff = 0.0
    for i in range(256):
        diff = diff + abs(trainedHist[i] - testHist[i])
    return diff


# Find 3 minimum distance image objects
def find3CloseImg(objectList, testObject):
    first_minimum = 1
    firs_object = ImageWithHist("-/-/-", 1, 1, 1, 1)
    second_object = ImageWithHist("-/-/-", 1, 1, 1, 1)
    third_object = ImageWithHist("-/-/-", 1, 1, 1, 1)
    second_minimum = 1
    third_minimum = 1

    for object in objectList:

        distance = Manhattanof2Hist(object.hist, testObject.hist)
        if distance < first_minimum:
            third_minimum = second_minimum;
            third_object = second_object
            second_minimum = first_minimum;
            second_object = firs_object
            first_minimum = distance;
            firs_object = object

        elif distance < second_minimum:
            third_minimum = second_minimum;
            third_object = second_object
            second_minimum = distance;
            second_object = object

        elif distance < third_minimum:
            third_minimum = distance;
            third_object = object

    fig = plt.figure(figsize=(15, 10))
    fig.add_subplot(2, 3, 2)
    image = img.imread(testObject.path)
    # Show Image
    plt.imshow(image)
    plt.axis("off")
    plt.title("Test Image: " + testObject.name)

    fig.add_subplot(2, 3, 4)
    image = img.imread(firs_object.path)
    # Show Image
    plt.imshow(image)
    plt.axis("off")
    plt.title(firs_object.path.split("/")[3] + " Distance:" + str(("%.2f" % first_minimum)))

    fig.add_subplot(2, 3, 5),
    image = img.imread(second_object.path)
    # Show Image
    plt.imshow(image)
    plt.axis("off")
    plt.title(second_object.path.split("/")[3] + " Distance:" + str(("%.2f" % second_minimum)))

    fig.add_subplot(2, 3, 6),
    image = img.imread(third_object.path)
    # Show Image
    plt.imshow(image)
    plt.axis("off")
    plt.title(third_object.path.split("/")[3] + " Distance:" + str(("%.2f" % third_minimum)))

    plt.show()

    return firs_object, first_minimum, second_object, second_minimum, third_object, third_minimum


# Read Train image and create object list
def createTrainImgList():
    folder_dir = "samples/train"
    image_objects = []
    for folder in os.listdir(folder_dir):
        new_folder = folder_dir + "/" + folder
        for inFolderImg in os.listdir(new_folder):
            if inFolderImg.endswith(".jpg") or inFolderImg.endswith(".jpeg"):
                inFolderImg_path = new_folder + "/" + inFolderImg
                image = img.imread(inFolderImg_path)
                row, column, dim = image.shape
                image_gray = rgbtogray(image, row, column)
                hist = LBPImagetoHist(image_gray, row, column)
                image_objects.append(ImageWithHist(inFolderImg_path, hist, row, column, dim))
                print(inFolderImg + " goruntusu yazildi.")

    return image_objects


def getTestImage(path):
    test_img_path = path
    test_img = img.imread(test_img_path)
    plt.axis("off")
    plt.title("Test Image")
    plt.imshow(test_img)
    t_row, t_column, t_dim = test_img.shape
    test_gray = rgbtogray(test_img, t_row, t_column)
    test_hist = LBPImagetoHist(test_gray, t_row, t_column)
    test_obj = ImageWithHist(test_img_path, test_hist, t_row, t_column, t_dim)
    return test_obj


def writeToFile(image_objects):
    # Object List Write
    pfile = open("img_hist_list.pickle", mode="wb")
    pickle.dump(image_objects, pfile)
    pfile.close()


def readFromFile():
    # Object List Read
    pfile = open("img_hist_list.pickle", mode="rb")
    recalled_img_objects = pickle.load(pfile)
    pfile.close()

    return recalled_img_objects


if __name__ == '__main__':
    # !!!!! Kodun calismasi icin samples klasoru ile kod dosyasının aynı dizinde bulunmasi gerekmektedir.!!!!

    # Read image control code and image gray conversion
    img_path = "samples/testRaporaEklenecek/striped_0002.jpg"
    image = img.imread(img_path)

    row, column, dim = image.shape

    r = image[:, :, 0]
    g = image[:, :, 1]
    b = image[:, :, 2]

    # Gray Matrix of Image
    image_gray = rgbtogray(image, row, column)

    #####################################################
    # Create LBP Hist of Image
    hist = LBPImagetoHist(image_gray, row, column)

    # Print hist matrix
    print("---LBP Histogram of Image---")
    print(hist)

    ######################################################
    # Crate image object
    ornek_object = ImageWithHist(img_path, hist, row, column, dim)

    ######################################################
    # Read all train images and save its in object list
    # image_objects = createTrainImgList() #This function will be used when pickle file not created.

    #####################################################
    # Write ImageWithHist Object List to file using Pickle Library
    # writeToFile(image_objects) #This function will be used when pickle file not created.

    #####################################################
    # Read ImageWithHist Object List from file using Pickle Library
    recalled_img_objects = readFromFile()

    #####################################################
    # Get Test image from path
    test_obj = getTestImage("samples/testRaporaEklenecek/banded_0025.jpg")

    #####################################################
    # Find 3 Close Image for test image using recalled_image_objects list which is retrieves from pickle file
    find3CloseImg(recalled_img_objects, test_obj)
