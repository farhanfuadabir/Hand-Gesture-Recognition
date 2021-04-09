import numpy as np
import pandas as pd
from sklearn import tree
from sklearn import svm
from sklearn.ensemble import RandomForestClassifier
from sklearn.neighbors import KNeighborsClassifier
from scipy.signal import medfilt
import vlc
import serial
import time
import os

g = ["Painting", "Thank You", "Sorry"]
audio = [0, 0, 0];
audio[0] = vlc.MediaPlayer("painting.mp3")
audio[1] = vlc.MediaPlayer("thank_you.mp3")
audio[2] = vlc.MediaPlayer("sorry.mp3")

X_train = np.array(pd.read_csv('feature_vector.csv'))
y_train = np.array(pd.read_csv('window_labels.csv'))

#clf = tree.DecisionTreeClassifier()
#clf = RandomForestClassifier(n_estimators=100, max_depth=7, random_state=0)
clf = KNeighborsClassifier(n_neighbors=7);
clf.fit(X_train, y_train.ravel());

ser = serial.Serial('/dev/ttyUSB0', 115200)
count = 0
result = 0
prevResult = 0
window_size = 10

x = np.zeros(window_size)
y = np.zeros(window_size)
z = np.zeros(window_size)

while True:
    incoming = ser.readline().decode('ascii')
    values = incoming.split(',')
    x = np.append(x, float(values[0]))
    y = np.append(y, float(values[1]))
    z = np.append(z, float(values[2]))

    if x.size > window_size:
    	x = np.delete(x, 0)
    	y = np.delete(y, 0)
    	z = np.delete(z, 0)
    
    x_std = np.std(x)
    y_std = np.std(y)
    z_std = np.std(z)

    # features = 0
    # for i in range(0, window_size):
    # 	features = np.sqrt(x_std ** 2 + y_std ** 2 + z_var ** 2)
    #features = np.concatenate(([x_std], [z_std]))
    features = np.array(np.array(x_std) - np.array(z_std))
    os.system('clear')
    result = clf.predict([[features]])
    if prevResult == result:
    	count = count + 1
    	if count > 16:
    		print(g[result[0]])
    		audio[result[0]].play()
    		time.sleep(0.7)
    		audio[result[0]].stop()
    		count = 0
    	else:
    		print("Looking For Gesture...")
    else :
    	count = 0
    	print("Looking For Gesture...")
    prevResult = result
