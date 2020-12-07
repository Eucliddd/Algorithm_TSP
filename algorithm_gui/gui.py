import tkinter as tk
import matplotlib
from matplotlib.backend_bases import key_press_handler
from matplotlib.backends._backend_tk import NavigationToolbar2Tk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import os

root = tk.Tk()

root.title("TSP")

root.geometry('1024x768')
dataset = []
file = open("nameList.txt", 'r')

while True:
    line = file.readline()
    if not line:
        break
    dataset.append(line.strip())

algorithm = ['AS', 'SA', '2-opt', 'Nearest', 'GA', 'Tabu', 'Chain']
v1 = tk.StringVar(root)
v1.set('please input')
v2 = tk.StringVar(root)
v2.set('please input')
v3 = tk.StringVar(root)
v3.set('')

fm = tk.Frame(root)
fm.pack(side=tk.LEFT, fill=tk.Y)

om1 = tk.OptionMenu(fm, v1, *dataset)
om1.pack(anchor=tk.NW, pady=100, padx=50)
om2 = tk.OptionMenu(fm, v2, *algorithm)
om2.pack(anchor=tk.W, pady=100, padx=50)

fm1 = tk.Frame(root)
fm1.pack(side=tk.TOP, fill=tk.X)
l = tk.Label(fm1, textvariable=v3, font=('Times New Roman', 18))
l.pack(ipady=20)

f = Figure(figsize=(5, 4), dpi=100)
f_plot = f.add_subplot(111)


def press_button():
    str1 = v1.get()
    str2 = v2.get()
    if str1 == 'please input' or str2 == 'please input':
        print('please select')
        return
    cmd = 'TSP %s %s' % (str1, str2)
    os.system(cmd)
    infile = open("data.txt", 'r')
    length = infile.readline().strip()
    time = infile.readline().strip()
    v3.set("%s m, %s sec" % (length, time))
    x = []
    y = []
    while True:
        line = infile.readline()
        if not line:
            break
        x_data, y_data = [float(i) for i in line.split()]
        x.append(x_data)
        y.append(y_data)
    infile.close()
    f_plot.clear()
    f_plot.plot(x, y)
    canvs.draw()


canvs = FigureCanvasTkAgg(f, root)
canvs.get_tk_widget().pack(anchor=tk.SE, side=tk.TOP, expand=tk.TRUE, fill=tk.BOTH)
button = tk.Button(fm, text='plot', command=press_button, width=10)
button.pack(anchor=tk.SW, pady=100, padx=50)
toolbar = NavigationToolbar2Tk(canvs, root)
toolbar.update()


def on_key_event(event):
    key_press_handler(event, canvs, toolbar)


canvs.mpl_connect('key_press_event', on_key_event)
root.mainloop()
