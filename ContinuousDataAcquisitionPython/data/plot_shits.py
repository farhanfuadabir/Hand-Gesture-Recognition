import numpy as np 
import matplotlib.pyplot as plt 

x = np.array([2, 3, 4, 5, 6, 7, 8, 9, 10])
y = np.array([96.6, 97.96, 97.96, 97.96, 97.96, 98.64, 98.64, 98.64, 98.64])

plt.plot(x, y)
plt.title("K value vs Accuracy(%)")
plt.xlabel("K values")
plt.ylabel("Accuracy(%)")
plt.show()

x = np.array([4, 6, 8, 10, 12, 14, 16, 18, 20])
y = np.array([89.92, 95.92, 96.72, 98.64, 97.54, 99.04, 98.91, 100, 100])

plt.plot(x, y)
plt.title("Window Length vs Accuracy(%)")
plt.xlabel("Window Length")
plt.ylabel("Accuracy(%)")
plt.show()
