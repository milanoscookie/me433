import numpy as np
import cv2 as cv
from matplotlib import pyplot as plt
 
# load in the image
img = cv.imread('line.jpg')
 
# guassian blur
# m,n filter
m = n = 5
gauss = np.ones((m,n),np.float32)/(m*n)
dst_blur = cv.filter2D(img,-1,gauss)

# look for verticle edges
# sobel edge detection
vedge = np.array([
    [-2/8, -1/5, 0, 1/5, 2/8],
    [-2/5, -1/2, 0, 1/2, 2/5],
    [-2/4, -1/1, 0, 1/1, 2/4],
    [-2/5, -1/2, 0, 1/2, 2/5],
    [-2/8, -1/5, 0, 1/5, 2/8],
                  ])

dst_edge = cv.filter2D(dst_blur,-1,vedge)

plt.subplot(121),plt.imshow(img),plt.title('Original')
plt.xticks([]), plt.yticks([])
plt.subplot(122),plt.imshow(dst_edge),plt.title('Averaging')
plt.xticks([]), plt.yticks([])
plt.savefig('edge_det.png')