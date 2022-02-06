
"""
TITLE: grouped bar plots
@author: Oluwatosin S. Oluseyi , December 11, 2021; modified: February 06, 2022
"""

import numpy as np
import matplotlib.pyplot as plt

fig, ax = plt.subplots()

BUF = [2.1127, 2.8254, 7.5188, 45.6981]
USM = [2.1404, 2.5239, 5.9738, 31.1607]

n = len(BUF)
x = np.arange(n)
width = 0.2

plt.bar(x, BUF, color = '#04D8B2',
        width = width, edgecolor = 'white', label='getrs_buf')

plt.bar(x + width, USM, color = '#15B01A',
        width = width, edgecolor = 'white', label ='getrs_usm')


plt.yscale('log')
plt.ylabel("Time (in seconds)")
plt.xticks(x+width,['500','2500','5000','10000'])
plt.title("Homogeneous - GPU selector")
plt.legend(loc="upper left")

# BAR ANNOTATION (see https://www.pythoncharts.com/matplotlib/grouped-bar-charts-matplotlib/)
for bar in ax.patches:
  # The text annotation for each bar should be its height.
  bar_value = bar.get_height()
  # Format text for bars
  text = f'{bar_value}'
  # This will give the middle of each bar on the x-axis.
  text_x = bar.get_x() + bar.get_width() / 2
  # get_y() is where the bar starts so we add the height to it.
  text_y = bar.get_y() + bar_value
  # make text the same color as the bar
  bar_color = bar.get_facecolor()
  # If you want a consistent color, you can just set it as a constant, e.g. #222222
  ax.text(text_x, text_y, text, ha='center', va='bottom', color=bar_color,
          size=5)
  
plt.show()
