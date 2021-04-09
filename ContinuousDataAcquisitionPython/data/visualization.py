#!/usr/bin/env python
# coding: utf-8

# In[1]:


import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.signal import medfilt


# In[2]:


dataset = pd.read_csv('train_data.csv')
dataset.columns = ['accX', 'accY', 'accZ']


# In[3]:


accX = np.array(dataset['accX'])
accY = np.array(dataset['accY'])
accZ = np.array(dataset['accZ'])


# In[14]:


plt.plot(accX + 150)
plt.plot(accY)
plt.plot(accZ - 150)
plt.title('Raw Data')
plt.xlabel('Time')
plt.ylabel('Acceleration')
plt.legend(['AccX', 'AccY', 'AccZ'], loc = 1)
plt.show()


# In[15]:


accX = medfilt(accX)
accY = medfilt(accY)
accZ = medfilt(accZ)
plt.plot(accX + 150)
plt.plot(accY)
plt.plot(accZ - 150)
plt.title('Filtered Data')
plt.xlabel('Time')
plt.ylabel('Acceleration')
plt.legend(['AccX', 'AccY', 'AccZ'], loc = 1)
plt.show()


# In[16]:


plt.plot(accX[100:300] + 100)
plt.plot(accY[100:300])
plt.plot(accZ[100:300] - 100)
plt.title('Acceleration for \'Painting\' Gesture')
plt.legend(['Acc-X', 'Acc-Y', 'Acc-Z'], loc = 1)
plt.xlabel('Time')
plt.ylabel('Acceleration')
plt.show()


# In[17]:


plt.plot(accX[1100:1300] + 100)
plt.plot(accY[1100:1300])
plt.plot(accZ[1100:1300] - 100)
plt.title('Acceleration for \'Thank You\' Gesture')
plt.xlabel('Time')
plt.ylabel('Acceleration')
plt.legend(['Acc-X', 'Acc-Y', 'Acc-Z'], loc = 1)
plt.show()


# In[18]:


plt.plot(accX[2100:2300] + 200)
plt.plot(accY[2100:2300])
plt.plot(accZ[2100:2300] - 200)
plt.title('Acceleration for \'Sorry\' Gesture')
plt.xlabel('Time')
plt.ylabel('Acceleration')
plt.legend(['Acc-X', 'Acc-Y', 'Acc-Z'], loc = 1)
plt.show()


# In[20]:


features = pd.read_csv('feature_vector.csv')
plt.plot(features)
plt.title('Feature')
plt.xlabel('Time')
plt.ylabel('STD(X) - STD(Z)')
plt.show()


# In[ ]:




