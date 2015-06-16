#!/usr/bin/python

import matplotlib.pyplot as plt
import numpy as np

#plt.figure()
#
#g = [256, 512, 1024, 2048, 4096]
#mag_ptr = [5.10216e-05, 0.000178099, 0.000814915, 0.0054729, 0.020463];
#mag_mapa = [0.000445127, 0.00205517, 0.00818896, 0.0332129, 0.113741];
#
#plt.plot(g, mag_ptr, 'b-', linewidth=2, label='Pointer')
#plt.plot(g, mag_mapa, 'r-', linewidth=2, label='Mapped')
#
#plt.legend(loc=2)
#plt.grid(True)
#plt.xlabel('grid dim (n^3)', fontweight='bold')
#plt.ylabel('time (sec)', fontweight='bold')
#plt.title('Magnitude Computation', fontweight='bold')
#
#plt.figure()
#
#grad_ptr = [0.000118017, 0.000510931, 0.0026722, 0.01109, 0.04072];
#grad_mapa =[0.00192618, 0.00775194, 0.0313561, 0.108544, 0.445552];
#
#plt.plot(g, grad_ptr, 'b-', linewidth=2, label='Pointer')
#plt.plot(g, grad_mapa, 'r-', linewidth=2, label='Mapped')
#
#plt.legend(loc=2)
#plt.grid(True)
#plt.xlabel('grid dim (n^3)', fontweight='bold')
#plt.ylabel('time (sec)', fontweight='bold')
#plt.title('Gradient Computation', fontweight='bold')
#

fig,ax = plt.subplots()

w = 0.25
label = ['mag', 'grad']
ids = np.array([0, 1])/0.5
ptr_t = np.array([0.020463, 0.04072])
map_t = np.array([0.113741, 0.445552])

p_r = ax.bar(ids, ptr_t, w, color='r')
m_r = ax.bar(ids+w, map_t, w, color='y')

ax.set_ylabel('time (sec)', fontweight='bold')
ax.set_title('Computation Times\nMapped Array vs Pointer', fontweight='bold')
ax.set_xticks(ids + w)
ax.set_xticklabels( ('Magnitude', 'Gradient') , fontweight='bold')
ax.legend((p_r, m_r), ('Pointer', 'Mapped Array'), loc=2 )
ax.yaxis.grid(True)
plt.xlim([ids[0]-w/2.0, ids[1]++2.0*w+w/2.0])

print 'pointers are faster by'
print map_t/ptr_t


plt.show()
